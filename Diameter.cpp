// #include <iostream>
// #include <fstream>
// #include <vector>
// #include <queue>
// #include <chrono>
// #include <sstream>
// #include <set>
// #include <climits>
// #include <tuple>

// using namespace std;

// struct Edge
// {
//     int to;
//     int weight;
// };

// // Single-source Dijkstra's to find shortest paths from src
// void dijkstra(int src, int n, vector<vector<Edge>> &graph, vector<long long> &dist, vector<int> &parent, ofstream &trace)
// {
//     dist.assign(n, LLONG_MAX);
//     parent.assign(n, -1);
//     priority_queue<pair<long long, int>, vector<pair<long long, int>>, greater<>> pq;

//     dist[src] = 0;
//     pq.push({0, src});
//     trace << "Priority Queue Push: (dist=0, node=" << src << ")\n";

//     while (!pq.empty())
//     {
//         long long d = pq.top().first;
//         int u = pq.top().second;
//         pq.pop();
//         trace << "Priority Queue Pop: (dist=" << d << ", node=" << u << ")\n";

//         if (d > dist[u])
//             continue;

//         if (u < 0 || u >= n)
//         {
//             cerr << "Error: Invalid node " << u << "\n"
//                  << flush;
//             continue;
//         }

//         for (const auto &edge : graph[u])
//         {
//             int v = edge.to;
//             int w = edge.weight;
//             if (v < 0 || v >= n)
//             {
//                 cerr << "Error: Invalid neighbor " << v << " from node " << u << "\n"
//                      << flush;
//                 continue;
//             }
//             if (dist[u] + w < dist[v])
//             {
//                 dist[v] = dist[u] + w;
//                 parent[v] = u;
//                 pq.push({dist[v], v});
//                 trace << "Update: dist[" << v << "]=" << dist[v] << ", parent[" << v << "]=" << u << "\n";
//                 trace << "Priority Queue Push: (dist=" << dist[v] << ", node=" << v << ")\n";
//             }
//         }
//     }
// }

// void compute_diameter(int n, vector<vector<Edge>> &graph, const set<int> &nodes, const string &trace_file, const string &result_file)
// {
//     ofstream trace(trace_file);
//     if (!trace)
//     {
//         cerr << "Error: Cannot open trace file\n"
//              << flush;
//         return;
//     }
//     ofstream result(result_file);
//     if (!result)
//     {
//         cerr << "Error: Cannot open result file\n"
//              << flush;
//         trace.close();
//         return;
//     }

//     long long max_dist = 0;
//     int start_node = -1, end_node = -1;
//     vector<int> max_path;

//     // Run Dijkstra's from each node in the graph
//     for (int src : nodes)
//     {
//         vector<long long> dist;
//         vector<int> parent;
//         trace << "Running Dijkstra from node " << src << "\n";
//         dijkstra(src, n, graph, dist, parent, trace);

//         // Find maximum distance
//         for (int v = 0; v < n; ++v)
//         {
//             if (dist[v] != LLONG_MAX && dist[v] > max_dist)
//             {
//                 max_dist = dist[v];
//                 start_node = src;
//                 end_node = v;

//                 // Reconstruct path
//                 max_path.clear();
//                 for (int u = v; u != -1; u = parent[u])
//                 {
//                     max_path.push_back(u);
//                 }
//             }
//         }
//     }

//     // Output results
//     if (max_dist == 0)
//     {
//         result << "No valid paths found in the graph\n";
//         cout << "No valid paths found in the graph\n"
//              << flush;
//     }
//     else
//     {
//         result << "Graph Diameter: " << max_dist << "\n";
//         result << "Path: ";
//         cout << "Graph Diameter: " << max_dist << "\n";
//         cout << "Path: " << flush;
//         for (int i = max_path.size() - 1; i >= 0; --i)
//         {
//             result << max_path[i];
//             cout << max_path[i];
//             if (i > 0)
//             {
//                 result << " -> ";
//                 cout << " -> ";
//             }
//         }
//         result << "\n";
//         result << "Start Node: " << start_node << "\n";
//         result << "End Node: " << end_node << "\n";
//         cout << "\n";
//         cout << "Start Node: " << start_node << "\n";
//         cout << "End Node: " << end_node << "\n"
//              << flush;
//     }

//     trace.close();
//     result.close();
// }

// int main()
// {
//     // Check dataset file
//     ifstream infile("../soc-sign-bitcoinalpha.csv");
//     if (!infile)
//     {
//         cerr << "Error: Cannot open soc-sign-bitcoin-alpha.csv\n"
//              << flush;
//         return 1;
//     }

//     // Get file size
//     infile.seekg(0, ios::end);
//     streampos file_size = infile.tellg();
//     infile.seekg(0, ios::beg);
//     cout << "Dataset file size: " << file_size << " bytes\n"
//          << flush;

