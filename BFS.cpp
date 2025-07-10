#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
using namespace std;
using namespace std::chrono;

const int MAX_ID = 10000;  
const int MAX_VERTICES_TO_SHOW = 100;


class Node 
{
public:
    int target;
    int rating;
    int timestamp;
    Node* next;
    
    Node(int targetVal, int rate, int time) : target(targetVal), rating(rate), timestamp(time), next(nullptr) {}
};

class LinkedList {
private:
    Node* head;
public:
    LinkedList() : head(nullptr) {}
    
    void append(int targetVal, int rate, int time) {
        Node* newNode = new Node(targetVal, rate, time);
        if (head == nullptr) {
            head = newNode;
        } else {
            Node* current = head;
            while (current->next != nullptr) {
                current = current->next;
            }
            current->next = newNode;
        }
    }
        
    bool isEmpty() const {
        return head == nullptr;
    }
    
    Node* getHead() const {
        return head;
    }
};

class Queue {
private:
    Node* front;
    Node* rear;
public:
    Queue() : front(nullptr), rear(nullptr) {}
    
    void enqueue(int targetVal, int rate, int time) {
        Node* newNode = new Node(targetVal, rate, time);
        if (isEmpty()) {
            front = rear = newNode;
        } else {
            rear->next = newNode;
            rear = newNode;
        }
    }
    
    void dequeue() {
        if (isEmpty()) {
            cout << "Queue is empty" << endl;
            return;
        }
        Node* temp = front;
        front = front->next;
        if (front == nullptr) {
            rear = nullptr;
        }
        delete temp;
    }
    
    Node* getFront() const {
        return front;
    }
    
    bool isEmpty() const {
        return front == nullptr;
    }
};


class Graph {
private:
    LinkedList adj[MAX_ID];
    bool visited[MAX_ID];
    ofstream traceFile;
    
public:
    Graph() {
        for (int i = 0; i < MAX_ID; i++) {
            visited[i] = false;
        }
    }
    
    void openTraceFile() {
        traceFile.open("BFS_TraceFile.txt");
        if (!traceFile.is_open()) {
            cout << "Failed to open trace file!" << endl;
        }
    }
    
    void closeTraceFile() {
        if (traceFile.is_open()) {
            traceFile.close();
        }
    }
    
    ~Graph() {
        closeTraceFile();
    }
    
    void addEdge(int source, int target, int rating, int timestamp) 
    {
        adj[source].append(target, rating, timestamp);
    }
    
    // Load graph data with node limit for benchmarking
    bool loadGraphWithLimit(string filename, int nodeLimit) {
        ifstream file(filename);
        if (!file.is_open()) {
            cout << "Error opening the file: " << filename << endl;
            return false;
        }
        
        // Clear existing graph
        for (int i = 0; i < MAX_ID; i++) {
            adj[i] = LinkedList();
        }
        
        string line;
        int edgeCount = 0;
        
        while (getline(file, line)) {
            int comma1 = line.find(',');
            int comma2 = line.find(',', comma1 + 1);
            int comma3 = line.find(',', comma2 + 1);
            
            if (comma1 == string::npos || comma2 == string::npos || comma3 == string::npos) {
                continue;
            }
            
            string sourceStr = line.substr(0, comma1);
            string targetStr = line.substr(comma1 + 1, comma2 - comma1 - 1);
            string ratingStr = line.substr(comma2 + 1, comma3 - comma2 - 1);
            string timestampStr = line.substr(comma3 + 1);
            
            int source = stoi(sourceStr);
            int target = stoi(targetStr);
            int rating = stoi(ratingStr);
            int timestamp = stoi(timestampStr);
            
            // Only add edges if nodes are within the limit
            if (source <= nodeLimit && target <= nodeLimit) {
                addEdge(source, target, rating, timestamp);
                edgeCount++;
            }
        }
        
        file.close();
        return true;
    }
    
