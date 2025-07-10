#include <iostream>
#include <vector>
#include <unordered_map>
#include <set>
#include <queue>
#include <fstream>
#include <chrono>
#include <algorithm>
#include "graph.h"

using namespace std;
using namespace chrono;


// ======== prim.h =========
class PrimMST {
public:
    vector<tuple<double, int, int>> mst;
    double totalWeight = 0;
    string traceLog;

    void run(Graph& graph, int start) {
        set<int> visited;
        priority_queue<tuple<double, int, int>,
                       vector<tuple<double, int, int>>,
                       greater<>> pq;

        visited.insert(start);
        for (auto& [v, w] : graph.adj[start])
            pq.push({w, start, v});

        while (!pq.empty()) {
            auto [w, u, v] = pq.top();
            pq.pop();

            if (visited.count(v)) continue;

            visited.insert(v);
            mst.push_back({w, u, v});
            totalWeight += w;

            traceLog += "Added edge: " + to_string(u) + " - " + to_string(v) + " (weight " + to_string(w) + ")\n";

            for (auto& [next, weight] : graph.adj[v]) {
                if (!visited.count(next))
                    pq.push({weight, v, next});
            }
        }
    }
};

// ======== main logic =========


void runPrimOnSubgraphs(Graph& fullGraph, const string& outputFile) {
    vector<int> allNodes(fullGraph.nodes.begin(), fullGraph.nodes.end());
    ofstream out(outputFile);
    out << "nodes, execution time (miliseconds)\n";

    for (int size = 100; size <= 1000; size += 100) {
        Graph subGraph;
        set<int> subSet(allNodes.begin(), allNodes.begin() + size);

        for (int u : subSet) {
            for (auto& [v, w] : fullGraph.adj[u]) {
                if (subSet.count(v)) {
                    subGraph.adj[u].push_back({v, w});
                    subGraph.adj[v].push_back({u, w});
                    subGraph.nodes.insert(u);
                    subGraph.nodes.insert(v);
                    subGraph.edgeCount++;
                }
            }
        }
        subGraph.edgeCount /= 2;

        PrimMST prim;
        auto start_time = high_resolution_clock::now();
        prim.run(subGraph, *subGraph.nodes.begin());
        auto end_time = high_resolution_clock::now();

        double duration = duration_cast<milliseconds>(end_time - start_time).count();
        out << size << ", " << duration << "\n";
    }

    out.close();
}

int main() {
    Graph g;
    g.loadFromCSV("soc-sign-bitcoinalpha.csv");

    PrimMST prim;
    auto start_time = high_resolution_clock::now();
    prim.run(g, *g.nodes.begin());
    auto end_time = high_resolution_clock::now();

    double duration = duration_cast<milliseconds>(end_time - start_time).count();

    ofstream outMST("prim_mst.txt");
    outMST << "Total Weight: " << prim.totalWeight << "\n";
    outMST << "MST Edges:\n";
    sort(prim.mst.begin(), prim.mst.end());
    for (auto [w, u, v] : prim.mst)
        outMST << "(Edges) " << u << " - " << v << " : (weight) " << w << "\n";
    outMST.close();

    ofstream outTrace("prim_trace_time.txt");
    outTrace << "Execution Time: " << duration << " ms\n";
    outTrace << "\nTrace Log:\n" << prim.traceLog;
    outTrace.close();

    runPrimOnSubgraphs(g, "100_to_1000_time_prim.txt");

    cout << "Prim's MST completed. Files saved.\n";
    return 0;
}
