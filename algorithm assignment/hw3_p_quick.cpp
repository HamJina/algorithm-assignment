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

// Partition function for Paranoid quick sort
int partition(vector<pair<int, vector<int>>>& arr, int low, int high) {
    int pivot = arr[high].first;
    int i = (low - 1);

    for (int j = low; j <= high - 1; j++) {
        if (arr[j].first >= pivot) {
            i++;
            swap(arr[i], arr[j]);
        }
    }
    swap(arr[i + 1], arr[high]);
    return (i + 1);
}

// Paranoid quick sort function
void quickSort(vector<pair<int, vector<int>>>& arr, int low, int high) {
    if (low < high) {
        int pi = partition(arr, low, high);
        quickSort(arr, low, pi - 1);
        quickSort(arr, pi + 1, high);
    }
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

        // Sort individuals by fitness using Paranoid quick sort
        quickSort(fitness, 0, fitness.size() - 1);

        // Update best partition
        if (fitness[0].first > bestCutWeight) {
            bestCutWeight = fitness[0].first;
            bestPartition = fitness[0].second;
        }

        // Select top individuals for reproduction
        vector<vector<int>> selected(populationSize / 2);
        for (int i = 0; i < populationSize / 2; ++i) {
            selected[i] = fitness[i].second;
        }

        // Crossover and mutation
        for (int i = 0; i < populationSize / 2; i += 2) {
            // Perform crossover (k-point cross-over)
            int k = 2; // Number of points for k-point cross-over
            vector<int> crossoverPoints(k);
            for (int j = 0; j < k; ++j) {
                crossoverPoints[j] = rand() % graph.V; // Randomly select crossover points
            }
            sort(crossoverPoints.begin(), crossoverPoints.end()); // Sort crossover points
            int idx = 0;
            for (int j = 0; j < graph.V; ++j) {
                if (j >= crossoverPoints[idx] && idx < k - 1) {
                    idx++;
                }
                if (idx % 2 == 0) {
                    swap(selected[i][j], selected[i + 1][j]);
                }
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

    ifstream input("chimera_946.txt");

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

    // Solve Max-Cut Problem using Genetic Algorithm with population size of 500
    vector<int> bestPartition = geneticAlgorithmMaxCut(graph, 500, 1000);



    ofstream output;
    output.open("maxcut.out");



    // Output S' partition
    for (int i = 0; i < V; ++i) {
        if (bestPartition[i] == 0) { // Partition S'
            output << i + 1 << " "; // Adjust back to 1-based indexing
        }
    }

    output << endl;

    return 0;
}
