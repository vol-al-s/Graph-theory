#include "../header/general.h"
#include <iostream>

void Graph::dfsEdgesUtil(bool useOriented,
                         int v,
                         std::vector<bool>& visited,
                         std::vector<std::vector<bool>>& visitedEdges,
                         int& iterations) const {
    const Matrix& matrix = useOriented ? orientedMatrix : undirectedMatrix;

    visited[v] = true;

    for (int u = 0; u < vertexCount; u++) {
        if (matrix.at(v, u) == 1) {
            iterations++;

            if (!visitedEdges[v][u]) {
                std::cout << v + 1 << " -> " << u + 1 << "\n";

                visitedEdges[v][u] = true;

                // Для неориентированного графа ребро считаем тем же самым в обе стороны
                if (!useOriented) {
                    visitedEdges[u][v] = true;
                }

                if (!visited[u]) {
                    dfsEdgesUtil(useOriented, u, visited, visitedEdges, iterations);
                }
            }
        }
    }
}

void Graph::dfsEdges(bool useOriented, int start) const {
    if (start < 0 || start >= vertexCount) {
        std::cout << "Некорректная стартовая вершина.\n";
        return;
    }

    std::vector<bool> visited(vertexCount, false);
    std::vector<std::vector<bool>> visitedEdges(
        vertexCount, std::vector<bool>(vertexCount, false)
    );

    int iterations = 0;

    if (useOriented) {
        std::cout << "\nОбход рёбер ориентированного графа поиском в глубину:\n";
    } else {
        std::cout << "\nОбход рёбер неориентированного графа поиском в глубину:\n";
    }

    // Сначала стартуем из выбранной вершины
    dfsEdgesUtil(useOriented, start, visited, visitedEdges, iterations);

    // Затем, как в образце, добираем оставшиеся вершины
    for (int i = start; i < vertexCount; i++) {
        if (!visited[i]) {
            dfsEdgesUtil(useOriented, i, visited, visitedEdges, iterations);
        }
    }

    std::cout << "Количество итераций: " << iterations << "\n";
}

void Graph::printDfsTraversal(bool useOriented, int start) const {
    if (useOriented == true) {
        (*this).printOriented();
    } else if (useOriented == false) {
        (*this).printUndirected();
    }
    dfsEdges(useOriented, start);
}