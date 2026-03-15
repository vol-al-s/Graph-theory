#ifndef GRAPH_H
#define GRAPH_H

#include <vector>
#include <string>
#include <limits>

class Graph {
public:
    Graph(int vertices = 0);
    ~Graph();
    
    // Основные операции
    void addVertex();
    void addEdge(int u, int v, int weight = 1);
    void removeEdge(int u, int v);
    bool hasEdge(int u, int v) const;
    
    // Получение информации
    int getVertexCount() const;
    int getEdgeCount() const;
    std::vector<int> getAdjacencyList(int vertex) const;
    int getWeight(int u, int v) const;
    
    // Представления графа
    std::vector<std::vector<int>> getAdjacencyMatrix() const;
    std::vector<std::vector<int>> getWeightMatrix() const;
    
    // Проверки
    bool isConnected() const;
    bool isAcyclic() const;
    bool isTree() const;
    
    // Очистка
    void clear();
    void resetWeights();
    
private:
    int vertices;
    int edges;
    std::vector<std::vector<int>> adjacencyList;
    std::vector<std::vector<int>> weightMatrix;
    
    // Вспомогательные методы
    bool hasCycleDFS(int v, int parent, std::vector<bool>& visited) const;
    void dfs(int v, std::vector<bool>& visited) const;
};

#endif // GRAPH_H