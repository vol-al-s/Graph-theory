#include "../header/general.h"
#include <iostream>
#include <stack>

// =============================================================================
//                           ПОИСК В ГЛУБИНУ (DFS)
//
// Реализация строго по псевдокоду со слайда 19 tg3.pdf
// (тот самый псевдокод с картинки к заданию).
//
//   for v ∈ V do x[v] := 0 end for       — обнуляем флаги отмеченности
//   select v ∈ V                          — выбираем стартовую вершину
//   v → T                                 — помещаем v в структуру данных T
//   x[v] := 1                             — отмечаем v
//   repeat
//       u ← T                             — извлекаем вершину из T
//       yield u                           — возвращаем её как очередную пройденную
//       for w ∈ Γ(u) do
//           if x[w] = 0 then
//               w → T                     — помещаем w в T
//               x[w] := 1                 — и сразу отмечаем
//           end if
//       end for
//   until T = ∅
//
// Если T — стек (LIFO), это поиск в глубину; если очередь (FIFO) — в ширину.
// Здесь T — std::stack<int>, поэтому это DFS.
//
// ВАЖНО: вершина отмечается В МОМЕНТ ПОМЕЩЕНИЯ В СТЕК, а не при извлечении.
// Это гарантирует, что одна и та же вершина не попадёт в стек дважды.
// =============================================================================

std::vector<int> Graph::dfsTraversal(bool useOriented, int start,
                                     long long& iterations) const {
    std::vector<int> order;            // последовательность пройденных вершин (выход алгоритма)
    iterations = 0;

    if (vertexCount == 0) return order;
    if (start < 0 || start >= vertexCount) return order;

    // Выбор матрицы смежности (Γ — список смежности; здесь — строка матрицы).
    const Matrix& matrix = useOriented ? orientedMatrix : undirectedMatrix;

    // Шаг "for v ∈ V do x[v] := 0":
    // массив отметок (1 = вершина уже была помещена в T хотя бы один раз).
    std::vector<bool> x(vertexCount, false);

    // T — наша структура данных. Для DFS — стек.
    std::stack<int> T;

    // "v → T" + "x[v] := 1": кладём стартовую вершину и сразу отмечаем её.
    T.push(start);
    x[start] = true;

    // repeat ... until T = ∅
    while (!T.empty()) {
        iterations++;                  // одна итерация внешнего цикла = одно "снятие" со структуры

        // "u ← T": извлекаем вершину из стека.
        int u = T.top();
        T.pop();

        // "yield u": эта вершина считается очередной пройденной.
        order.push_back(u);

        // "for w ∈ Γ(u) do": перебираем соседей u.
        //
        // Идём от vertexCount-1 к 0, чтобы при извлечении из стека
        // мы заходили сначала в соседа с меньшим номером — порядок обхода
        // получается более привычным (1, 2, 3, ... как «естественный» спуск
        // в первого попавшегося соседа). На корректность это не влияет.
        for (int w = vertexCount - 1; w >= 0; w--) {
            if (matrix.at(u, w) == 1 && !x[w]) {
                // "w → T" и "x[w] := 1" — кладём и сразу отмечаем.
                T.push(w);
                x[w] = true;
            }
        }
    }

    // Если граф несвязный, основной цикл прошёл только по компоненте,
    // содержащей start. Дозапускаем обход с каждой неотмеченной вершины,
    // чтобы получить полный обход графа (как и было в исходной версии).
    for (int v = 0; v < vertexCount; v++) {
        if (!x[v]) {
            T.push(v);
            x[v] = true;
            while (!T.empty()) {
                iterations++;
                int u = T.top();
                T.pop();
                order.push_back(u);
                for (int w = vertexCount - 1; w >= 0; w--) {
                    if (matrix.at(u, w) == 1 && !x[w]) {
                        T.push(w);
                        x[w] = true;
                    }
                }
            }
        }
    }

    return order;
}

void Graph::printDfsTraversal(bool useOriented, int start) const {
    if (start < 0 || start >= vertexCount) {
        std::cout << "Некорректная стартовая вершина.\n";
        return;
    }

    // Вывод матрицы — в том же стиле, что и у вас в исходной версии.
    if (useOriented) {
        this->printOriented();
        std::cout << "\nОбход вершин ориентированного графа поиском в глубину:\n";
    } else {
        this->printUndirected();
        std::cout << "\nОбход вершин неориентированного графа поиском в глубину:\n";
    }

    long long iterations = 0;
    std::vector<int> order = dfsTraversal(useOriented, start, iterations);

    // Последовательность вершин выводится построчно как переходы:
    //   1 -> 2
    //   2 -> 3
    //   ...
    for (size_t i = 0; i + 1 < order.size(); i++) {
        std::cout << order[i] + 1 << " -> " << order[i + 1] + 1 << "\n";
    }

    std::cout << "Количество итераций: " << iterations << "\n";
}