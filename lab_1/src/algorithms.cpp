#include "algorithms.h"
#include <queue>
#include <algorithm>
#include <limits>

const int INF = std::numeric_limits<int>::max();

std::vector<int> GraphAlgorithms::calculateEccentricities(const Graph& graph) {
    int n = graph.getVertexCount();
    std::vector<int> eccentricities(n, 0);
    
    // Для каждой вершины находим максимальное расстояние до всех остальных
    for (int i = 0; i < n; i++) {
        std::vector<int> distances = bfs(graph, i);
        int maxDist = 0;
        
        for (int j = 0; j < n; j++) {
            if (distances[j] != INF && distances[j] > maxDist) {
                maxDist = distances[j];
            }
        }
        
        eccentricities[i] = maxDist;
    }
    
    return eccentricities;
}

std::vector<int> GraphAlgorithms::findGraphCenter(const Graph& graph) {
    std::vector<int> eccentricities = calculateEccentricities(graph);
    int minEccentricity = INF;
    
    // Находим минимальный эксцентриситет (радиус графа)
    for (int ecc : eccentricities) {
        if (ecc < minEccentricity) {
            minEccentricity = ecc;
        }
    }
    
    // Все вершины с минимальным эксцентриситетом образуют центр
    std::vector<int> center;
    for (int i = 0; i < eccentricities.size(); i++) {
        if (eccentricities[i] == minEccentricity) {
            center.push_back(i);
        }
    }
    
    return center;
}

std::vector<std::pair<int, int>> GraphAlgorithms::findDiametralVertices(const Graph& graph) {
    int n = graph.getVertexCount();
    std::vector<std::pair<int, int>> diametral;
    int diameter = 0;
    
    // Находим диаметр и все пары вершин с таким расстоянием
    for (int i = 0; i < n; i++) {
        std::vector<int> distances = bfs(graph, i);
        
        for (int j = i + 1; j < n; j++) {
            if (distances[j] != INF) {
                if (distances[j] > diameter) {
                    diameter = distances[j];
                    diametral.clear();
                    diametral.push_back({i, j});
                } else if (distances[j] == diameter) {
                    diametral.push_back({i, j});
                }
            }
        }
    }
    
    return diametral;
}

int GraphAlgorithms::calculateDiameter(const Graph& graph) {
    auto diametral = findDiametralVertices(graph);
    if (diametral.empty()) return 0;
    
    int n = graph.getVertexCount();
    int diameter = 0;
    
    for (int i = 0; i < n; i++) {
        std::vector<int> distances = bfs(graph, i);
        for (int j = 0; j < n; j++) {
            if (distances[j] != INF && distances[j] > diameter) {
                diameter = distances[j];
            }
        }
    }
    
    return diameter;
}

std::vector<std::vector<int>> GraphAlgorithms::shimbelAlgorithm(
    const Graph& graph,
    bool allowNegative
) {
    int n = graph.getVertexCount();
    std::vector<std::vector<int>> dist(n, std::vector<int>(n, INF));
    
    // Инициализация матрицы расстояний
    auto weightMatrix = graph.getWeightMatrix();
    for (int i = 0; i < n; i++) {
        dist[i][i] = 0;
        for (int j = 0; j < n; j++) {
            if (weightMatrix[i][j] != 0) {
                dist[i][j] = weightMatrix[i][j];
            }
        }
    }
    
    // Алгоритм Флойда-Уоршелла (аналог метода Шимбеля)
    // Новиков, раздел 8.6.2
    for (int k = 0; k < n; k++) {
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                if (dist[i][k] != INF && dist[k][j] != INF) {
                    if (dist[i][k] + dist[k][j] < dist[i][j]) {
                        dist[i][j] = dist[i][k] + dist[k][j];
                    }
                }
            }
        }
    }
    
    // Проверка на отрицательные циклы
    if (!allowNegative) {
        for (int i = 0; i < n; i++) {
            if (dist[i][i] < 0) {
                // Обнаружен отрицательный цикл
                // Возвращаем матрицу с пометкой об ошибке
                for (int j = 0; j < n; j++) {
                    for (int k = 0; k < n; k++) {
                        dist[j][k] = INF;
                    }
                }
                break;
            }
        }
    }
    
    return dist;
}

