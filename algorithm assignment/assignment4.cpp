#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <ctime>
#include <fstream>
#include <climits>
#include <omp.h> // OpenMP for parallelism

using namespace std;

// Define Edge structure
struct Edge {
    int src, dest, weight;
};

// Define Graph class
class Graph {
public:
    int V, E;
    vector<Edge> edges;

    Graph(int V, int E) {
        this->V = V;
        this->E = 0; // Initialize edge count to 0
    }

    void addEdge(int src, int dest, int weight) {
        Edge edge;
        edge.src = src;
        edge.dest = dest;
        edge.weight = weight;
        edges.push_back(edge);
        ++E; // Increase edge count when adding an edge
    }
};

// Calculate cut weight for a given partition and vertex index
int calculateCutWeight(const vector<int>& partition, const Graph& graph, int vertexIndex) {
    int cutWeight = 0;
    for (const auto& edge : graph.edges) {
        if (partition[edge.src] != partition[edge.dest] && (edge.src == vertexIndex || edge.dest == vertexIndex)) {
            cutWeight += edge.weight;
        }
    }
    return cutWeight;
}

// Roulette wheel selection
vector<int> rouletteWheelSelection(const vector<pair<int, vector<int>>>& fitness) {
    vector<int> selectedIndices;
    vector<double> cumulativeProbabilities(fitness.size() + 1, 0.0);

    double totalFitness = 0.0;
    for (const auto& fit : fitness) {
        totalFitness += fit.first;
    }

#pragma omp parallel
    {
        default_random_engine generator(time(NULL)); // Use the same seed for all threads
        uniform_real_distribution<double> distribution(0.0, 1.0);

        vector<int> privateSelectedIndices; // Each thread has its own private copy

#pragma omp for
        for (size_t i = 0; i < fitness.size(); ++i) {
            double randNum = distribution(generator);
            for (size_t j = 0; j < cumulativeProbabilities.size() - 1; ++j) {
                if (randNum >= cumulativeProbabilities[j] && randNum < cumulativeProbabilities[j + 1]) {
                    privateSelectedIndices.push_back(j);
                    break;
                }
            }
        }

#pragma omp critical
        selectedIndices.insert(selectedIndices.end(), privateSelectedIndices.begin(), privateSelectedIndices.end());
    }

    return selectedIndices;
}

// K-point crossover
void kPointCrossover(const vector<int>& parent1, const vector<int>& parent2, vector<int>& child1, vector<int>& child2, int k) {
    int V = parent1.size();
    vector<int> crossoverPoints(k);

    default_random_engine generator(time(NULL)); // Use the same seed for all crossover operations
    uniform_int_distribution<int> distribution(0, V - 1);

    for (int i = 0; i < k; ++i) {
        crossoverPoints[i] = distribution(generator);
    }
    sort(crossoverPoints.begin(), crossoverPoints.end());

    int idx = 0;
    for (int i = 0; i < V; ++i) {
        if (idx < k && i >= crossoverPoints[idx] && (idx == k - 1 || i < crossoverPoints[idx + 1])) {
            idx++;
        }
        if (idx % 2 == 0) {
            child1[i] = parent1[i];
            child2[i] = parent2[i];
        }
        else {
            child1[i] = parent2[i];
            child2[i] = parent1[i];
        }
    }
}


// Uniform mutation
void uniformMutation(vector<int>& individual, double mutationRate) {
    default_random_engine generator(time(NULL));
    uniform_real_distribution<double> distribution(0.0, 1.0);

    for (int i = 0; i < individual.size(); ++i) {
        if (distribution(generator) < mutationRate) {
            individual[i] = 1 - individual[i]; // Flip the bit
        }
    }
}

