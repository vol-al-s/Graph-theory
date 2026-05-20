#include "../header/general.h"
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <vector>

// =============================================================================
//                  ПОТОК МИНИМАЛЬНОЙ СТОИМОСТИ (Min-Cost Flow)
//
// Источник: tg7.pdf, слайды 40-41 (постановка задачи); tg5.pdf, слайды 53-59
// (Беллман-Форд как подпрограмма поиска кратчайшего пути).
//
// Задача: пустить ровно targetValue единиц потока из s в t так, чтобы
//         суммарная стоимость Σ cost(u,v) * flow(u,v) была минимальной.
//
// Алгоритм (SSP — Successive Shortest Path, "последовательные кратчайшие пути"):
//   1. F[u][v] := 0
//   2. Пока не пустили targetValue единиц:
//        a) В ОСТАТОЧНОЙ сети с весами = стоимостям найти кратчайший по
//           стоимости путь из s в t.
//        b) Если путь не найден — сеть не пропускает столько потока, выходим.
//        c) Найти узкое место δ (остаточная пропускная способность пути).
//        d) Пустить min(δ, оставшаяся_нужная_величина) единиц по этому пути.
//   3. Возвращаем фактически пущенный поток и его стоимость.
//
// КЛЮЧЕВАЯ ТОНКОСТЬ: в остаточной сети у обратной дуги стоимость
// равна -cost(прямой дуги). Это очевидно: если мы "отменяем" поток
// по прямой дуге (u,v), то деньги, которые мы за неё платили,
// возвращаются — отсюда и минус. Из-за этих ОТРИЦАТЕЛЬНЫХ весов
// Дейкстра здесь не работает, и приходится использовать Беллмана-Форда.
//
// Псевдокод релаксации (как в нашем bellmanford.cpp, но с поправкой
// на остаточную сеть и на знак стоимости):
//   for u, v in V x V:
//     // прямая дуга
//     if capacity[u][v] - flow[u][v] > 0 and dist[u] + cost[u][v] < dist[v]:
//        dist[v] := dist[u] + cost[u][v]; parent[v] := u
//     // обратная дуга (отмена потока)
//     if flow[u][v] > 0 and dist[v] - cost[u][v] < dist[u]:
//        dist[u] := dist[v] - cost[u][v]; parent[u] := v
// =============================================================================

static const int MCF_INF = 1000000000;

