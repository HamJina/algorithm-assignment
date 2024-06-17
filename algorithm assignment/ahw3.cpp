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

// Genetic Algorithm for Max-Cut Problem
pair<vector<int>, int> geneticAlgorithmMaxCut(const Graph& graph, int populationSize, int generations) {
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

        // Sort individuals by fitness using counting sort
        vector<pair<int, vector<int>>> sortedPopulation(populationSize);
        vector<int> count(2, 0); // Count array for 0 and 1
        for (const auto& fit : fitness) {
            ++count[fit.second[0]];
        }
        for (vector<int>::size_type i = 1; i < count.size(); ++i) {
            count[i] += count[i - 1];
        }
        for (int i = fitness.size() - 1; i >= 0; --i) {
            sortedPopulation[count[fitness[i].second[0]] - 1] = fitness[i];
            --count[fitness[i].second[0]];
        }

        // Update best partition
        if (sortedPopulation[0].first > bestCutWeight) {
            bestCutWeight = sortedPopulation[0].first;
            bestPartition = sortedPopulation[0].second;
        }

        // Select top individuals for reproduction 
        vector<vector<int>> selected(populationSize / 2);
        for (int i = 0; i < populationSize / 2; ++i) {
            selected[i] = sortedPopulation[i].second;
        }

        // Crossover and mutation
        // (Same as before)

        // Update population with new individuals
        // (Same as before)
    }

    return { bestPartition, bestCutWeight };
}

int main() {
    // Set random seed
    srand(time(0));

    ifstream input("random_500.txt");

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

    // Solve Max-Cut Problem using Genetic Algorithm
    pair<vector<int>, int> result = geneticAlgorithmMaxCut(graph, 500, 1000);
    vector<int> bestPartition = result.first;
    int bestCutWeight = result.second;

    ofstream output;
    output.open("maxcut.out");

    // Output S' partition
    for (int i = 0; i < V; ++i) {
        if (bestPartition[i] == 0) { // Partition S'
            output << i + 1 << " "; // Adjust back to 1-based indexing
        }
    }
    output << endl;

    // Output best cut weight
    output << "Best Cut Weight: " << bestCutWeight << endl;

    output.close();

    return 0;
}
