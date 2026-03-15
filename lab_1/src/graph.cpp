#include "graph.h"
#include <algorithm>
#include <queue>

const int INF = std::numeric_limits<int>::max();

Graph::Graph(int vertices) : vertices(vertices), edges(0) {
    adjacencyList.resize(vertices);
    weightMatrix.resize(vertices, std::vector<int>(vertices, 0));
}

Graph::~Graph() {
    clear();
}

void Graph::addVertex() {
    adjacencyList.push_back(std::vector<int>());
    weightMatrix.push_back(std::vector<int>(vertices + 1, 0));
    for (auto& row : weightMatrix) {
        row.push_back(0);
    }
    vertices++;
}

void Graph::addEdge(int u, int v, int weight) {
    if (u < 0 || u >= vertices || v < 0 || v >= vertices) return;
    
    if (!hasEdge(u, v)) {
        adjacencyList[u].push_back(v);
        adjacencyList[v].push_back(u);
        edges++;
    }
    
    weightMatrix[u][v] = weight;
    weightMatrix[v][u] = weight;
}

void Graph::removeEdge(int u, int v) {
    if (u < 0 || u >= vertices || v < 0 || v >= vertices) return;
    
    if (hasEdge(u, v)) {
        adjacencyList[u].erase(
            std::remove(adjacencyList[u].begin(), adjacencyList[u].end(), v),
            adjacencyList[u].end()
        );
        adjacencyList[v].erase(
            std::remove(adjacencyList[v].begin(), adjacencyList[v].end(), u),
            adjacencyList[v].end()
        );
        edges--;
    }
    
    weightMatrix[u][v] = 0;
    weightMatrix[v][u] = 0;
}

bool Graph::hasEdge(int u, int v) const {
    if (u < 0 || u >= vertices || v < 0 || v >= vertices) return false;
    return std::find(adjacencyList[u].begin(), adjacencyList[u].end(), v) 
           != adjacencyList[u].end();
}

int Graph::getVertexCount() const {
    return vertices;
}

int Graph::getEdgeCount() const {
    return edges;
}

std::vector<int> Graph::getAdjacencyList(int vertex) const {
    if (vertex < 0 || vertex >= vertices) return {};
    return adjacencyList[vertex];
}

int Graph::getWeight(int u, int v) const {
    if (u < 0 || u >= vertices || v < 0 || v >= vertices) return 0;
    return weightMatrix[u][v];
}

std::vector<std::vector<int>> Graph::getAdjacencyMatrix() const {
    std::vector<std::vector<int>> matrix(vertices, std::vector<int>(vertices, 0));
    for (int i = 0; i < vertices; i++) {
        for (int neighbor : adjacencyList[i]) {
            matrix[i][neighbor] = 1;
        }
    }
    return matrix;
}

std::vector<std::vector<int>> Graph::getWeightMatrix() const {
    return weightMatrix;
}

bool Graph::isConnected() const {
    if (vertices == 0) return true;
    
    std::vector<bool> visited(vertices, false);
    dfs(0, visited);
    
    for (bool v : visited) {
        if (!v) return false;
    }
    return true;
}

bool Graph::isAcyclic() const {
    std::vector<bool> visited(vertices, false);
    
    for (int i = 0; i < vertices; i++) {
        if (!visited[i]) {
            if (hasCycleDFS(i, -1, visited)) {
                return false;
            }
        }
    }
    return true;
}

bool Graph::isTree() const {
    return isConnected() && isAcyclic();
}

void Graph::clear() {
    vertices = 0;
    edges = 0;
    adjacencyList.clear();
    weightMatrix.clear();
}

void Graph::resetWeights() {
    for (int i = 0; i < vertices; i++) {
        for (int j = 0; j < vertices; j++) {
            if (weightMatrix[i][j] != 0) {
                weightMatrix[i][j] = 1;
            }
        }
    }
}

bool Graph::hasCycleDFS(int v, int parent, std::vector<bool>& visited) const {
    visited[v] = true;
    
    for (int neighbor : adjacencyList[v]) {
        if (!visited[neighbor]) {
            if (hasCycleDFS(neighbor, v, visited)) {
                return true;
            }
        } else if (neighbor != parent) {
            return true;
        }
    }
    return false;
}

void Graph::dfs(int v, std::vector<bool>& visited) const {
    visited[v] = true;
    for (int neighbor : adjacencyList[v]) {
        if (!visited[neighbor]) {
            dfs(neighbor, visited);
        }
    }
}