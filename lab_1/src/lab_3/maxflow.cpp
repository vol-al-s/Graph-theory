#include "../header/general.h"
#include <iostream>
#include <iomanip>
#include <queue>
#include <algorithm>
#include <cstdlib>


void Graph::generateCapacityAndCostMatrices(const PascalDistribution& distribution) {
    capacityMatrix.resize(vertexCount);
    costMatrix.resize(vertexCount);

    for (int i = 0; i < vertexCount; i++) {
        for (int j = 0; j < vertexCount; j++) {
            capacityMatrix.at(i, j) = 0;
            costMatrix.at(i, j)     = 0;
        }
    }

    if (vertexCount == 2 && orientedMatrix.at(0, 1) == 1) {
        capacityMatrix.at(0, 1) = 1;
        costMatrix.at(0, 1)     = distribution.generate() + 15;
        return;
    }

    for (int i = 0; i < vertexCount; i++) {
        for (int j = 0; j < vertexCount; j++) {
            if (orientedMatrix.at(i, j) == 1) {
                capacityMatrix.at(i, j) = distribution.generate() + 1 + rand()%15;
                costMatrix.at(i, j)     = distribution.generate() + 1 + rand()%15;
            }
        }
    }
}

const Matrix& Graph::getCapacityMatrix() const { return capacityMatrix; }
const Matrix& Graph::getCostMatrix()     const { return costMatrix; }



std::vector<int> Graph::findSources() const {
    std::vector<int> sources;
    if (capacityMatrix.size() != vertexCount) return sources;

    for (int v = 0; v < vertexCount; v++) {
        bool hasIncoming = false;
        for (int u = 0; u < vertexCount; u++) {
            if (capacityMatrix.at(u, v) > 0) { hasIncoming = true; break; }
        }
        if (!hasIncoming) sources.push_back(v);
    }
    return sources;
}

std::vector<int> Graph::findSinks() const {
    std::vector<int> sinks;
    if (capacityMatrix.size() != vertexCount) return sinks;

    for (int v = 0; v < vertexCount; v++) {
        bool hasOutgoing = false;
        for (int w = 0; w < vertexCount; w++) {
            if (capacityMatrix.at(v, w) > 0) { hasOutgoing = true; break; }
        }
        if (!hasOutgoing) sinks.push_back(v);
    }
    return sinks;
}



static void printMatrixGeneric(const Matrix& m, const std::string& title) {
    int n = m.size();
    std::cout << "\n" << title << "\n\n    ";
    for (int j = 0; j < n; j++) {
        if (j == 0) std::cout << std::setw(1) << "| " << std::setw(4) << "v" << j+1;
        else        std::cout << std::setw(5) << "v" << j+1;
    }
    std::cout << "\n";
    for (int k = 0; k < (6 + n * 6); k++) std::cout << "-";
    std::cout << "\n";
    for (int i = 0; i < n; i++) {
        std::cout << std::setw(2) << "v" << i+1 << " |";
        for (int j = 0; j < n; j++) {
            if (m.at(i, j) == 0) std::cout << std::setw(6) << ".";
            else                 std::cout << std::setw(6) << m.at(i, j);
        }
        std::cout << "\n";
    }
}

void Graph::printCapacityMatrix() const {
    printMatrixGeneric(capacityMatrix, "Матрица пропускных способностей:");
}
void Graph::printCostMatrix() const {
    printMatrixGeneric(costMatrix, "Матрица стоимостей:");
}
void Graph::printFlowMatrix(const Matrix& flow, const std::string& title) const {
    printMatrixGeneric(flow, title);
}


bool Graph::bfsAugmentingPath(const Matrix& capacity, const Matrix& flow,
                              int s, int t, std::vector<int>& parent,
                              long long& iterations) const {
    parent.assign(vertexCount, -1);
    parent[s] = s;

    std::queue<int> q;
    q.push(s);

    while (!q.empty()) {
        iterations++;
        int u = q.front(); q.pop();

        for (int v = 0; v < vertexCount; v++) {
            if (parent[v] != -1) continue;

            int residual = (capacity.at(u, v) - flow.at(u, v)) + flow.at(v, u);

            if (residual > 0) {
                parent[v] = u;
                if (v == t) return true;
                q.push(v);
            }
        }
    }
    return false;
}

int Graph::fordFulkersonMaxFlow(int source, int sink,
                                Matrix& flowOut,
                                std::vector<AugmentingPath>& augmentingPaths,
                                long long& iterations) const {
    iterations = 0;
    augmentingPaths.clear();
    flowOut.resize(vertexCount);
    for (int i = 0; i < vertexCount; i++)
        for (int j = 0; j < vertexCount; j++)
            flowOut.at(i, j) = 0;

    if (vertexCount < 2) return 0;
    if (source < 0 || source >= vertexCount) return 0;
    if (sink   < 0 || sink   >= vertexCount) return 0;
    if (source == sink) return 0;

    int maxFlow = 0;
    std::vector<int> parent;

    while (bfsAugmentingPath(capacityMatrix, flowOut, source, sink, parent, iterations)) {


        std::vector<int> path;
        for (int v = sink; v != source; v = parent[v]) path.push_back(v);
        path.push_back(source);
        std::reverse(path.begin(), path.end());

        int delta = INF;
        for (int v = sink; v != source; v = parent[v]) {
            int u = parent[v];
            int residual = (capacityMatrix.at(u, v) - flowOut.at(u, v))
                         + flowOut.at(v, u);
            if (residual < delta) delta = residual;
        }

        for (int v = sink; v != source; v = parent[v]) {
            int u = parent[v];
            if (capacityMatrix.at(u, v) > 0 &&
                flowOut.at(u, v) < capacityMatrix.at(u, v)) {
                int direct = capacityMatrix.at(u, v) - flowOut.at(u, v);
                int add    = std::min(delta, direct);
                flowOut.at(u, v) += add;
                int rest = delta - add;
                if (rest > 0) flowOut.at(v, u) -= rest;
            } else {
                flowOut.at(v, u) -= delta;
            }
        }

        AugmentingPath ap;
        ap.vertices = path;
        ap.delta = delta;
        augmentingPaths.push_back(ap);

        maxFlow += delta;
    }
    return maxFlow;
}

void Graph::printMaxFlowResult(int source, int sink) const {
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

    Matrix flow;
    long long iterations = 0;
    std::vector<AugmentingPath> paths;
    int maxFlow = fordFulkersonMaxFlow(source, sink, flow, paths, iterations);

    std::cout << "\nИсток: вершина " << source + 1
              << ", сток: вершина "  << sink + 1 << "\n";

    if (paths.empty()) {
        std::cout << "\nУвеличивающих цепей не найдено — из истока в сток в этой сети "
                     "нет ни одного пути с положительной пропускной способностью.\n";
    } else {
        std::cout << "\nНайденные увеличивающие цепи:\n";
        for (size_t i = 0; i < paths.size(); i++) {
            const AugmentingPath& p = paths[i];
            std::cout << "  \n" << (i + 1) << ") ";
            for (size_t k = 0; k < p.vertices.size(); k++) {
                std::cout << p.vertices[k] + 1;
                if (k + 1 < p.vertices.size()) std::cout << " -> ";
            }
            //std::cout << "   δ = " << p.delta << "\n";
        }
    }

    this->printFlowMatrix(flow, "Итоговая матрица потока (Форд-Фалкерсон):");

    std::cout << "\nВеличина максимального потока: " << maxFlow << "\n";
    /*std::cout << "Количество итераций (узлов, извлечённых из очереди BFS): "
              << iterations << "\n";*/
}