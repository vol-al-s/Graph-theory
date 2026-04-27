#include "../header/general.h"
#include <iostream>
#include <algorithm>

static const int BF_INF = 1000000000;

BellmanFordResult Graph::bellmanFord(int start) const {
    BellmanFordResult result;
    result.distances.assign(vertexCount, BF_INF);
    result.parent.assign(vertexCount, -1);
    result.hasNegativeCycle = false;

    if (start < 0 || start >= vertexCount) {
        return result;
    }

    result.distances[start] = 0;

    // Основные |V|-1 итераций релаксации
    for (int iter = 0; iter < vertexCount - 1; iter++) {
        bool changed = false;

        for (int u = 0; u < vertexCount; u++) {
            if (result.distances[u] == BF_INF) {
                continue;
            }

            for (int v = 0; v < vertexCount; v++) {
                if (weightMatrix.at(u, v) == INF) {
                    continue;
                }

                int w = weightMatrix.at(u, v);

                if (result.distances[u] + w < result.distances[v]) {
                    result.distances[v] = result.distances[u] + w;
                    result.parent[v] = u;
                    changed = true;
                }
            }
        }

        // Если за итерацию ничего не изменилось,
        // дальше можно не продолжать
        if (!changed) {
            break;
        }
    }

    // Проверка на достижимый отрицательный цикл
    for (int u = 0; u < vertexCount; u++) {
        if (result.distances[u] == BF_INF) {
            continue;
        }

        for (int v = 0; v < vertexCount; v++) {
            if (weightMatrix.at(u, v) == INF) {
                continue;
            }

            int w = weightMatrix.at(u, v);

            if (result.distances[u] + w < result.distances[v]) {
                result.hasNegativeCycle = true;
                return result;
            }
        }
    }

    return result;
}

std::vector<int> Graph::restoreBellmanFordPath(int start, int finish,
                                               const std::vector<int>& parent) const {
    std::vector<int> path;

    if (start < 0 || start >= vertexCount || finish < 0 || finish >= vertexCount) {
        return path;
    }

    if (start == finish) {
        path.push_back(start);
        return path;
    }

    int current = finish;

    while (current != -1) {
        path.push_back(current);

        if (current == start) {
            break;
        }

        current = parent[current];
    }

    if (path.back() != start) {
        path.clear();
        return path;
    }

    std::reverse(path.begin(), path.end());
    return path;
}

void Graph::printBellmanFordResult(int start, int finish) const {
    (*this).printWeightMatrix();
    if (start < 0 || start >= vertexCount || finish < 0 || finish >= vertexCount) {
        std::cout << "Некорректные номера вершин.\n";
        return;
    }

    BellmanFordResult result = bellmanFord(start);

    std::cout << "\nРезультат алгоритма Беллмана-Форда\n";
    std::cout << "Источник: " << start + 1 << "\n";

    if (result.hasNegativeCycle) {
        std::cout << "Обнаружен достижимый отрицательный цикл. "
                  << "Кратчайшие пути не определены.\n";
        return;
    }

    std::cout << "\nВектор расстояний:\n";
    for (int i = 0; i < vertexCount; i++) {
        std::cout << "До вершины " << i + 1 << ": ";
        if (result.distances[i] == BF_INF) {
            std::cout << "-";
        } else {
            std::cout << result.distances[i];
        }
        std::cout << "\n";
    }

    std::cout << "\nКратчайший путь из " << start + 1
              << " в " << finish + 1 << ":\n";

    if (result.distances[finish] == BF_INF) {
        std::cout << "Путь не существует.\n";
        return;
    }

    std::vector<int> path =
        restoreBellmanFordPath(start, finish, result.parent);

    if (path.empty()) {
        std::cout << "Путь не существует.\n";
        return;
    }

    std::cout << "Длина пути: " << result.distances[finish] << "\n";
    std::cout << "Путь: ";

    for (int i = 0; i < (int)path.size(); i++) {
        std::cout << path[i] + 1;
        if (i + 1 < (int)path.size()) {
            std::cout << " -> ";
        }
    }
    std::cout << "\n";
}