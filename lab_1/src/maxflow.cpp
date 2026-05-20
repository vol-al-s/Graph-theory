#include "../header/general.h"
#include <iostream>
#include <iomanip>
#include <queue>
#include <algorithm>
#include <cstdlib>

// =============================================================================
//          ГЕНЕРАЦИЯ МАТРИЦ ПРОПУСКНЫХ СПОСОБНОСТЕЙ И СТОИМОСТЕЙ
//
// Источник: tg7.pdf, слайды 8, 40-41.
// На каждую дугу (i,j) ориентированного графа выставляем:
//   - пропускную способность c(i,j) — положительное число;
//   - стоимость передачи единицы потока cost(i,j) — положительное число.
// На местах, где дуги нет, ставим 0 (это удобно: 0-пропускная способность
// означает, что дуга и в остаточной сети будет недоступна).
// =============================================================================

void Graph::generateCapacityAndCostMatrices(const PascalDistribution& distribution) {
    capacityMatrix.resize(vertexCount);
    costMatrix.resize(vertexCount);

    for (int i = 0; i < vertexCount; i++) {
        for (int j = 0; j < vertexCount; j++) {
            capacityMatrix.at(i, j) = 0;
            costMatrix.at(i, j)     = 0;
        }
    }

    // Специальный случай для графа из 2 вершин:
    // фиксируем пропускную способность дуги (1,2) равной 1,
    // стоимость — большое случайное число, чтобы продемонстрировать,
    // что величина потока [2/3 * max] = [2/3 * 1] = 0 и стоимость = 0.
    if (vertexCount == 2 && orientedMatrix.at(0, 1) == 1) {
        capacityMatrix.at(0, 1) = 1;
        costMatrix.at(0, 1)     = distribution.generate() + 10; // условно "большая"
        return;
    }

    // Общий случай:
    for (int i = 0; i < vertexCount; i++) {
        for (int j = 0; j < vertexCount; j++) {
            if (orientedMatrix.at(i, j) == 1) {
                capacityMatrix.at(i, j) = distribution.generate() + 1;
                costMatrix.at(i, j)     = distribution.generate() + 1;
            }
        }
    }
}

const Matrix& Graph::getCapacityMatrix() const { return capacityMatrix; }
const Matrix& Graph::getCostMatrix()     const { return costMatrix; }

int Graph::getSource() const { return 0; }                  // источник = вершина 1
int Graph::getSink()   const { return vertexCount - 1; }    // сток     = вершина N

// =============================================================================
//                    ОБЩАЯ ФУНКЦИЯ ПЕЧАТИ МАТРИЦЫ
// =============================================================================

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

// =============================================================================
//                       АЛГОРИТМ ФОРДА-ФАЛКЕРСОНА
//
// Источник: tg7.pdf, слайды 27, 31-33 (алгоритм расстановки пометок).
//
// Идея:
//   1. F[u][v] := 0 для всех (u,v)                        — начальный поток нулевой.
//   2. Пока в ОСТАТОЧНОЙ сети есть путь из s в t:
//        - найти этот путь (любым поиском, мы возьмём BFS — это даст нам
//          вариант Эдмондса-Карпа со слайда 39, O(V*E^2));
//        - найти минимальную остаточную пропускную способность δ вдоль пути;
//        - "пустить" δ единиц потока по этому пути:
//             для прямой дуги (u,v):  F[u][v] += δ
//             для обратной дуги (u,v): F[v][u] -= δ
//   3. Когда пути больше нет — текущий поток максимальный (по теореме ФФ).
//
// Остаточная пропускная способность дуги (u,v):
//   - если в исходной сети есть дуга u→v с c(u,v), то можно пустить
//     ещё c(u,v) - F[u][v] единиц "вперёд";
//   - если в исходной сети есть дуга v→u и по ней уже течёт F[v][u] > 0,
//     то можно "отменить" этот поток, т.е. в обратном направлении доступно F[v][u].
// =============================================================================

