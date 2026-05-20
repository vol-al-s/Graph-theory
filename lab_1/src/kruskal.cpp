#include "../header/general.h"
#include <iostream>
#include <algorithm>
#include <iomanip>

// =============================================================================
//                          АЛГОРИТМ КРАСКАЛА
//
// Источник: tg9.pdf, слайд 54.
//
// Псевдокод из лекции:
//   T := ∅; k := 1
//   for i from 1 to p-1 do
//       while z(T + E[k]) > 0 do      { пока добавление ребра создаст цикл }
//           k := k + 1
//       end while
//       T := T + E[k]                  { добавляем безопасное ребро }
//       k := k + 1
//   end for
//
// Где E[] — список рёбер, отсортированный по возрастанию веса;
// z(...) = 1 если добавление ребра создаёт цикл, 0 иначе.
//
// Для эффективной проверки "создаст ли ребро цикл" используется структура
// "СНМ" (DSU, Disjoint Set Union): если оба конца ребра уже в одной компоненте,
// добавление создаст цикл. Это классическая реализация Краскала за O(E·log E).
// =============================================================================

// --- Простая реализация СНМ с эвристиками path compression и rank ---
namespace {
    struct DSU {
        std::vector<int> parent;
        std::vector<int> rank_;
        DSU(int n) : parent(n), rank_(n, 0) {
            for (int i = 0; i < n; i++) parent[i] = i;
        }
        int find(int x) {
            while (parent[x] != x) {
                parent[x] = parent[parent[x]];   // path compression (halving)
                x = parent[x];
            }
            return x;
        }
        // вернёт true, если объединили (то есть были в разных компонентах)
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
    if (vertexCount == 1) { isConnected = true; return result; }  // тривиальный остов

    // 1) Собираем все рёбра НЕориентированного графа с весами.
    //    Веса берём из weightMatrix; если она хранит INF для "ребра нет",
    //    то такие пары пропускаем. Если граф ещё не имеет весов — берём вес = 1.
    std::vector<MstEdge> edges;
    bool weightsPresent = (weightMatrix.size() == vertexCount);

    for (int i = 0; i < vertexCount; i++) {
        for (int j = i + 1; j < vertexCount; j++) {
            if (undirectedMatrix.at(i, j) != 1) continue;
            int w = 1;
            if (weightsPresent) {
                int wij = weightMatrix.at(i, j);
                int wji = weightMatrix.at(j, i);
                // В неориентированном случае берём то значение, которое не INF.
                // Веса в этом проекте могут оказаться только в одной из клеток
                // (т.к. весовая матрица создавалась поверх ориентированного графа).
                if (wij != INF) w = wij;
                else if (wji != INF) w = wji;
                else w = 1;
            }
            edges.push_back({i, j, w});
        }
    }

    // 2) Сортируем рёбра по возрастанию веса. Это ключевая часть жадного алгоритма.
    std::sort(edges.begin(), edges.end(),
              [](const MstEdge& a, const MstEdge& b){ return a.weight < b.weight; });

    // 3) Идём по отсортированным рёбрам и добавляем безопасные.
    DSU dsu(vertexCount);
    int added = 0;
    for (const MstEdge& e : edges) {
        if (dsu.unite(e.u, e.v)) {
            result.push_back(e);
            totalWeight += e.weight;
            added++;
            // По построению остова надо ровно p-1 рёбер. Дальше можно не идти.
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