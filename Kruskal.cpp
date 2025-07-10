#include <iostream>
#include <vector>
#include <unordered_map>
#include <set>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <chrono>
#include "graph.h"

using namespace std;


// ======== kruskal.h =========
class UnionFind {
public:
    unordered_map<int, int> parent, rank;

    int find(int x) {
        if (parent[x] != x)
            parent[x] = find(parent[x]);
        return parent[x];
    }

    void unionSets(int x, int y) {
        int rootX = find(x), rootY = find(y);
        if (rootX != rootY) {
            if (rank[rootX] > rank[rootY])
                parent[rootY] = rootX;
            else if (rank[rootX] < rank[rootY])
                parent[rootX] = rootY;
            else {
                parent[rootY] = rootX;
                rank[rootX]++;
            }
        }
    }

    void makeSet(int x) {
        parent[x] = x;
        rank[x] = 0;
    }
};

class KruskalMST {
public:
    vector<pair<double, pair<int, int>>> edges;
    vector<pair<int, int>> mst;
    double totalWeight = 0;
    string traceLog;

    void run(Graph& graph) {
        UnionFind uf;
        for (int node : graph.nodes)
            uf.makeSet(node);

        for (auto& [u, adjList] : graph.adj) {
            for (auto& [v, weight] : adjList) {
                if (u < v)
                    edges.push_back({weight, {u, v}});
            }
        }

        sort(edges.begin(), edges.end());

        for (auto& [weight, edge] : edges) {
            int u = edge.first, v = edge.second;
            traceLog += "Edges: (" + to_string(u) + ", " + to_string(v) + ", " + to_string(weight) + ")\n";
            if (uf.find(u) != uf.find(v)) {
                uf.unionSets(u, v);
                mst.push_back({u, v});
                totalWeight += weight;
                traceLog += "Added to MST: (" + to_string(u) + ", " + to_string(v) + ", " + to_string(weight) + ")\n";
            }
        }
    }
};

// ======== main logic =========
using namespace chrono;

void runKruskalOnSubgraphs(Graph& fullGraph, const string& outputFile) {
    vector<int> allNodes(fullGraph.nodes.begin(), fullGraph.nodes.end());
    ofstream out(outputFile);
    out << "nodes, execution time (milliseconds)\n";

    for (int size = 100; size <= 1000; size += 100) {
        Graph subGraph;
        set<int> subSet(allNodes.begin(), allNodes.begin() + size);

        set<pair<int, int>> seenEdges;
        for (int u : subSet) {
            for (auto& [v, w] : fullGraph.adj[u]) {
                if (subSet.count(v)) {
                    if (!seenEdges.count({v, u}) && !seenEdges.count({u, v})) {
                        subGraph.adj[u].push_back({v, w});
                        subGraph.adj[v].push_back({u, w});
                        subGraph.nodes.insert(u);
                        subGraph.nodes.insert(v);
                        subGraph.edgeCount++;
                        seenEdges.insert({u, v});
                    }
                }
            }
        }

        KruskalMST kruskal;
        auto start_time = high_resolution_clock::now();
        kruskal.run(subGraph);
        auto end_time = high_resolution_clock::now();

        double duration = duration_cast<milliseconds>(end_time - start_time).count();
        out << size << ", " << duration << "\n";
    }

    out.close();
}

int main() {
    Graph g;
    g.loadFromCSV("soc-sign-bitcoinalpha.csv");

    KruskalMST kruskal;
    auto start_time = high_resolution_clock::now();
    kruskal.run(g);
    auto end_time = high_resolution_clock::now();

    double duration = duration_cast<milliseconds>(end_time - start_time).count();

    sort(kruskal.mst.begin(), kruskal.mst.end());
    ofstream out("kruskal_mst.txt");
    out << "Total Weight: " << kruskal.totalWeight << "\n";
    out << "Kruskal's MST Edges:\n";
    for (auto& edge : kruskal.mst)
        out << "(Edges) " << edge.first << " - " << edge.second << ": (weight) " << edge.first << "\n";
    out.close();

    ofstream traceFile("kruskal_trace_time.txt");
    traceFile << "Kruskal Execution Time: " << duration << " ms\n\n";
    traceFile << "Trace Log:\n" << kruskal.traceLog;
    traceFile.close();

    runKruskalOnSubgraphs(g, "100_to_1000_time_kruskal.txt");

    cout << "Kruskal's MST completed. Files saved.\n";
    return 0;
}
