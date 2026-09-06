#include "../header/general.h"
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <vector>

static const int MCF_INF = 1000000000;

int Graph::minCostFlow(int source, int sink, int targetValue,
                       Matrix& flowOut,
                       long long& costOut, long long& iterations) const {
    iterations = 0;
    costOut    = 0;
    flowOut.resize(vertexCount);
    for (int i = 0; i < vertexCount; i++)
        for (int j = 0; j < vertexCount; j++)
            flowOut.at(i, j) = 0;

    if (vertexCount < 2 || targetValue <= 0) return 0;
    if (source < 0 || source >= vertexCount) return 0;
    if (sink   < 0 || sink   >= vertexCount) return 0;
    if (source == sink) return 0;

    int totalFlow = 0;

    while (totalFlow < targetValue) {


        std::vector<int> dist(vertexCount, MCF_INF);
        std::vector<int> parent(vertexCount, -1);
        dist[source] = 0;

        for (int iter = 0; iter < vertexCount - 1; iter++) {
            bool changed = false;
            for (int u = 0; u < vertexCount; u++) {
                if (dist[u] == MCF_INF) continue;
                for (int v = 0; v < vertexCount; v++) {

                    if (capacityMatrix.at(u, v) - flowOut.at(u, v) > 0) {
                        iterations++;
                        int w = costMatrix.at(u, v);
                        if (dist[u] + w < dist[v]) {
                            dist[v]   = dist[u] + w;
                            parent[v] = u;
                            changed = true;
                        }
                    }
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


        if (dist[sink] == MCF_INF) break; 
        int delta = targetValue - totalFlow;
        for (int v = sink; v != source; v = parent[v]) {
            int u = parent[v];
            int residual;
            if (capacityMatrix.at(u, v) > 0 && flowOut.at(u, v) < capacityMatrix.at(u, v)) {
                residual = capacityMatrix.at(u, v) - flowOut.at(u, v);
            } else {
                residual = flowOut.at(v, u);
            }
            if (residual < delta) delta = residual;
        }

        for (int v = sink; v != source; v = parent[v]) {
            int u = parent[v];
            if (capacityMatrix.at(u, v) > 0 && flowOut.at(u, v) < capacityMatrix.at(u, v)) {
                flowOut.at(u, v) += delta;
            } else {
                flowOut.at(v, u) -= delta;
            }
        }

        totalFlow += delta;
        costOut   += (long long)delta * dist[sink];
    }

    return totalFlow;
}

void Graph::printMinCostFlowResult(int source, int sink) const {
    if (vertexCount == 0) {
        std::cout << "Сначала необходимо сгенерировать граф.\n";
        return;
    }
    if (capacityMatrix.size() == 0) {
        std::cout << "Сначала необходимо сгенерировать матрицы.\n";
        return;
    }
    if (source < 0 || source >= vertexCount ||
        sink   < 0 || sink   >= vertexCount) {
        std::cout << "Некорректные номера истока/стока.\n";
        return;
    }
    if (source == sink) {
        std::cout << "Исток и сток должны различаться.\n";
        return;
    }

    this->printCapacityMatrix();
    this->printCostMatrix();

    Matrix maxFlowMatrix;
    long long ffIter = 0;
    std::vector<AugmentingPath> tmpPaths;
    int maxFlow = fordFulkersonMaxFlow(source, sink, maxFlowMatrix, tmpPaths, ffIter);

    int target = (2 * maxFlow) / 3;

    std::cout << "\nИсток: вершина " << source + 1
              << ", сток: вершина "  << sink + 1 << "\n";
    std::cout << "Максимальный поток в сети между этими вершинами: " << maxFlow << "\n";
    std::cout << "Целевая величина потока [2/3 * max] = " << target << "\n";

    if (target <= 0) {
        std::cout << "Целевая величина равна 0 — поток нулевой, стоимость 0.\n";
        return;
    }

    Matrix flow;
    long long cost = 0, iter = 0;
    int delivered = minCostFlow(source, sink, target, flow, cost, iter);

    this->printFlowMatrix(flow, "Матрица потока минимальной стоимости:");

    std::cout << "\nЗапрошенная величина потока: " << target << "\n";
    std::cout << "Фактически проведённая величина: " << delivered << "\n";
    std::cout << "Суммарная стоимость: " << cost << "\n";
    //std::cout << "Количество итераций релаксации (Беллман-Форд): " << iter << "\n";

    if (delivered < target) {
        std::cout << "(сеть не пропускает " << target
                  << " единиц между этими вершинами)\n";
    }
}