bool Graph::bfsAugmentingPath(const Matrix& capacity, const Matrix& flow,
                              int s, int t, std::vector<int>& parent,
                              long long& iterations) const {
    parent.assign(vertexCount, -1);
    parent[s] = s;                  // s помечаем сам собой, чтобы отличить от -1

    std::queue<int> q;
    q.push(s);

    while (!q.empty()) {
        iterations++;
        int u = q.front(); q.pop();

        for (int v = 0; v < vertexCount; v++) {
            if (parent[v] != -1) continue;             // вершина уже посещена

            // остаточная пропускная способность по направлению u → v:
            //   прямой ход: capacity[u][v] - flow[u][v]
            //   обратный  : flow[v][u]      (если по дуге v→u уже течёт поток,
            //                                 мы можем его "отменить", пройдя u→v)
            int residual = (capacity.at(u, v) - flow.at(u, v)) + flow.at(v, u);

            if (residual > 0) {
                parent[v] = u;
                if (v == t) return true;                // нашли путь до стока
                q.push(v);
            }
        }
    }
    return false;
}

int Graph::fordFulkersonMaxFlow(Matrix& flowOut, long long& iterations) const {
    iterations = 0;
    flowOut.resize(vertexCount);
    for (int i = 0; i < vertexCount; i++)
        for (int j = 0; j < vertexCount; j++)
            flowOut.at(i, j) = 0;

    if (vertexCount < 2) return 0;

    int s = getSource();
    int t = getSink();
    int maxFlow = 0;
    std::vector<int> parent;

    // Главный цикл: пока в остаточной сети есть путь s→t.
    while (bfsAugmentingPath(capacityMatrix, flowOut, s, t, parent, iterations)) {

        // 1) Найдём δ — узкое место (bottleneck) на найденном пути.
        int delta = INF;
        for (int v = t; v != s; v = parent[v]) {
            int u = parent[v];
            int residual = (capacityMatrix.at(u, v) - flowOut.at(u, v))
                         + flowOut.at(v, u);
            if (residual < delta) delta = residual;
        }

        // 2) Пустим δ единиц потока вдоль пути.
        //    Здесь самое важное в Форде-Фалкерсоне: если по дуге v→u в исходной
        //    сети уже течёт поток flow[v][u], то "движение вперёд" по u→v в
        //    остаточной сети — это на самом деле УМЕНЬШЕНИЕ flow[v][u].
        //    Это и есть тот самый "обратный шаг" из псевдокода со слайда 32.
        for (int v = t; v != s; v = parent[v]) {
            int u = parent[v];
            if (capacityMatrix.at(u, v) > 0 &&
                flowOut.at(u, v) < capacityMatrix.at(u, v)) {
                // есть незанятая часть прямой дуги — увеличиваем по ней
                int direct = capacityMatrix.at(u, v) - flowOut.at(u, v);
                int add    = std::min(delta, direct);
                flowOut.at(u, v) += add;
                // если δ больше, чем direct, остаток "съедим" из обратной дуги
                int rest = delta - add;
                if (rest > 0) flowOut.at(v, u) -= rest;
            } else {
                // прямая дуга насыщена (или её вовсе нет) — пользуемся обратной
                flowOut.at(v, u) -= delta;
            }
        }

        maxFlow += delta;
    }
    return maxFlow;
}

void Graph::printMaxFlowResult() const {
    if (vertexCount == 0) {
        std::cout << "Сначала необходимо сгенерировать граф.\n";
        return;
    }
    if (capacityMatrix.size() == 0) {
        std::cout << "Сначала необходимо сгенерировать матрицы.\n";
        return;
    }

    this->printCapacityMatrix();

    Matrix flow;
    long long iterations = 0;
    int maxFlow = fordFulkersonMaxFlow(flow, iterations);

    this->printFlowMatrix(flow, "Матрица потока (Форд-Фалкерсон):");

    std::cout << "\nИсточник: вершина " << getSource() + 1
              << ", сток: вершина "    << getSink()   + 1 << "\n";
    std::cout << "Величина максимального потока: " << maxFlow << "\n";
    std::cout << "Количество итераций (узлов, извлечённых из очереди BFS): "
              << iterations << "\n";
}