bool GraphAlgorithms::hasRoute(const Graph& graph, int start, int end) {
    if (start < 0 || start >= graph.getVertexCount()) return false;
    if (end < 0 || end >= graph.getVertexCount()) return false;
    
    std::vector<int> distances = bfs(graph, start);
    return distances[end] != INF;
}

int GraphAlgorithms::countRoutes(const Graph& graph, int start, int end) {
    if (start < 0 || start >= graph.getVertexCount()) return 0;
    if (end < 0 || end >= graph.getVertexCount()) return 0;
    if (start == end) return 1;
    
    int count = 0;
    std::vector<bool> visited(graph.getVertexCount(), false);
    countRoutesDFS(graph, start, end, visited, count);
    
    return count;
}

std::vector<std::vector<int>> GraphAlgorithms::findAllRoutes(
    const Graph& graph,
    int start,
    int end
) {
    std::vector<std::vector<int>> allPaths;
    
    if (start < 0 || start >= graph.getVertexCount()) return allPaths;
    if (end < 0 || end >= graph.getVertexCount()) return allPaths;
    
    std::vector<bool> visited(graph.getVertexCount(), false);
    std::vector<int> path;
    path.push_back(start);
    visited[start] = true;
    
    findAllRoutesDFS(graph, start, end, visited, path, allPaths);
    
    return allPaths;
}

std::vector<int> GraphAlgorithms::bfs(const Graph& graph, int start) {
    int n = graph.getVertexCount();
    std::vector<int> distances(n, INF);
    
    if (start < 0 || start >= n) return distances;
    
    std::queue<int> queue;
    queue.push(start);
    distances[start] = 0;
    
    while (!queue.empty()) {
        int current = queue.front();
        queue.pop();
        
        auto neighbors = graph.getAdjacencyList(current);
        for (int neighbor : neighbors) {
            if (distances[neighbor] == INF) {
                distances[neighbor] = distances[current] + 1;
                queue.push(neighbor);
            }
        }
    }
    
    return distances;
}

std::vector<int> GraphAlgorithms::dfs(const Graph& graph, int start) {
    int n = graph.getVertexCount();
    std::vector<int> order;
    
    if (start < 0 || start >= n) return order;
    
    std::vector<bool> visited(n, false);
    std::vector<int> stack;
    stack.push_back(start);
    
    while (!stack.empty()) {
        int current = stack.back();
        stack.pop_back();
        
        if (!visited[current]) {
            visited[current] = true;
            order.push_back(current);
            
            auto neighbors = graph.getAdjacencyList(current);
            for (auto it = neighbors.rbegin(); it != neighbors.rend(); ++it) {
                if (!visited[*it]) {
                    stack.push_back(*it);
                }
            }
        }
    }
    
    return order;
}

void GraphAlgorithms::countRoutesDFS(
    const Graph& graph,
    int current,
    int end,
    std::vector<bool>& visited,
    int& count
) {
    if (current == end) {
        count++;
        return;
    }
    
    auto neighbors = graph.getAdjacencyList(current);
    for (int neighbor : neighbors) {
        if (!visited[neighbor]) {
            visited[neighbor] = true;
            countRoutesDFS(graph, neighbor, end, visited, count);
            visited[neighbor] = false;
        }
    }
}

void GraphAlgorithms::findAllRoutesDFS(
    const Graph& graph,
    int current,
    int end,
    std::vector<bool>& visited,
    std::vector<int>& path,
    std::vector<std::vector<int>>& allPaths
) {
    if (current == end) {
        allPaths.push_back(path);
        return;
    }
    
    auto neighbors = graph.getAdjacencyList(current);
    for (int neighbor : neighbors) {
        if (!visited[neighbor]) {
            visited[neighbor] = true;
            path.push_back(neighbor);
            findAllRoutesDFS(graph, neighbor, end, visited, path, allPaths);
            path.pop_back();
            visited[neighbor] = false;
        }
    }
}