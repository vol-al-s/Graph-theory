#ifndef ALGORITHMS_H
#define ALGORITHMS_H

#include "graph.h"
#include <vector>
#include <map>

/**
 * Алгоритмы для работы с графами
 * На основе учебника Новикова (главы 7, 8, 9)
 */
class GraphAlgorithms {
public:
    // Экцентриситеты и центр графа (Новиков, раздел 7.2.6)
    static std::vector<int> calculateEccentricities(const Graph& graph);
    static std::vector<int> findGraphCenter(const Graph& graph);
    static std::vector<std::pair<int, int>> findDiametralVertices(const Graph& graph);
    static int calculateDiameter(const Graph& graph);
    
    // Метод Шимбеля для кратчайших путей (аналог алгоритма Флойда, Новиков 8.6.2)
    static std::vector<std::vector<int>> shimbelAlgorithm(
        const Graph& graph,
        bool allowNegative = false
    );
    
    // Поиск маршрутов (Новиков, раздел 7.2.3)
    static bool hasRoute(const Graph& graph, int start, int end);
    static int countRoutes(const Graph& graph, int start, int end);
    static std::vector<std::vector<int>> findAllRoutes(const Graph& graph, int start, int end);
    
    // Поиск в ширину (Новиков, раздел 7.4.6)
    static std::vector<int> bfs(const Graph& graph, int start);
    
    // Поиск в глубину (Новиков, раздел 7.4.6)
    static std::vector<int> dfs(const Graph& graph, int start);
    
private:
    // Вспомогательный метод для подсчета маршрутов
    static void countRoutesDFS(
        const Graph& graph,
        int current,
        int end,
        std::vector<bool>& visited,
        int& count
    );
    
    // Вспомогательный метод для поиска всех маршрутов
    static void findAllRoutesDFS(
        const Graph& graph,
        int current,
        int end,
        std::vector<bool>& visited,
        std::vector<int>& path,
        std::vector<std::vector<int>>& allPaths
    );
};

#endif // ALGORITHMS_H