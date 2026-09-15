#include "../header/general.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <cstdlib>


std::vector<int> Graph::minVertexCover(bool useSpanning) const {
    std::vector<int> answer;
    if (vertexCount == 0) return answer;

    int n = vertexCount;

    std::vector<std::pair<int,int>> edges;

    if (useSpanning) {
        long long total = 0;
        bool connected = false;
        std::vector<MstEdge> mst = kruskalMST(total, connected);
        for (const MstEdge& e : mst) edges.push_back({e.u, e.v});
    } else {
        for (int i = 0; i < n; i++) {
            for (int j = i + 1; j < n; j++) {
                if (undirectedMatrix.at(i, j) == 1) edges.push_back({i, j});
            }
        }
    }

    int m = (int)edges.size();
    if (m == 0) return answer;

std::vector<bool> edgeRemoved(m, false); // ребро уже покрыто и выброшено из графа
    std::vector<bool> inCover(n, false);     // вершина уже попала в решение S

    while (true) {
        // 1) собираем номера рёбер, которые ещё не покрыты
        std::vector<int> alive;
        for (int i = 0; i < m; i++) {
            if (!edgeRemoved[i]) alive.push_back(i);
        }

        // если непокрытых рёбер не осталось — алгоритм закончил работу
        if (alive.empty()) break;

        // 2) выбираем среди них одно случайное ребро e = (u, v)
        int idx = alive[rand() % (int)alive.size()];
        int u = edges[idx].first;
        int v = edges[idx].second;

        // 3) добавляем в решение S обе вершины ребра
        inCover[u] = true;
        inCover[v] = true;

        // 4) удаляем из графа все рёбра, инцидентные u или v
        for (int i = 0; i < m; i++) {
            if (edgeRemoved[i]) continue;
            int a = edges[i].first;
            int b = edges[i].second;
            if (a == u || a == v || b == u || b == v) {
                edgeRemoved[i] = true;
            }
        }
    }

    // 5) переводим отметки в список номеров вершин (сразу по возрастанию)
    for (int v = 0; v < n; v++) {
        if (inCover[v]) answer.push_back(v);
    }

    return answer;
}

void Graph::printVertexCoverResult(bool useSpanning) const {
    if (vertexCount == 0) {
        std::cout << "Сначала необходимо сгенерировать граф.\n";
        return;
    }

    if (useSpanning) {
        if (weightMatrix.size() != vertexCount) {
            std::cout << "Сначала необходимо сгенерировать весовую матрицу "
                         "(остов строится по ней).\n";
            return;
        }
        long long total = 0;
        bool connected = false;
        std::vector<MstEdge> mst = kruskalMST(total, connected);
        if (!connected) {
            std::cout << "Граф несвязный — остова не существует.\n";
            return;
        }
        std::cout << "\nИщем минимальное вершинное покрытие на ОСТОВЕ:\n";
        for (const MstEdge& e : mst) {
            std::cout << "  " << e.u + 1 << " -- " << e.v + 1
                      << "   вес = " << e.weight << "\n";
        }
    } else {
        std::cout << "\nИщем минимальное вершинное покрытие на исходном "
                     "неориентированном графе.\n";
        this->printUndirected();
    }

    std::vector<int> cover = minVertexCover(useSpanning);

    std::cout << "\nРазмер минимального вершинного покрытия: "
              << cover.size() << "\n";
    std::cout << "Вершины покрытия: ";
    if (cover.empty()) {
        std::cout << "(пусто — в графе нет рёбер)\n";
    } else {
        for (size_t i = 0; i < cover.size(); i++) {
            std::cout << cover[i] + 1;
            if (i + 1 < cover.size()) std::cout << ", ";
        }
        std::cout << "\n";
    }

    if (vertexCount == 2) {
        std::cout << "\n(краевой случай: для графа из 2 вершин с одним ребром "
                     "минимальное покрытие — одна вершина.";
    }
}