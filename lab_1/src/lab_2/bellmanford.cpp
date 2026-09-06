#include "../header/general.h"
#include <iostream>
#include <algorithm>
#include <deque>
#include <vector>

static const int BF_INF = 1000000000;

BellmanFordResult Graph::bellmanFord(int start) const {
    BellmanFordResult result;
    result.distances.assign(vertexCount, BF_INF);
    result.parent.assign(vertexCount, -1);
    result.hasNegativeCycle = false;
    result.iterations = 0;

    if (vertexCount == 0) return result;
    if (start < 0 || start >= vertexCount) return result;

    result.distances[start] = 0;

    const int PHASE_MARKER = -1;

    std::deque<int> queue;
    std::vector<bool> inQueue(vertexCount, false);

    queue.push_back(start);
    inQueue[start] = true;
    queue.push_back(PHASE_MARKER);

    int phasesCompleted = 0;

    while (!queue.empty()) {
        int u = queue.front();
        queue.pop_front();

        if (u == PHASE_MARKER) {
            phasesCompleted++;

            if (queue.empty()) break;

            if (phasesCompleted >= vertexCount - 1) {
                result.hasNegativeCycle = true;
                break;
            }

            queue.push_back(PHASE_MARKER);
            continue;
        }

        inQueue[u] = false;


        for (int v = 0; v < vertexCount; v++) {
            int w = weightMatrix.at(u, v);
            if (w == INF) continue;  

            result.iterations++; 

            if (result.distances[u] + w < result.distances[v]) {
                result.distances[v] = result.distances[u] + w;
                result.parent[v]    = u;

                if (!inQueue[v]) {
                    queue.push_back(v);
                    inQueue[v] = true;
                }
            }
        }
    }

    return result;
}



std::vector<int> Graph::restoreBellmanFordPath(int start, int finish,
                                               const std::vector<int>& parent) const {
    std::vector<int> path;

    if (start  < 0 || start  >= vertexCount) return path;
    if (finish < 0 || finish >= vertexCount) return path;

    if (start == finish) {
        path.push_back(start);
        return path;
    }

    int current = finish;
    while (current != -1) {
        path.push_back(current);
        if (current == start) break;
        current = parent[current];
    }

    if (path.empty() || path.back() != start) {
        path.clear();
        return path;
    }

    std::reverse(path.begin(), path.end());
    return path;
}

void Graph::printBellmanFordResult(int start, int finish) const {
    this->printWeightMatrix();

    if (start  < 0 || start  >= vertexCount ||
        finish < 0 || finish >= vertexCount) {
        std::cout << "Некорректные номера вершин.\n";
        return;
    }

    BellmanFordResult result = bellmanFord(start);

    std::cout << "Источник: " << start + 1 << "\n";
    std::cout << "Количество итераций: " << result.iterations << "\n";

    if (result.hasNegativeCycle) {
        std::cout << "Обнаружен достижимый из источника отрицательный цикл. "
                  << "Кратчайшие пути не определены.\n";
        return;
    }

    std::cout << "\nВектор расстояний от вершины " << start + 1 << ":\n";
    for (int i = 0; i < vertexCount; i++) {
        std::cout << "  До вершины " << i + 1 << ": ";
        if (result.distances[i] == BF_INF) std::cout << "-";
        else                                std::cout << result.distances[i];
        std::cout << "\n";
    }

    std::cout << "\nКратчайший путь из " << start + 1
              << " в " << finish + 1 << ":\n";

    if (result.distances[finish] == BF_INF) {
        std::cout << "Путь не существует.\n";
        return;
    }

    std::vector<int> path = restoreBellmanFordPath(start, finish, result.parent);
    if (path.empty()) {
        std::cout << "Путь не существует.\n";
        return;
    }

    std::cout << "Длина пути: " << result.distances[finish] << "\n";
    std::cout << "Путь: ";
    for (size_t i = 0; i < path.size(); i++) {
        std::cout << path[i] + 1;
        if (i + 1 < path.size()) std::cout << " -> ";
    }
    std::cout << "\n";
}