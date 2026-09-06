#include "../header/general.h"
#include <iostream>
#include <algorithm>
#include <iomanip>

namespace {
    struct DSU {
        std::vector<int> parent;
        std::vector<int> rank_;
        DSU(int n) : parent(n), rank_(n, 0) {
            for (int i = 0; i < n; i++) parent[i] = i;
        }
        int find(int x) {
            while (parent[x] != x) {
                parent[x] = parent[parent[x]];
                x = parent[x];
            }
            return x;
        }
        bool unite(int a, int b) {
            a = find(a); b = find(b);
            if (a == b) return false;
            if (rank_[a] < rank_[b]) std::swap(a, b);
            parent[b] = a;
            if (rank_[a] == rank_[b]) rank_[a]++;
            return true;
        }
    };
}

std::vector<Graph::MstEdge> Graph::kruskalMST(long long& totalWeight,
                                              bool& isConnected) const {
    std::vector<MstEdge> result;
    totalWeight = 0;
    isConnected = false;

    if (vertexCount == 0) return result;
    if (vertexCount == 1) { isConnected = true; return result; } 

    std::vector<MstEdge> edges;
    bool weightsPresent = (weightMatrix.size() == vertexCount);

    for (int i = 0; i < vertexCount; i++) {
        for (int j = i + 1; j < vertexCount; j++) {
            if (undirectedMatrix.at(i, j) != 1) continue;
            int w = 1;
            if (weightsPresent) {
                int wij = weightMatrix.at(i, j);
                int wji = weightMatrix.at(j, i);
                if (wij != INF) w = wij;
                else if (wji != INF) w = wji;
                else w = 1;
            }
            edges.push_back({i, j, w});
        }
    }

    std::sort(edges.begin(), edges.end(),
              [](const MstEdge& a, const MstEdge& b){ return a.weight < b.weight; });


    DSU dsu(vertexCount);
    int added = 0;
    for (const MstEdge& e : edges) {
        if (dsu.unite(e.u, e.v)) {
            result.push_back(e);
            totalWeight += e.weight;
            added++;
            if (added == vertexCount - 1) break;
        }
    }

    isConnected = (added == vertexCount - 1);
    return result;
}

void Graph::printKruskalResult() const {
    if (vertexCount == 0) {
        std::cout << "Сначала необходимо сгенерировать граф.\n";
        return;
    }
    this->printUndirected();
    if (weightMatrix.size() == vertexCount) this->printWeightMatrix();

    long long total = 0;
    bool connected = false;
    std::vector<MstEdge> mst = kruskalMST(total, connected);

    if (!connected) {
        std::cout << "\nГраф несвязный — минимальный остов не существует.\n";
        std::cout << "(построен лес из " << mst.size() << " рёбер)\n";
    } else {
        std::cout << "\nМинимальный остов (алгоритм Краскала):\n";
        for (const MstEdge& e : mst) {
            std::cout << "  " << e.u + 1 << " -- " << e.v + 1
                      << "   вес = " << e.weight << "\n";
        }
        std::cout << "Суммарный вес остова: " << total << "\n";
    }
}