#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include <sstream>
#include <set>
#include <climits>
#include <tuple>

using namespace std;

struct Edge
{
    int from, to, weight;
};

void bellman_ford(int src, int n, const vector<Edge> &edges, const set<int> &nodes, const string &trace_file, const string &result_file)
{
    vector<long long> dist(n, LLONG_MAX); // Use long long to prevent overflow
    vector<int> parent(n, -1);
    ofstream trace(trace_file);
    if (!trace)
    {
        cerr << "Error: Cannot open trace file\n"
             << flush;
        return;
    }
    ofstream result(result_file);
    if (!result)
    {
        cerr << "Error: Cannot open result file\n"
             << flush;
        trace.close();
        return;
    }

    dist[src] = 0;
    trace << "Initialize: dist[" << src << "] = 0\n";

    // Relax edges V-1 times
    int node_count = nodes.size();
    for (int i = 1; i <= node_count - 1; ++i)
    {
        bool updated = false;
        trace << "Iteration " << i << ":\n";
        for (const auto &edge : edges)
        {
            int u = edge.from;
            int v = edge.to;
            int w = edge.weight;
            if (u < 0 || u >= n || v < 0 || v >= n)
            {
                cerr << "Error: Invalid edge " << u << " -> " << v << "\n"
                     << flush;
                trace << "  Skip: Invalid edge " << u << " -> " << v << "\n";
                continue;
            }
            if (dist[u] != LLONG_MAX && dist[u] + w < dist[v])
            {
                dist[v] = dist[u] + w;
                parent[v] = u;
                updated = true;
                trace << "  Update: dist[" << v << "] = " << dist[v] << ", parent[" << v << "] = " << u << " (edge " << u << " -> " << v << ", weight=" << w << ")\n";
            }
            else
            {
                trace << "  No update: edge " << u << " -> " << v << ", dist[" << u << "]=" << (dist[u] == LLONG_MAX ? "INF" : to_string(dist[u])) << ", dist[" << v << "]=" << (dist[v] == LLONG_MAX ? "INF" : to_string(dist[v])) << "\n";
            }
        }
        if (!updated)
        {
            trace << "Early termination: No updates in iteration " << i << "\n";
            break;
        }
    }

    // Store and print results
    result << "Shortest paths from source " << src << ":\n";
    cout << "Shortest paths from source " << src << ":\n"
         << flush;
    for (int i = 0; i < n; ++i)
    {
        if (!nodes.count(i))
            continue; // Skip nodes not in graph
        if (dist[i] == LLONG_MAX)
        {
            result << "Node " << i << ": Unreachable\n";
            cout << "Node " << i << ": Unreachable\n"
                 << flush;
        }
        else
        {
            result << "Node " << i << ": Distance=" << dist[i] << ", Path=";
            cout << "Node " << i << ": Distance=" << dist[i] << ", Path=" << flush;
            vector<int> path;
            for (int v = i; v != -1; v = parent[v])
                path.push_back(v);
            for (int j = path.size() - 1; j >= 0; --j)
            {
                result << path[j];
                cout << path[j];
                if (j > 0)
                {
                    result << "->";
                    cout << "->";
                }
            }
            result << "\n";
            cout << "\n"
                 << flush;
        }
    }

    trace.close();
    result.close();
}