int Graph::minCostFlow(int targetValue, Matrix& flowOut,
                       long long& costOut, long long& iterations) const {
    iterations = 0;
    costOut    = 0;
    flowOut.resize(vertexCount);
    for (int i = 0; i < vertexCount; i++)
        for (int j = 0; j < vertexCount; j++)
            flowOut.at(i, j) = 0;

    if (vertexCount < 2 || targetValue <= 0) return 0;

    int s = getSource();
    int t = getSink();

    int totalFlow = 0;

    while (totalFlow < targetValue) {

        // -------- Беллман-Форд по остаточной сети с весами = стоимостям --------
        std::vector<int> dist(vertexCount, MCF_INF);
        std::vector<int> parent(vertexCount, -1);
        dist[s] = 0;

        for (int iter = 0; iter < vertexCount - 1; iter++) {
            bool changed = false;
            for (int u = 0; u < vertexCount; u++) {
                if (dist[u] == MCF_INF) continue;
                for (int v = 0; v < vertexCount; v++) {

                    // Прямая дуга u→v: есть остаточная ёмкость и стоимость +cost(u,v)
                    if (capacityMatrix.at(u, v) - flowOut.at(u, v) > 0) {
                        iterations++;
                        int w = costMatrix.at(u, v);
                        if (dist[u] + w < dist[v]) {
                            dist[v]   = dist[u] + w;
                            parent[v] = u;
                            changed = true;
                        }
                    }

                    // Обратная дуга v→u: возникает, если по прямой дуге u→v
                    // уже течёт поток. В остаточной сети это означает, что из v
                    // можно "вернуться" в u по цене -cost(u,v).
                    if (flowOut.at(u, v) > 0) {
                        iterations++;
                        int w = -costMatrix.at(u, v);
                        if (dist[v] != MCF_INF && dist[v] + w < dist[u]) {
                            dist[u]   = dist[v] + w;
                            parent[u] = v;
                            changed = true;
                        }
                    }
                }
            }
            if (!changed) break;
        }
        // -----------------------------------------------------------------------

        // Если сток недостижим в остаточной сети — больше потока пустить нельзя.
        if (dist[t] == MCF_INF) break;

        // 1) Найти δ — узкое место. Идём по родителям от t к s.
        //    Для каждого ребра пути определяем, прямое оно или обратное,
        //    и берём соответствующую остаточную пропускную способность.
        int delta = targetValue - totalFlow;     // больше не нужно тянуть
        for (int v = t; v != s; v = parent[v]) {
            int u = parent[v];
            int residual;
            if (capacityMatrix.at(u, v) > 0 && flowOut.at(u, v) < capacityMatrix.at(u, v)) {
                residual = capacityMatrix.at(u, v) - flowOut.at(u, v);
            } else {
                // значит, в пути использована обратная дуга (была пара v→u в исходной сети)
                residual = flowOut.at(v, u);
            }
            if (residual < delta) delta = residual;
        }

        // 2) Пустить δ единиц по найденному пути.
        for (int v = t; v != s; v = parent[v]) {
            int u = parent[v];
            if (capacityMatrix.at(u, v) > 0 && flowOut.at(u, v) < capacityMatrix.at(u, v)) {
                flowOut.at(u, v) += delta;
            } else {
                flowOut.at(v, u) -= delta;
            }
        }

        totalFlow += delta;
        // Стоимость одной единицы потока по этому пути = dist[t]
        // (для отрицательной части эта величина уже даёт правильный знак).
        costOut += (long long)delta * dist[t];
    }

    return totalFlow;
}

void Graph::printMinCostFlowResult() const {
    if (vertexCount == 0) {
        std::cout << "Сначала необходимо сгенерировать граф.\n";
        return;
    }
    if (capacityMatrix.size() == 0) {
        std::cout << "Сначала необходимо сгенерировать матрицы.\n";
        return;
    }

    this->printCapacityMatrix();
    this->printCostMatrix();

    // Сначала надо знать максимальный поток, чтобы взять от него 2/3.
    Matrix maxFlowMatrix;
    long long ffIter = 0;
    int maxFlow = fordFulkersonMaxFlow(maxFlowMatrix, ffIter);

    int target = (2 * maxFlow) / 3;    // целочисленное деление, как и требует условие

    std::cout << "\nМаксимальный поток в сети: " << maxFlow << "\n";
    std::cout << "Целевая величина потока [2/3 * max] = " << target << "\n";

    if (target <= 0) {
        std::cout << "Целевая величина равна 0 — поток нулевой, стоимость 0.\n";
        return;
    }

    Matrix flow;
    long long cost = 0, iter = 0;
    int delivered = minCostFlow(target, flow, cost, iter);

    this->printFlowMatrix(flow, "Матрица потока минимальной стоимости:");

    std::cout << "\nИсточник: вершина " << getSource() + 1
              << ", сток: вершина "    << getSink()   + 1 << "\n";
    std::cout << "Запрошенная величина потока: " << target << "\n";
    std::cout << "Фактически проведённая величина: " << delivered << "\n";
    std::cout << "Суммарная стоимость: " << cost << "\n";
    std::cout << "Количество итераций релаксации (Беллман-Форд): " << iter << "\n";

    if (delivered < target) {
        std::cout << "(сеть не пропускает " << target
                  << " единиц — это нормально для данной структуры дуг)\n";
    }
}