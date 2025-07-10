#include <iostream>
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <set>
#include <chrono>
#include <vector>

using namespace std;
using namespace chrono;

class DegreeAnalyzer {
public:
    unordered_map<int, int> degree;
    set<int> nodes;
    int edgeCount = 0;
    string trace;

    void readGraph(const string& filename, const set<int>& subNodes) {
        ifstream file(filename);
        string line;

        while (getline(file, line)) {
            stringstream ss(line);
            int u, v;
            double w;
            char comma;

            ss >> u >> comma >> v >> comma >> w;

            if (subNodes.count(u) && subNodes.count(v)) {
                degree[u]++;
                degree[v]++;
                nodes.insert(u);
                nodes.insert(v);
                edgeCount++;

                trace += "Edge: " + to_string(u) + " - " + to_string(v) + "\n";
                trace += "Increment degree[" + to_string(u) + "] → " + to_string(degree[u]) + "\n";
                trace += "Increment degree[" + to_string(v) + "] → " + to_string(degree[v]) + "\n";
            }
        }
    }

    double computeAverageDegree() const {
        if (nodes.empty()) return 0.0;
        return (2.0 * edgeCount) / nodes.size();
    }
};

int main() {
    // Step 1: Load full node set (in order of appearance)
    set<int> allNodes;
    vector<int> orderedNodes;
    ifstream infile("soc-sign-bitcoinalpha.csv");
    string line;
    while (getline(infile, line)) {
        stringstream ss(line);
        int u, v;
        double w;
        char comma;
        ss >> u >> comma >> v >> comma >> w;

        if (!allNodes.count(u)) {
            allNodes.insert(u);
            orderedNodes.push_back(u);
        }
        if (!allNodes.count(v)) {
            allNodes.insert(v);
            orderedNodes.push_back(v);
        }

        if (orderedNodes.size() >= 1000)
            break; // limit to 1000 nodes
    }

    // Step 2: Analyze each subset size
    ofstream timeOut("degree_subgraph_times.txt");
    timeOut << "nodes, execution time (ms)\n";

    for (int size = 100; size <= 1000; size += 100) {
        set<int> subSet(orderedNodes.begin(), orderedNodes.begin() + size);
        DegreeAnalyzer analyzer;

        auto start = high_resolution_clock::now();
        analyzer.readGraph("soc-sign-bitcoinalpha.csv", subSet);
        analyzer.computeAverageDegree(); // You can also save output if needed
        auto end = high_resolution_clock::now();

        auto duration = duration_cast<milliseconds>(end - start).count();
        timeOut << size << ", " << duration << "\n";

        cout << "Processed subgraph with " << size << " nodes in " << duration << " ms\n";
    }

    timeOut.close();
    cout << "Execution times written to degree_subgraph_times.txt\n";

    return 0;
}