    // Function to generate a GraphViz DOT file for visualizing the graph
    void generateGraphVisualization(string outputFileName) {
        ofstream dotFile(outputFileName);
        
        if (!dotFile.is_open()) {
            cout << "Failed to open output file for graph visualization!" << endl;
            return;
        }
        
        // Start DOT file
        dotFile << "digraph G {" << endl;
        dotFile << "  rankdir=LR;" << endl;  // Left to right layout
        dotFile << "  node [shape=circle, style=filled, fillcolor=lightblue];" << endl;
        dotFile << "  edge [color=gray];" << endl;
        
        // Find the first MAX_VERTICES_TO_SHOW active vertices
        int selectedVertices[MAX_VERTICES_TO_SHOW];
        bool isSelected[MAX_ID] = {false};
        int selectedCount = 0;
        
        // First pass: select vertices
        for (int i = 0; i < MAX_ID && selectedCount < MAX_VERTICES_TO_SHOW; i++) {
            if (!adj[i].isEmpty()) {
                selectedVertices[selectedCount] = i;
                isSelected[i] = true;
                selectedCount++;
            }
        }

        
        // Second pass: only add edges between selected vertices
        for (int i = 0; i < selectedCount; i++) {
            int vertex = selectedVertices[i];
            
            // Add the vertex
            dotFile << "  \"" << vertex << "\" [tooltip=\"Vertex " << vertex << "\"];" << endl;
            
            // Add edges only to other selected vertices
            Node* current = adj[vertex].getHead();
            while (current != nullptr) {
                // Only include edges to other vertices in our selected set
                if (current->target < MAX_ID && isSelected[current->target]) {
                    dotFile << "  \"" << vertex << "\" -> \"" << current->target << "\" [label=\"" 
                            << current->rating << "\"];" << endl;
                }
                current = current->next;
            }
        }
        
        // End DOT file
        dotFile << "}" << endl;
        dotFile.close();
        
        cout << "Graph visualization file created: " << outputFileName << endl;
        cout << "To view: dot -Tpng " << outputFileName << " -o graph.png" << endl;
    }
    
    // Original BFS with tracing
    void BFS(int startVertex) {
        // Mark all vertices as not visited initially
        for (int i = 0; i < MAX_ID; i++) {
            visited[i] = false;
        }
        
        // Write header to trace file
        traceFile << "BFS Traversal Trace Starting from Vertex " << startVertex << endl;
        traceFile << "==================================================" << endl;
        cout << "BFS Traversal Trace Starting from Vertex " << startVertex << endl;
        cout << "==================================================" << endl;
        
        // Create a queue for BFS
        Queue queue;
        
        // Mark the current node as visited and enqueue it
        visited[startVertex] = true;
        
        // Only enqueue if there are adjacent vertices
        if (!adj[startVertex].isEmpty() && adj[startVertex].getHead() != nullptr) {
            queue.enqueue(startVertex, adj[startVertex].getHead()->rating, adj[startVertex].getHead()->timestamp);
        } else {
            queue.enqueue(startVertex, 0, 0); // Default values if no adjacent vertices
        }
        
        traceFile << "MARK: Vertex " << startVertex << " marked as visited" << endl;
        traceFile << "ENQUEUE: Vertex " << startVertex << " added to queue" << endl;
        cout << "MARK: Vertex " << startVertex << " marked as visited" << endl;
        cout << "ENQUEUE: Vertex " << startVertex << " added to queue" << endl;
        
        // BFS loop
        while (!queue.isEmpty()) {
            // Dequeue a vertex from queue
            Node* current = queue.getFront();
            int vertex = current->target;
            queue.dequeue();
            
            traceFile << "DEQUEUE: Processing vertex " << vertex << endl;
            cout << "DEQUEUE: Processing vertex " << vertex << endl;
            
            // Get all adjacent vertices of the dequeued vertex
            Node* adjNode = adj[vertex].getHead();
            
            // Process all adjacent vertices
            while (adjNode != nullptr) {
                int adjVertex = adjNode->target;
                int rating = adjNode->rating;
                int timestamp = adjNode->timestamp;
                
                traceFile << "CHECK: Adjacent vertex " << adjVertex << " (Rating: " << rating 
                         << ", Timestamp: " << timestamp << ")";
                cout << "CHECK: Adjacent vertex " << adjVertex << " (Rating: " << rating 
                     << ", Timestamp: " << timestamp << ")";
                
                // If adjacent vertex is not visited, mark it visited and enqueue it
                if (!visited[adjVertex]) {
                    visited[adjVertex] = true;
                    queue.enqueue(adjVertex, rating, timestamp);
                    
                    traceFile << " - Not visited, marking as visited and enqueueing" << endl;
                    cout << " - Not visited, marking as visited and enqueueing" << endl;
                    
                    traceFile << "MARK: Vertex " << adjVertex << " marked as visited" << endl;
                    traceFile << "ENQUEUE: Vertex " << adjVertex << " added to queue" << endl;
                    cout << "MARK: Vertex " << adjVertex << " marked as visited" << endl;
                    cout << "ENQUEUE: Vertex " << adjVertex << " added to queue" << endl;
                } else {
                    traceFile << " - Already visited, skipping" << endl;
                    cout << " - Already visited, skipping" << endl;
                }
                
                adjNode = adjNode->next;
            }
            
            traceFile << "COMPLETE: Finished processing all adjacents of vertex " << vertex << endl;
            cout << "COMPLETE: Finished processing all adjacents of vertex " << vertex << endl;
            traceFile << "--------------------------------------------------" << endl;
            cout << "--------------------------------------------------" << endl;
        }
    }
    
