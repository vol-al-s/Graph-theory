#include "../header/general.h"
#include <iostream>
#include <vector>
#include <algorithm>

// =============================================================================
//                  МИНИМАЛЬНОЕ ВЕРШИННОЕ ПОКРЫТИЕ
//
// Источник: tg11.pdf, слайд 37.
//
// Определение: множество вершин C ⊆ V такое, что для каждого ребра {u,v} ∈ E
// хотя бы один из концов лежит в C. Минимальное — это покрытие наименьшей
// мощности.
//
// На слайде 37 явно сказано, что задача NP-полная (сводится к задаче о
// гамильтоновом цикле). Поэтому быстрого точного алгоритма не существует;
// для маленьких графов мы используем полный перебор по битовой маске:
//   - 2^n подмножеств вершин;
//   - для каждого проверяем, покрывает ли оно все рёбра;
//   - запоминаем покрытие минимального размера.
// Сложность O(2^n · E). Для n до ~20-22 работает мгновенно.
//
// Дополнительная оптимизация: перебираем подмножества в порядке возрастания
// числа единиц в маске (popcount). Тогда первое же найденное покрытие будет
// минимальным, и можно сразу остановиться.
//
// Краевой случай n = 2: если ребро между ними есть, минимальное покрытие —
// одна из двух вершин (любая); если рёбер нет — пустое покрытие.
// =============================================================================

std::vector<int> Graph::minVertexCover(bool useSpanning) const {
    std::vector<int> answer;
    if (vertexCount == 0) return answer;

    int n = vertexCount;

    // ----- Подготовим матрицу, на которой будем считать покрытие. -----
    //
    // Если useSpanning == true — берём остов Краскала.
    // Иначе — исходный неориентированный граф.
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
    // ----- Краевой случай: рёбер нет — пустое покрытие. -----
    if (m == 0) return answer;

    // ----- Краевой случай n=2 ----------------------------------------
    // Если у нас всего 2 вершины и есть ребро между ними, любое из двух
    // покрытий {0} или {1} имеет размер 1 и является минимальным.
    // Нижеследующий перебор это и так найдёт, но проверим явно для надёжности.
    if (n == 2) {
        answer.push_back(0);   // вершина 1
        return answer;
    }
    // -----------------------------------------------------------------

    // Полный перебор по битовой маске.
    // Чтобы найденное первым же покрытие сразу было минимальным, идём
    // от маленьких подмножеств к большим. Сделаем это явно по размеру
    // подмножества.
    for (int size = 1; size <= n; size++) {
        // Генерируем все маски ровно с size единиц через "next subset of size".
        // Простейший способ: проходим все 2^n масок и берём только те, у которых
        // popcount == size. Это O(2^n) на размер, но для n <= 22 в сумме всё
        // равно 2^22 ≈ 4 миллиона — приемлемо.
        for (int mask = 0; mask < (1 << n); mask++) {
            // popcount
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

    // Дополнительная проверка для случая 2 вершин:
    if (vertexCount == 2) {
        std::cout << "\n(краевой случай: для графа из 2 вершин с одним ребром "
                     "минимальное покрытие — одна вершина; "
                     "число остовных деревьев = 1 — это K2)\n";
    }
}