// Local optimization: 1-bit flip neighborhood search
void localOptimization(vector<int>& partition, const Graph& graph) {
    bool improved = true;
    while (improved) {
        improved = false;
        vector<int> perm(graph.V);
        for (int i = 0; i < graph.V; ++i) {
            perm[i] = i;
        }
        random_shuffle(perm.begin(), perm.end());

        for (int i : perm) {
            int newCutWeight = calculateCutWeight(partition, graph, i);
            if (newCutWeight > 0) { // Improvement found
                partition[i] = 1 - partition[i]; // Flip the bit
                improved = true;
            }
        }
    }
}

// Genetic Algorithm for Max-Cut Problem with parallelism and island model
vector<int> parallelIslandGeneticAlgorithmMaxCut(const Graph& graph, int populationSize, int generations, int islandCount) {
    vector<vector<vector<int>>> populations(islandCount, vector<vector<int>>(populationSize / islandCount, vector<int>(graph.V)));
    int bestCutWeight = INT_MIN;
    vector<int> globalBestPartition;

#pragma omp parallel for
    for (int i = 0; i < islandCount; ++i) {
        // Initialize population randomly for each island
        for (auto& individual : populations[i]) {
            for (int j = 0; j < graph.V; ++j) {
                individual[j] = rand() % 2; // 0 or 1 representing partition
            }
        }

        vector<int> localBestPartition;
        int localBestCutWeight = INT_MIN;

        // Main loop
        for (int gen = 0; gen < generations; ++gen) {
            // Evaluate fitness of each individual
            vector<pair<int, vector<int>>> fitness; // pair<cutWeight, partition>
            for (const auto& individual : populations[i]) {
                int cutWeight = calculateCutWeight(individual, graph, i);
                fitness.push_back({ cutWeight, individual });
            }

            // Find local best partition for each island
            for (const auto& fit : fitness) {
                if (fit.first > localBestCutWeight) {
                    localBestCutWeight = fit.first;
                    localBestPartition = fit.second;
                }
            }

            // Local optimization
            localOptimization(localBestPartition, graph);

            // Roulette wheel selection
            vector<int> selectedIndices = rouletteWheelSelection(fitness);

            // Crossover and mutation
            for (int j = 0; j < selectedIndices.size(); j += 2) {
                int parent1Idx = selectedIndices[j];
                int parent2Idx = selectedIndices[j + 1];
                vector<int> child1(graph.V);
                vector<int> child2(graph.V);
                kPointCrossover(fitness[parent1Idx].second, fitness[parent2Idx].second, child1, child2, 2); // Using 2-point crossover
                uniformMutation(child1, 0.01); // Mutation rate set to 1%
                uniformMutation(child2, 0.01);
                populations[i][parent1Idx] = child1;
                populations[i][parent2Idx] = child2;
            }
        }

        // Update global best partition
#pragma omp critical
        {
            if (localBestCutWeight > bestCutWeight) {
                bestCutWeight = localBestCutWeight;
                globalBestPartition = localBestPartition;
            }
        }
    }

    return globalBestPartition;
}




int main() {
    // Set random seed
    srand(time(0));

    // Read input from file
    ifstream input("weighted_500.txt");

    if (input.fail()) {
        cout << "파일을 열 수 없습니다." << endl;
        return 1;
    }

    // Read input
    int V, E;
    input >> V >> E;
    Graph graph(V, E);
    for (int i = 0; i < E; ++i) {
        int src, dest, weight;
        input >> src >> dest >> weight;
        graph.addEdge(src - 1, dest - 1, weight); // Adjust vertex indices to start from 0
    }

    input.close();

    // Solve Max-Cut Problem using Parallel Island Genetic Algorithm with population size of 500, 4 islands, and 1000 generations
    vector<int> bestPartition = parallelIslandGeneticAlgorithmMaxCut(graph, 500, 1000, 4);

    // Write output to file
    ofstream output;
    output.open("maxcut.out");

    // Output S' partition
    for (int i = 0; i < V; ++i) {
        if (bestPartition[i] == 0) { // Partition S'
            output << i + 1 << " "; // Adjust back to 1-based indexing
        }
    }

    output.close(); // 파일을 닫습니다.

    return 0;
}
