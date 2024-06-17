#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <ctime>
#include <fstream>
#include <climits>

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

// Calculate cut weight for a given partition
int calculateCutWeight(const vector<int>& partition, const Graph& graph) {
    int cutWeight = 0;
    for (const auto& edge : graph.edges) {
        if (partition[edge.src] != partition[edge.dest]) {
            cutWeight += edge.weight;
        }
    }
    return cutWeight;
}

// Calculate cut weight between partitions S and S'
int calculateCutWeightBetweenPartitions(const vector<int>& partition, const vector<Edge>& edges) {
    int cutWeight = 0;
    for (const auto& edge : edges) {
        if (partition[edge.src] != partition[edge.dest]) {
            cutWeight += edge.weight;
        }
    }
    return cutWeight;
}


// Genetic Algorithm for Max-Cut Problem
vector<int> geneticAlgorithmMaxCut(const Graph& graph, int populationSize, int generations) {
    // Initialize population randomly
    vector<vector<int>> population(populationSize, vector<int>(graph.V));
    for (auto& individual : population) {
        for (int i = 0; i < graph.V; ++i) {
            individual[i] = rand() % 2; // 0 or 1 representing partition
        }
    }

    vector<int> bestPartition;
    int bestCutWeight = INT_MIN;

    // Main loop
    for (int gen = 0; gen < generations; ++gen) {
        // Evaluate fitness of each individual
        vector<pair<int, vector<int>>> fitness; // pair<cutWeight, partition>
        for (const auto& individual : population) {
            int cutWeight = calculateCutWeight(individual, graph);
            fitness.push_back({ cutWeight, individual });
        }

        // Sort individuals by fitness
        sort(fitness.begin(), fitness.end(), greater<pair<int, vector<int>>>());

        // Update best partition
        if (fitness[0].first > bestCutWeight) {
            bestCutWeight = fitness[0].first;
            bestPartition = fitness[0].second;
        }

        // Select top individuals for reproduction (elitism)
        vector<vector<int>> selected(populationSize / 2);
        for (int i = 0; i < populationSize / 2; ++i) {
            int idx1 = rand() % populationSize;
            int idx2 = rand() % populationSize;
            selected[i] = (fitness[idx1].first > fitness[idx2].first) ? fitness[idx1].second : fitness[idx2].second;
        }

        // Crossover and mutation
        for (int i = 0; i < populationSize / 2; i += 2) {
            // Perform crossover
            int crossoverPoint = rand() % graph.V;
            for (int j = 0; j < crossoverPoint; ++j) {
                swap(selected[i][j], selected[i + 1][j]);
            }

            // Perform mutation
            int mutationPoint = rand() % graph.V;
            selected[i][mutationPoint] = 1 - selected[i][mutationPoint];
            mutationPoint = rand() % graph.V;
            selected[i + 1][mutationPoint] = 1 - selected[i + 1][mutationPoint];
        }

        // Update population with new individuals
        for (int i = 0; i < populationSize / 2; ++i) {
            population[i + populationSize / 2] = selected[i];
        }
    }

    return bestPartition;
}

int main() {
    // Set random seed
    srand(time(0));

    ifstream input("unweighted_50.txt");

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
        while (input >> src >> dest >> weight) {
            graph.addEdge(src - 1, dest - 1, weight); // Adjust vertex indices to start from 0
        }
    }

    input.close();

    // Solve Max-Cut Problem using Genetic Algorithm
    vector<int> bestPartition = geneticAlgorithmMaxCut(graph, 100, 1000);

    ofstream output;
    output.open("maxcut.out");

    // Calculate cut weight between partitions S and S'
    for(int i = 0; i < 500; i++)
    { 
        int cutWeight = calculateCutWeightBetweenPartitions(bestPartition, graph.edges);
        output << cutWeight << endl;
    }
    
    output.close();

    return 0;
}
