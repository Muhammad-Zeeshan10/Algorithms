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

class Stack {
private:
    int arr[MAX_ID];
    int top;
    int capacity;
    
public:
    Stack() : top(-1), capacity(MAX_ID) {}
    
    bool isEmpty() {
        return top == -1;
    }
    
    bool isFull() {
        return top == capacity - 1;
    }
    
    void push(int item) {
        if (isFull()) {
            return;
        }
        arr[++top] = item;
    }
    
    int pop() {
        if (isEmpty()) {
            return -1;
        }
        return arr[top--];
    }
    
    int peek() {
        if (isEmpty()) {
            return -1;
        }
        return arr[top];
    }
    
    bool contains(int item) {
        for (int i = 0; i <= top; i++) {
            if (arr[i] == item) {
                return true;
            }
        }
        return false;
    }
    
    int size() {
        return top + 1;
    }
    
    void getPath(int path[], int& pathSize) {
        pathSize = top + 1;
        for (int i = 0; i <= top; i++) {
            path[i] = arr[i];
        }
    }
};

class Graph {
private:
    LinkedList adj[MAX_ID];
    bool visited[MAX_ID];
    bool inRecursionStack[MAX_ID];
    ofstream cycleFile;
    int cycleCount;
    
public:
    Graph() {
        for (int i = 0; i < MAX_ID; i++) {
            visited[i] = false;
            inRecursionStack[i] = false;
        }
        cycleCount = 0;
    }
    
    void openCycleFile() {
        cycleFile.open("Cycles.txt");
        if (!cycleFile.is_open()) {
            cout << "Failed to open cycle file!" << endl;
        }
        cycleFile << "Cycles Detected in the Graph" << endl;
        cycleFile << "==========================" << endl;
    }
    
    void closeCycleFile() {
        if (cycleFile.is_open()) {
            cycleFile.close();
        }
    }
    
    ~Graph() {
        closeCycleFile();
    }
    