//     string line;
//     int max_node_id = 0;
//     set<int> nodes;
//     vector<tuple<int, int, int>> edges; // Store (u, v, w)
//     int edge_count = 0;

//     // Read edges and find max node ID
//     while (getline(infile, line))
//     {
//         stringstream ss(line);
//         int u, v, w;
//         string timestamp;
//         char comma;
//         if (!(ss >> u >> comma >> v >> comma >> w >> comma >> timestamp) || comma != ',')
//         {
//             cerr << "Warning: Failed to parse line: " << line << "\n"
//                  << flush;
//             continue;
//         }
//         if (w > 0)
//         {
//             max_node_id = max(max_node_id, max(u, v));
//             nodes.insert(u);
//             nodes.insert(v);
//             edges.emplace_back(u, v, w);
//             edge_count++;
//         }
//     }
//     infile.close();

//     int n = nodes.size();
//     cout << "Number of nodes: " << n << "\n"
//          << flush;
//     cout << "Number of positive-weight edges: " << edge_count << "\n"
//          << flush;
//     cout << "Maximum node ID: " << max_node_id << "\n"
//          << flush;

//     if (n <= 1000)
//     {
//         cerr << "Error: Graph has " << n << " nodes, requires >1000\n"
//              << flush;
//         return 1;
//     }

//     // Initialize graph
//     vector<vector<Edge>> graph(max_node_id + 1);
//     for (const auto &[u, v, w] : edges)
//     {
//         if (u >= 0 && u < graph.size() && v >= 0 && v < graph.size())
//         {
//             graph[u].push_back({v, w});
//         }
//         else
//         {
//             cerr << "Error: Invalid edge " << u << " -> " << v << "\n"
//                  << flush;
//         }
//     }

//     // Measure execution time
//     auto start = chrono::high_resolution_clock::now();
//     compute_diameter(max_node_id + 1, graph, nodes, "Diameter_trace.txt", "Diameter_results.txt");
//     auto end = chrono::high_resolution_clock::now();
//     auto duration = chrono::duration_cast<chrono::microseconds>(end - start);

//     // Store execution time
//     ofstream time_file("Diameter_time.txt");
//     if (!time_file)
//     {
//         cerr << "Error: Cannot open Diameter_time.txt\n"
//              << flush;
//     }
//     else
//     {
//         time_file << "Execution time: " << duration.count() << " microseconds\n";
//         time_file.close();
//     }

//     cout << "Execution time: " << duration.count() << " microseconds\n"
//          << flush;
//     cout << "Results written to Diameter_results.txt and Diameter_trace.txt\n"
//          << flush;

//     cout << "Press Enter to exit...\n"
//          << flush;
//     cin.get();
//     return 0;
// }

#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <chrono>
#include <sstream>
#include <set>
#include <climits>
#include <tuple>

using namespace std;

struct Edge
{
    int to;
    int weight;
};

// Single-source Dijkstra's to find shortest paths from src
void dijkstra(int src, int n, vector<vector<Edge>> &graph, vector<long long> &dist, vector<int> &parent, ofstream &trace)
{
    dist.assign(n, LLONG_MAX);
    parent.assign(n, -1);
    priority_queue<pair<long long, int>, vector<pair<long long, int>>, greater<>> pq;

    dist[src] = 0;
    pq.push({0, src});
    trace << "Priority Queue Push: (dist=0, node=" << src << ")\n";

    while (!pq.empty())
    {
        long long d = pq.top().first;
        int u = pq.top().second;
        pq.pop();
        trace << "Priority Queue Pop: (dist=" << d << ", node=" << u << ")\n";

        if (d > dist[u])
            continue;

        if (u < 0 || u >= n)
        {
            cerr << "Error: Invalid node " << u << "\n"
                 << flush;
            continue;
        }

        for (const auto &edge : graph[u])
        {
            int v = edge.to;
            int w = edge.weight;
            if (v < 0 || v >= n)
            {
                cerr << "Error: Invalid neighbor " << v << " from node " << u << "\n"
                     << flush;
                continue;
            }
            if (dist[u] + w < dist[v])
            {
                dist[v] = dist[u] + w;
                parent[v] = u;
                pq.push({dist[v], v});
                trace << "Update: dist[" << v << "]=" << dist[v] << ", parent[" << v << "]=" << u << "\n";
                trace << "Priority Queue Push: (dist=" << dist[v] << ", node=" << v << ")\n";
            }
        }
    }
}