int main()
{
    // Check dataset file
    ifstream infile("../soc-sign-bitcoinalpha.csv");
    if (!infile)
    {
        cerr << "Error: Cannot open soc-sign-bitcoinalpha.csv\n"
             << flush;
        return 1;
    }

    // Get file size
    infile.seekg(0, ios::end);
    streampos file_size = infile.tellg();
    infile.seekg(0, ios::beg);
    cout << "Dataset file size: " << file_size << " bytes\n"
         << flush;

    string line;
    int max_node_id = 0;
    set<int> nodes;
    vector<Edge> edges; // Store edges as (from, to, weight)
    int edge_count = 0;

    // Read edges and find max node ID
    while (getline(infile, line))
    {
        stringstream ss(line);
        int u, v, w;
        string timestamp;
        char comma;
        if (!(ss >> u >> comma >> v >> comma >> w >> comma >> timestamp) || comma != ',')
        {
            cerr << "Warning: Failed to parse line: " << line << "\n"
                 << flush;
            continue;
        }
        if (w > 0)
        {
            max_node_id = max(max_node_id, max(u, v));
            nodes.insert(u);
            nodes.insert(v);
            edges.push_back({u, v, w});
            edge_count++;
        }
    }
    infile.close();

    int total_nodes = nodes.size();
    cout << "Number of nodes: " << total_nodes << "\n"
         << flush;
    cout << "Number of positive-weight edges: " << edge_count << "\n"
         << flush;
    cout << "Maximum node ID: " << max_node_id << "\n"
         << flush;

    if (total_nodes <= 1000)
    {
        cerr << "Error: Graph has " << total_nodes << " nodes, requires >1000\n"
             << flush;
        return 1;
    }

    int src;
    cout << "Enter source node (0 to " << max_node_id << "): " << flush;
    if (!(cin >> src))
    {
        cerr << "Error: Invalid input for source node\n"
             << flush;
        return 1;
    }
    if (src < 0 || src > max_node_id || nodes.count(src) == 0)
    {
        cerr << "Error: Invalid source node " << src << "\n"
             << flush;
        return 1;
    }

    // // Open file to store execution times for different node counts
    // ofstream times_file("BellmanFord_execution_times.txt");
    // if (!times_file)
    // {
    //     cerr << "Error: Cannot open BellmanFord_execution_times.txt\n"
    //          << flush;
    //     return 1;
    // }

    // // Measure execution time for different node counts (100, 200, 300, ...)
    // for (int target_nodes = 100; target_nodes <= total_nodes; target_nodes += 100)
    // {
    //     // Build subgraph with target_nodes nodes
    //     set<int> subgraph_nodes;
    //     int node_count = 0;
    //     for (int node : nodes)
    //     {
    //         subgraph_nodes.insert(node);
    //         node_count++;
    //         if (node_count >= target_nodes)
    //             break;
    //     }

    //     // Build edge list for the selected nodes, limited to target_nodes - 1
    //     vector<Edge> subgraph_edges;
    //     int subgraph_edge_count = 0;
    //     for (const auto &edge : edges)
    //     {
    //         int u = edge.from;
    //         int v = edge.to;
    //         int w = edge.weight;
    //         if (u < target_nodes && v < target_nodes && subgraph_nodes.count(u) && subgraph_nodes.count(v))
    //         {
    //             subgraph_edges.push_back({u, v, w});
    //             subgraph_edge_count++;
    //         }
    //     }

    //     cout << "Running Bellman-Ford for " << node_count << " nodes, " << subgraph_edge_count << " edges...\n"
    //          << flush;

    //     // Measure execution time
    //     auto start = chrono::high_resolution_clock::now();
    //     bellman_ford(src, target_nodes, subgraph_edges, subgraph_nodes, "BellmanFord_trace_temp.txt", "BellmanFord_results_temp.txt");
    //     auto end = chrono::high_resolution_clock::now();
    //     auto duration = chrono::duration_cast<chrono::microseconds>(end - start);

    //     // Save execution time
    //     times_file << node_count << "," << duration.count() << "\n";
    //     cout << "Nodes: " << node_count << ", Time: " << duration.count() << " microseconds\n"
    //          << flush;
    // }

    // times_file.close();

    // Run Bellman-Ford on the full graph for final results
    auto start = chrono::high_resolution_clock::now();
    bellman_ford(src, max_node_id + 1, edges, nodes, "BellmanFord_trace.txt", "BellmanFord_results.txt");
    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(end - start);

    // Store execution time for full graph
    ofstream time_file("BellmanFord_time.txt");
    if (!time_file)
    {
        cerr << "Error: Cannot open BellmanFord_time.txt\n"
             << flush;
    }
    else
    {
        time_file << "Execution time: " << duration.count() << " microseconds\n";
        time_file.close();
    }

    cout << "Execution time (full graph): " << duration.count() << " microseconds\n"
         << flush;
    cout << "Results written to BellmanFord_results.txt and BellmanFord_trace.txt\n"
         << flush;
    cout << "Execution times for different node counts written to BellmanFord_execution_times.txt\n"
         << flush;

    cout << "Press Enter to exit...\n"
         << flush;
    cin.get();
    cin.get();
    return 0;
}