    // BFS without tracing (for benchmarking)
    void BFSBenchmark(int startVertex) {
        // Mark all vertices as not visited initially
        for (int i = 0; i < MAX_ID; i++) {
            visited[i] = false;
        }
        
        // Create a queue for BFS
        Queue queue;
        
        // Mark the current node as visited and enqueue it
        visited[startVertex] = true;
        
        // Only enqueue if there are adjacent vertices
        if (!adj[startVertex].isEmpty() && adj[startVertex].getHead() != nullptr) {
            queue.enqueue(startVertex, adj[startVertex].getHead()->rating, adj[startVertex].getHead()->timestamp);
        } else {
            queue.enqueue(startVertex, 0, 0); // Default values if no adjacent vertices
        }
        
        // BFS loop without trace output
        while (!queue.isEmpty()) {
            // Dequeue a vertex from queue
            Node* current = queue.getFront();
            int vertex = current->target;
            queue.dequeue();
            
            // Process all adjacent vertices
            Node* adjNode = adj[vertex].getHead();
            while (adjNode != nullptr) {
                int adjVertex = adjNode->target;
                int rating = adjNode->rating;
                int timestamp = adjNode->timestamp;
                
                if (!visited[adjVertex]) {
                    visited[adjVertex] = true;
                    queue.enqueue(adjVertex, rating, timestamp);
                }
                
                adjNode = adjNode->next;
            }
        }
    }
    
