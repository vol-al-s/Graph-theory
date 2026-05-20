#include "../header/general.h"
#include <iostream>
#include <algorithm>

// =============================================================================
//                       АЛГОРИТМ БЕЛЛМАНА-ФОРДА
//
// Источник: tg5.pdf, слайды 53–59.
//
// Слайд 54 (классический псевдокод):
//   for i from 1 to p do
//       T[i] := C[1,i]            { начальное приближение по матрице весов }
//   end for
//   for i from 2 to p do
//       for j ∈ Γ⁻¹(i) do
//           T[i] := min(T[i], T[j] + C[j,i])   { пересчёт оценки длины пути }
//       end for
//   end for
//
// На практике берут чуть более общую форму с (|V|-1) проходом по всем дугам:
//   d[s] := 0;  d[v] := ∞ для v ≠ s
//   повторить |V|-1 раз:
//       для каждой дуги (u,v) с весом w:
//           if d[u] + w < d[v] then
//               d[v] := d[u] + w
//               parent[v] := u
//
// Слайд 59 (анализ + оптимизация):
//   1. Если за всю итерацию ни одна метка не уменьшилась — досрочный выход.
//   2. Чтобы детектировать достижимый из s отрицательный цикл, делаем
//      ровно |V| итераций (а не |V|-1) и смотрим, изменилось ли что-то
//      на последней — если да, цикл есть.
//
// Граф у вас задан матрицей весов weightMatrix размера V×V, где элемент INF
// означает «дуги нет». Эта матрица хранит ровно "C[j,i]" из псевдокода.
// =============================================================================

static const int BF_INF = 1000000000;

BellmanFordResult Graph::bellmanFord(int start) const {
    BellmanFordResult result;
    result.distances.assign(vertexCount, BF_INF);
    result.parent.assign(vertexCount, -1);
    result.hasNegativeCycle = false;
    result.iterations = 0;

    if (vertexCount == 0) return result;
    if (start < 0 || start >= vertexCount) return result;

    // d[s] := 0
    result.distances[start] = 0;

    // Внешний цикл: ровно vertexCount итераций (как на слайде 59).
    // На первых V-1 итерациях находим кратчайшие расстояния,
    // V-я итерация служит детектором отрицательного цикла.
    for (int iter = 0; iter < vertexCount; iter++) {
        bool changedThisPass = false;

        // Перебор всех дуг (u, v). В матричном представлении дуга (u, v)
        // существует тогда и только тогда, когда weightMatrix[u][v] != INF.
        // Это и есть "for j ∈ Γ⁻¹(i)" из лекции, только записанное в матричной форме.
        for (int u = 0; u < vertexCount; u++) {
            // Если d[u] = ∞, релаксировать через u бесполезно: ∞ + w = ∞.
            if (result.distances[u] == BF_INF) continue;

            for (int v = 0; v < vertexCount; v++) {
                int w = weightMatrix.at(u, v);
                if (w == INF) continue;   // дуги нет

                result.iterations++;      // ИТЕРАЦИЯ = одна попытка релаксации одной дуги

                // Условие релаксации: d[u] + w < d[v]
                if (result.distances[u] + w < result.distances[v]) {
                    // На последней (V-й) итерации это означает наличие
                    // достижимого из s отрицательного цикла.
                    if (iter == vertexCount - 1) {
                        result.hasNegativeCycle = true;
                        return result;
                    }
                    result.distances[v] = result.distances[u] + w;
                    result.parent[v]    = u;
                    changedThisPass     = true;
                }
            }
        }

        // Оптимизация со слайда 59: если за полный проход ничего не изменилось,
        // дальнейшие итерации бессмысленны (вектор расстояний уже зафиксирован).
        if (!changedThisPass) break;
    }

    return result;
}

// Восстановление пути по массиву предков parent.
// Идём от finish назад по parent[], пока не упрёмся в start или в -1.
// Если упёрлись в -1, значит вершина finish недостижима — возвращаем пустой путь.
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

    // Если последний элемент — не start, значит пути нет.
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

    std::cout << "\nРезультат алгоритма Беллмана-Форда\n";
    std::cout << "Источник: " << start + 1 << "\n";
    std::cout << "Количество итераций: " << result.iterations << "\n";

    if (result.hasNegativeCycle) {
        std::cout << "Обнаружен достижимый из источника отрицательный цикл. "
                  << "Кратчайшие пути не определены.\n";
        return;
    }

    // Вектор расстояний — обязательное требование лабораторной (пункт 2).
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