void compute_diameter(int n, vector<vector<Edge>> &graph, const set<int> &nodes, const string &trace_file, const string &result_file)
{
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

    long long max_dist = 0;
    int start_node = -1, end_node = -1;
    vector<int> max_path;

    // Run Dijkstra's from each node in the graph
    for (int src : nodes)
    {
        vector<long long> dist;
        vector<int> parent;
        trace << "Running Dijkstra from node " << src << "\n";
        dijkstra(src, n, graph, dist, parent, trace);

        // Find maximum distance
        for (int v = 0; v < n; ++v)
        {
            if (dist[v] != LLONG_MAX && dist[v] > max_dist)
            {
                max_dist = dist[v];
                start_node = src;
                end_node = v;

                // Reconstruct path
                max_path.clear();
                for (int u = v; u != -1; u = parent[u])
                {
                    max_path.push_back(u);
                }
            }
        }
    }

    // Output results
    if (max_dist == 0)
    {
        result << "No valid paths found in the graph\n";
        cout << "No valid paths found in the graph\n"
             << flush;
    }
    else
    {
        result << "Graph Diameter: " << max_dist << "\n";
        result << "Path: ";
        cout << "Graph Diameter: " << max_dist << "\n";
        cout << "Path: " << flush;
        for (int i = max_path.size() - 1; i >= 0; --i)
        {
            result << max_path[i];
            cout << max_path[i];
            if (i > 0)
            {
                result << " -> ";
                cout << " -> ";
            }
        }
        result << "\n";
        result << "Start Node: " << start_node << "\n";
        result << "End Node: " << end_node << "\n";
        cout << "\n";
        cout << "Start Node: " << start_node << "\n";
        cout << "End Node: " << end_node << "\n"
             << flush;
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
    vector<tuple<int, int, int>> edges; // Store (u, v, w)
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
            edges.emplace_back(u, v, w);
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

    // // Open file to store execution times for different node counts
    // ofstream times_file("Diameter_execution_times.txt");
    // if (!times_file)
    // {
    //     cerr << "Error: Cannot open Diameter_execution_times.txt\n"
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

    //     // Build graph for the selected nodes, limited to target_nodes - 1
    //     vector<vector<Edge>> graph(target_nodes); // Resize to target_nodes
    //     int subgraph_edge_count = 0;
    //     for (const auto &[u, v, w] : edges)
    //     {
    //         if (u < target_nodes && v < target_nodes && subgraph_nodes.count(u) && subgraph_nodes.count(v))
    //         {
    //             if (u >= 0 && u < graph.size() && v >= 0 && v < graph.size())
    //             {
    //                 graph[u].push_back({v, w});
    //                 subgraph_edge_count++;
    //             }
    //             else
    //             {
    //                 cerr << "Error: Invalid edge " << u << " -> " << v << "\n"
    //                      << flush;
    //             }
    //         }
    //     }

    //     cout << "Running Diameter for " << node_count << " nodes, " << subgraph_edge_count << " edges...\n"
    //          << flush;

    //     // Measure execution time
    //     auto start = chrono::high_resolution_clock::now();
    //     compute_diameter(target_nodes, graph, subgraph_nodes, "Diameter_trace_temp.txt", "Diameter_results_temp.txt");
    //     auto end = chrono::high_resolution_clock::now();
    //     auto duration = chrono::duration_cast<chrono::microseconds>(end - start);

    //     // Save execution time
    //     times_file << node_count << "," << duration.count() << "\n";
    //     cout << "Nodes: " << node_count << ", Time: " << duration.count() << " microseconds\n"
    //          << flush;
    // }

    // times_file.close();

    // Run Diameter on the full graph for final results
    vector<vector<Edge>> full_graph(max_node_id + 1);
    for (const auto &[u, v, w] : edges)
    {
        if (u >= 0 && u < full_graph.size() && v >= 0 && v < full_graph.size())
        {
            full_graph[u].push_back({v, w});
        }
        else
        {
            cerr << "Error: Invalid edge " << u << " -> " << v << "\n"
                 << flush;
        }
    }

    auto start = chrono::high_resolution_clock::now();
    compute_diameter(max_node_id + 1, full_graph, nodes, "Diameter_trace.txt", "Diameter_results.txt");
    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(end - start);

    // Store execution time for full graph
    ofstream time_file("Diameter_time.txt");
    if (!time_file)
    {
        cerr << "Error: Cannot open Diameter_time.txt\n"
             << flush;
    }
    else
    {
        time_file << "Execution time: " << duration.count() << " microseconds\n";
        time_file.close();
    }

    cout << "Execution time (full graph): " << duration.count() << " microseconds\n"
         << flush;
    cout << "Results written to Diameter_results.txt and Diameter_trace.txt\n"
         << flush;
    cout << "Execution times for different node counts written to Diameter_execution_times.txt\n"
         << flush;

    cout << "Press Enter to exit...\n"
         << flush;
    cin.get();
    return 0;
}