    // Benchmark BFS with different node counts
    void runBenchmark() {
        // Hardcoded node counts
        const int NUM_DATA_POINTS = 5;
        int nodeCounts[NUM_DATA_POINTS] = {500, 1000, 2000, 4000, 8000};
        double executionTimes[NUM_DATA_POINTS];
        
        cout << "BFS Algorithm Benchmark" << endl;
        cout << "======================" << endl;
        cout << "Nodes\tTime (ns)" << endl;
        
        // Run benchmark for each node count
        for (int i = 0; i < NUM_DATA_POINTS; i++) {
            int nodeLimit = nodeCounts[i];
            
            // Load data with node limit
            loadGraphWithLimit("data.csv", nodeLimit);
            
            // Find a valid start vertex - use node 1
            int startVertex = 1;
            
            // Measure execution time directly in nanoseconds
            auto start = high_resolution_clock::now();
            
            // Run BFS once without trace output
            BFSBenchmark(startVertex);
            
            auto stop = high_resolution_clock::now();
            auto duration = duration_cast<nanoseconds>(stop - start);
            
            // Store execution time in nanoseconds
            executionTimes[i] = duration.count();
            
            cout << nodeLimit << "\t" << executionTimes[i] << endl;
        }
        
        // Write results to file
        ofstream outFile("bfs_benchmark_results.csv");
        if (outFile.is_open()) {
            outFile << "NodeCount,TimeNanosec" << endl;
            for (int i = 0; i < NUM_DATA_POINTS; i++) {
                outFile << nodeCounts[i] << "," << executionTimes[i] << endl;
            }
            outFile.close();
            cout << "\nResults written to bfs_benchmark_results.csv" << endl;
        } else {
            cout << "Failed to open output file for benchmark results" << endl;
        }
    }
};

int main() {
    Graph graph;
    int choice;
    
    cout << "BFS Algorithm Menu" << endl;
    cout << "==================" << endl;
    cout << "1. Run Algorithm with Trace" << endl;
    cout << "2. Run Benchmark" << endl;
    cout << "3. Generate Graph Visualization" << endl;
    cout << "Enter your choice (1-3): ";
    cin >> choice;
    
    switch (choice) {
        case 1: {
            // Original BFS algorithm with trace
            ifstream file("data.csv");  
            if (!file.is_open()) {
                cout << "Error opening the file" << endl;
                return 1;
            }
            
            string line;
            
            while (getline(file, line)) {
                int comma1 = line.find(',');
                int comma2 = line.find(',', comma1 + 1);
                int comma3 = line.find(',', comma2 + 1);
                
                string sourceStr = line.substr(0, comma1);
                string targetStr = line.substr(comma1 + 1, comma2 - comma1 - 1);
                string ratingStr = line.substr(comma2 + 1, comma3 - comma2 - 1);
                string timestampStr = line.substr(comma3 + 1);
                
                int source = stoi(sourceStr);
                int target = stoi(targetStr);
                int rating = stoi(ratingStr);
                int timestamp = stoi(timestampStr);
                
                graph.addEdge(source, target, rating, timestamp);
            }
            
            file.close();
            
            cout << "Please enter the starting vertex ( 1 - " << MAX_ID - 1 << " ): ";
            int startVertex;
            cin >> startVertex;
            if (startVertex < 1 || startVertex >= MAX_ID) {
                cout << "Invalid starting vertex" << endl;
                return 1;
            }
            
            graph.openTraceFile();
            graph.BFS(startVertex);
            graph.closeTraceFile();
            
            cout << "BFS trace has been saved to BFS_TraceFile.txt" << endl;
            break;
        }
        
        case 2: {
            // Run benchmark
            graph.runBenchmark();
            break;
        }
        
        case 3: {
            // Generate visualization
            ifstream file("data.csv");  
            if (!file.is_open()) {
                cout << "Error opening the file" << endl;
                return 1;
            }
            
            string line;
            
            while (getline(file, line)) {
                int comma1 = line.find(',');
                int comma2 = line.find(',', comma1 + 1);
                int comma3 = line.find(',', comma2 + 1);
                
                string sourceStr = line.substr(0, comma1);
                string targetStr = line.substr(comma1 + 1, comma2 - comma1 - 1);
                string ratingStr = line.substr(comma2 + 1, comma3 - comma2 - 1);
                string timestampStr = line.substr(comma3 + 1);
                
                int source = stoi(sourceStr);
                int target = stoi(targetStr);
                int rating = stoi(ratingStr);
                int timestamp = stoi(timestampStr);
                
                graph.addEdge(source, target, rating, timestamp);
            }
            
            file.close();
            
            graph.generateGraphVisualization("bfs_graph.dot");
            break;
        }
        
        default:
            cout << "Invalid choice!" << endl;
            break;
    }
    
    return 0;
}