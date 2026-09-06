#include "../header/general.h"
#include <iostream>
#include <vector>
#include <algorithm>


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

    if (n == 2) {
        answer.push_back(0);
        return answer;
    }

    for (int size = 1; size <= n; size++) {
        for (int mask = 0; mask < (1 << n); mask++) {
            int bits = __builtin_popcount((unsigned)mask);
            if (bits != size) continue;

            bool covers = true;
            for (const auto& e : edges) {
                bool u_in = (mask >> e.first)  & 1;
                bool v_in = (mask >> e.second) & 1;
                if (!u_in && !v_in) { covers = false; break; }
            }
            if (covers) {
                for (int v = 0; v < n; v++) {
                    if ((mask >> v) & 1) answer.push_back(v);
                }
                return answer;
            }
        }
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
                     "минимальное покрытие — одна вершина; "
                     "число остовных деревьев = 1 — это K2)\n";
    }
}