    void addEdge(int source, int target, int rating, int timestamp) {
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
        
        // If we couldn't find enough active vertices
        if (selectedCount == 0) {
            dotFile << "  \"No active vertices found\" [shape=box, color=red];" << endl;
            dotFile << "}" << endl;
            dotFile.close();
            cout << "No active vertices found for visualization." << endl;
            return;
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
        cout << "To view: dot -Tpng " << outputFileName << " -o cycle_graph.png" << endl;
    }
    
    // Main cycle detection with logging
    int detectCycles(int startVertex) {
        // Reset for new detection
        for (int i = 0; i < MAX_ID; i++) {
            visited[i] = false;
            inRecursionStack[i] = false;
        }
        
        cycleCount = 0;
        cout << "Starting cycle detection from vertex " << startVertex << endl;
        cout << "==================================================" << endl;
        
        Stack path;
        detectCyclesRecursive(startVertex, path);
        
        cout << "Total cycles detected: " << cycleCount << endl;
        cycleFile << "\nTotal cycles detected: " << cycleCount << endl;
        
        return cycleCount;
    }
    
    // Cycle detection without logs (for benchmarking)
    int detectCyclesBenchmark(int startVertex) {
        // Reset for new detection
        for (int i = 0; i < MAX_ID; i++) {
            visited[i] = false;
            inRecursionStack[i] = false;
        }
        
        cycleCount = 0;
        Stack path;
        detectCyclesRecursiveBenchmark(startVertex, path);
        
        return cycleCount;
    }
    
    void detectCyclesRecursive(int vertex, Stack& path) {
        // If vertex is already in recursion stack, we found a cycle
        if (path.contains(vertex)) {
            processCycle(vertex, path);
            return;
        }
        
        // If vertex is already visited but not in recursion stack, no cycle through this vertex
        if (visited[vertex]) {
            return;
        }
        
        // Mark current vertex as visited and add to recursion stack
        visited[vertex] = true;
        path.push(vertex);
        
        // Process all adjacent vertices
        Node* current = adj[vertex].getHead();
        while (current != nullptr) {
            detectCyclesRecursive(current->target, path);
            current = current->next;
        }
        
        // Remove vertex from recursion stack
        path.pop();
    }
    
    // Benchmark version without logging
    void detectCyclesRecursiveBenchmark(int vertex, Stack& path) {
        // If vertex is already in recursion stack, we found a cycle
        if (path.contains(vertex)) {
            processCycleBenchmark(vertex, path);
            return;
        }
        
        // If vertex is already visited but not in recursion stack, no cycle through this vertex
        if (visited[vertex]) {
            return;
        }
        
        // Mark current vertex as visited and add to recursion stack
        visited[vertex] = true;
        path.push(vertex);
        
        // Process all adjacent vertices
        Node* current = adj[vertex].getHead();
        while (current != nullptr) {
            detectCyclesRecursiveBenchmark(current->target, path);
            current = current->next;
        }
        
        // Remove vertex from recursion stack
        path.pop();
    }
    
    void processCycle(int cycleStart, Stack& path) {
        int cyclePath[MAX_ID];
        int cycleSize = 0;
        bool inCycle = false;
        int cycleVertices[MAX_ID];
        int cycleVertexCount = 0;
        
        // Get the current path
        path.getPath(cyclePath, cycleSize);
        
        // Find where the cycle starts in the path
        for (int i = 0; i < cycleSize; i++) {
            if (cyclePath[i] == cycleStart) {
                inCycle = true;
            }
            
            if (inCycle) {
                cycleVertices[cycleVertexCount++] = cyclePath[i];
            }
        }
        
        // Only count and record if this is a valid cycle (at least 3 vertices)
        if (cycleVertexCount > 2) {
            cycleCount++;
            
            cycleFile << "Cycle #" << cycleCount << ": ";
            cout << "Cycle #" << cycleCount << ": ";
            
            for (int i = 0; i < cycleVertexCount; i++) {
                cycleFile << cycleVertices[i];
                cout << cycleVertices[i];
                if (i < cycleVertexCount - 1) {
                    cycleFile << " -> ";
                    cout << " -> ";
                }
            }
            cycleFile << " -> " << cycleStart << endl;
            cout << " -> " << cycleStart << endl;
        }
    }
    
    // Benchmark version without output
    void processCycleBenchmark(int cycleStart, Stack& path) {
        int cyclePath[MAX_ID];
        int cycleSize = 0;
        bool inCycle = false;
        int cycleVertices[MAX_ID];
        int cycleVertexCount = 0;
        
        // Get the current path
        path.getPath(cyclePath, cycleSize);
        
        // Find where the cycle starts in the path
        for (int i = 0; i < cycleSize; i++) {
            if (cyclePath[i] == cycleStart) {
                inCycle = true;
            }
            
            if (inCycle) {
                cycleVertices[cycleVertexCount++] = cyclePath[i];
            }
        }
        
        // Only count if this is a valid cycle (at least 3 vertices)
        if (cycleVertexCount > 2) {
            cycleCount++;
        }
    }
    
    int getTotalCycles() {
        return cycleCount;
    }
    
    // Benchmark cycle detection with different node counts
    void runBenchmark() {
        // Hardcoded node counts
        const int NUM_DATA_POINTS = 5;
        int nodeCounts[NUM_DATA_POINTS] = {500, 1000, 2000, 4000, 8000};
        double executionTimes[NUM_DATA_POINTS];
        
        cout << "Cycle Detection Algorithm Benchmark" << endl;
        cout << "=================================" << endl;
        cout << "Nodes\tTime (μs)" << endl;
        
        // Run benchmark for each node count
        for (int i = 0; i < NUM_DATA_POINTS; i++) {
            int nodeLimit = nodeCounts[i];
            
            // Load data with node limit
            loadGraphWithLimit("data.csv", nodeLimit);
            
            // Find a valid start vertex - use node 1
            int startVertex = 1;
            
            // Measure execution time
            auto start = high_resolution_clock::now();
            
            // Run cycle detection once without trace output
            detectCyclesBenchmark(startVertex);
            
            auto stop = high_resolution_clock::now();
            auto duration = duration_cast<microseconds>(stop - start);
            
            // Store execution time in microseconds
            executionTimes[i] = duration.count();
            
            cout << nodeLimit << "\t" << executionTimes[i] << endl;
        }
        
        // Write results to file
        ofstream outFile("cycle_benchmark_results.csv");
        if (outFile.is_open()) {
            outFile << "NodeCount,TimeMicrosec" << endl;
            for (int i = 0; i < NUM_DATA_POINTS; i++) {
                outFile << nodeCounts[i] << "," << executionTimes[i] << endl;
            }
            outFile.close();
            cout << "\nResults written to cycle_benchmark_results.csv" << endl;
        } else {
            cout << "Failed to open output file for benchmark results" << endl;
        }
    }
};

int main() {
    Graph graph;
    int choice;
    
    cout << "Cycle Detection Algorithm Menu" << endl;
    cout << "===========================" << endl;
    cout << "1. Run Algorithm with Trace" << endl;
    cout << "2. Run Benchmark" << endl;
    cout << "3. Generate Graph Visualization" << endl;
    cout << "Enter your choice (1-3): ";
    cin >> choice;
    
    switch (choice) {
        case 1: {
            // Original cycle detection algorithm with trace
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
            
            graph.openCycleFile();
            int totalCycles = graph.detectCycles(startVertex);
            graph.closeCycleFile();
            
            cout << "Detected " << totalCycles << " cycles. Details saved to Cycles.txt" << endl;
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
            
            graph.generateGraphVisualization("cycle_graph.dot");
            break;
        }
        
        default:
            cout << "Invalid choice!" << endl;
            break;
    }
    
    return 0;
}