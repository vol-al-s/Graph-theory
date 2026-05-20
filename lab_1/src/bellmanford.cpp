#include "../header/general.h"
#include <iostream>
#include <algorithm>
#include <deque>
#include <vector>

// =============================================================================
//                  АЛГОРИТМ БЕЛЛМАНА-МУРА (Bellman-Moore, SPFA)
//
// Источник: книга, Глава 8, раздел 8.7 "Ослабление напряжения всех ребер:
// алгоритм Беллмана-Форда", подраздел "Улучшение Мура" (рис. 8.16).
//
// Это FIFO-вариант алгоритма Беллмана-Форда, предложенный Эдвардом Муром в
// 1957 г. Идея: вместо того чтобы на каждой итерации перебирать ВСЕ дуги
// графа (как в классическом Беллмане-Форде), мы держим очередь "интересных"
// вершин — тех, чья метка dist[v] недавно уменьшилась. Только для таких
// вершин имеет смысл повторно релаксировать исходящие из них дуги.
//
// Псевдокод из книги (рис. 8.16):
//
//   Moore(s):
//       InitSSSP(s)                       // dist[s]=0, dist[v]=INF, parent[v]=-1
//       Push(s)
//       Push(✠)                           // ✠ — маркер конца этапа
//       while очередь содержит хотя бы одну вершину:
//           u ← Pull()
//           if u = ✠:
//               Push(✠)                   // начало следующего этапа
//           else:
//               for all edges u → v:
//                   if u → v is tense:    // dist[u] + w(u→v) < dist[v]
//                       Relax(u → v)
//                       if v не в очереди:
//                           Push(v)
//
// Ключевые тонкости из текста книги (стр. 356-358):
//
//  (1) ПЕРЕД ВСТАВКОЙ ПРОВЕРЯЕМ, в очереди ли уже эта вершина: "очередь всегда
//      содержит не более одной копии каждой вершины". Это критично для
//      гарантии сложности O(VE).
//
//  (2) МАРКЕР ✠ разделяет процесс на ЭТАПЫ. Один этап Мура соответствует
//      одной итерации классического Беллмана-Форда. Каждый этап начинается,
//      когда маркер записывается в очередь, и заканчивается, когда маркер
//      снова извлекается.
//
//  (3) ДЕТЕКЦИЯ ОТРИЦАТЕЛЬНОГО ЦИКЛА (стр. 358): "исходный граф содержит
//      отрицательный цикл, если и только если очередь остается не пустой
//      сразу после извлечения из нее маркера в (V-1)-й раз". То есть если за
//      V-1 этапов алгоритм не сошёлся — цикл есть.
//
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

    // InitSSSP: dist[s] = 0, всё остальное уже INF.
    result.distances[start] = 0;

    // Очередь будет хранить int >= 0 (номер вершины) и специальное значение
    // PHASE_MARKER для маркера ✠. Берём -1 — он не пересекается с номерами вершин.
    const int PHASE_MARKER = -1;

    // Используем std::deque вместо std::queue, чтобы было проще проверять
    // "в очереди ли уже эта вершина" через отдельный bool-вектор inQueue.
    std::deque<int> queue;
    std::vector<bool> inQueue(vertexCount, false);

    // Push(s); Push(✠) — стартовая инициализация (см. псевдокод).
    queue.push_back(start);
    inQueue[start] = true;
    queue.push_back(PHASE_MARKER);

    // Счётчик ИЗВЛЕЧЕНИЙ маркера = числу завершённых этапов.
    // Согласно книге, если за V-1 этапов алгоритм не остановился — есть
    // отрицательный цикл. Поэтому ограничиваем число этапов сверху.
    int phasesCompleted = 0;

    while (!queue.empty()) {
        // Pull() — извлечение из головы очереди (FIFO).
        int u = queue.front();
        queue.pop_front();

        if (u == PHASE_MARKER) {
            phasesCompleted++;

            // Если в очереди после извлечения маркера НИЧЕГО не осталось —
            // значит, на прошедшем этапе никаких релаксаций не было, и
            // алгоритм сошёлся. Это естественный выход (соответствует
            // ранней остановке в классическом Беллмане-Форде).
            if (queue.empty()) break;

            // Проверка на отрицательный цикл (см. книгу, стр. 358):
            // если уже сделано V-1 этапов, а очередь всё ещё не пуста —
            // цикл существует.
            if (phasesCompleted >= vertexCount - 1) {
                result.hasNegativeCycle = true;
                break;
            }

            // Начало следующего этапа: снова кладём маркер в конец очереди.
            queue.push_back(PHASE_MARKER);
            continue;
        }

        // u вышла из очереди — освобождаем флаг "в очереди".
        inQueue[u] = false;

        // Перебираем все исходящие дуги u → v. В нашем матричном
        // представлении это все v, для которых weightMatrix[u][v] != INF.
        for (int v = 0; v < vertexCount; v++) {
            int w = weightMatrix.at(u, v);
            if (w == INF) continue;          // дуги нет

            result.iterations++;             // одна попытка релаксации = одна итерация

            // Проверка "напряжённости" дуги u → v:
            // dist[u] + w(u→v) < dist[v]  ⇔  релаксация что-то даст.
            // (dist[u] == BF_INF здесь невозможно — u только что вышла из
            //  очереди, значит её метка была когда-то обновлена и конечна.)
            if (result.distances[u] + w < result.distances[v]) {
                result.distances[v] = result.distances[u] + w;
                result.parent[v]    = u;

                // Если v ещё не в очереди — добавить её. Это и есть та самая
                // вторая модификация Мура: "проверяется, находится ли уже
                // вершина в FIFO-очереди перед её вставкой".
                if (!inQueue[v]) {
                    queue.push_back(v);
                    inQueue[v] = true;
                }
            }
        }
    }

    return result;
}

// =============================================================================
// Восстановление пути и печать остаются БЕЗ ИЗМЕНЕНИЙ — они работают только
// с полями distances и parent, которые алгоритм Мура заполняет точно так же,
// как и классический Беллман-Форд.
// =============================================================================

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