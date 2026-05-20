#ifndef GRAPH_H
#define GRAPH_H

#include <vector>
#include <string>
#include <queue>
#include "distribution.h"

const int INF = 1000000000;


class Matrix {
private:
    std::vector<std::vector<int>> data;

public:
    Matrix();
    Matrix(int size);

    void resize(int size);
    int size() const;

    int& at(int i, int j);
    int at(int i, int j) const;

    const std::vector<std::vector<int>>& getData() const;

    void print(const std::string& title) const;
};

struct BellmanFordResult {
    std::vector<int> distances;   // вектор кратчайших расстояний от источника
    std::vector<int> parent;      // массив предков для восстановления пути
    bool hasNegativeCycle;        // найден ли достижимый отрицательный цикл
    long long iterations;
};


class Graph {
private:
    int vertexCount; // количество вершин
    Matrix orientedMatrix; // ориентированный граф
    Matrix undirectedMatrix; // неориентированный граф
    Matrix weightMatrix; // весовая матрица
// ---- 3 лабораторная -----
    Matrix capacityMatrix;   // пропускные способности
    Matrix costMatrix;       // стоимости передачи единицы потока по дуге

public:
    Graph();
    Graph(int n);

    void setVertexCount(int n);
    int getVertexCount() const;

    void generateOrientedAcyclic(const PascalDistribution& distribution);
    void generateOrientedAcyclicFromDegrees(const std::vector<int>& outDegrees);
    void buildUndirectedFromOriented();

    std::vector<int> calculateOutDegrees() const;
    std::vector<int> calculateUndirectedDegrees() const;

    void printOriented() const;
    void printUndirected() const;

    const Matrix& getOrientedMatrix() const;
    const Matrix& getUndirectedMatrix() const;
    //--------------------------------------
    //          часть 2
    //--------------------------------------
    std::vector<int> bfsDistances(int start, bool useOriented) const;

    std::vector<int> findEccentricities(bool useOriented) const;
    int findRadius(bool useOriented) const;
    int findDiameter(bool useOriented) const;
    std::vector<int> findCenter(bool useOriented) const;
    std::vector<int> findDiametralVertices(bool useOriented) const;

    void printGraphCharacteristics(bool useOriented) const;
    //--------------------------------------
    //          часть 3
    //--------------------------------------
    void generateWeightMatrix(const PascalDistribution& distribution, int mode);
    const Matrix& getWeightMatrix() const;
    void printWeightMatrix() const;

    Matrix shimbellStepMin(const Matrix& left, const Matrix& right) const;
    Matrix shimbellStepMax(const Matrix& left, const Matrix& right) const;

    Matrix shimbellMin(int edgesCount) const;
    Matrix shimbellMax(int edgesCount) const;

    void printShimbellResult(int edgesCount, bool findMin, bool findMax) const;
    int findMaxPathLengthOriented() const;
    //--------------------------------------
    //          часть 4
    //--------------------------------------
    bool routeExistsOriented(int start, int finish) const;
    long long countRoutesOriented(int start, int finish) const;
    void printRouteInfoOriented(int start, int finish) const;

    //--------------------------------------
    //          лабораторная 2
    //--------------------------------------
    /*void dfsEdgesUtil(bool useOriented, int v, std::vector<bool>& visited, std::vector<std::vector<bool>>& visitedEdges, int& iterations) const;

    void dfsEdges(bool useOriented, int start) const;
    void printDfsTraversal(bool useOriented, int start) const;

    BellmanFordResult bellmanFord(int start) const;
    std::vector<int> restoreBellmanFordPath(int start, int finish, const std::vector<int>& parent) const;
    void printBellmanFordResult(int start, int finish) const;
    */

    //--------------------------------------
    //          лабораторная 2
    //--------------------------------------
    // Поиск в глубину по псевдокоду со слайда 19 tg3.pdf
    // (универсальный обход через структуру T; для DFS T — это стек, LIFO).
    // Возвращает последовательность пройденных вершин (то, что псевдокод выдаёт через yield).
    std::vector<int> dfsTraversal(bool useOriented, int start, long long& iterations) const;
    void printDfsTraversal(bool useOriented, int start) const;

    // Алгоритм Беллмана-Форда (tg5.pdf, слайды 53–59).
    // Возвращает вектор расстояний, массив предков, флаг отрицательного цикла
    // и число итераций релаксации (для сравнения с DFS).
    BellmanFordResult bellmanFord(int start) const;
    std::vector<int> restoreBellmanFordPath(int start, int finish,
                                            const std::vector<int>& parent) const;
    void printBellmanFordResult(int start, int finish) const;

    //--------------------------------------
    //          лабораторная 3
    //--------------------------------------
    // Генерация двух матриц: пропускных способностей и стоимостей.
    // На месте дуг исходного ориентированного графа ставим случайные
    // положительные числа, в остальных клетках — 0 (дуги нет).
    void generateCapacityAndCostMatrices(const PascalDistribution& distribution);

    const Matrix& getCapacityMatrix() const;
    const Matrix& getCostMatrix() const;
    void printCapacityMatrix() const;
    void printCostMatrix() const;
    void printFlowMatrix(const Matrix& flow, const std::string& title) const;

    // Источник = первая вершина, сток = последняя.
    int getSource() const;
    int getSink()   const;

    // Алгоритм Форда-Фалкерсона. Возвращает величину максимального потока
    // и через out-параметр — матрицу потока F[u][v].
    int fordFulkersonMaxFlow(Matrix& flowOut, long long& iterations) const;

    // Поиск увеличивающего пути в остаточной сети поиском в ширину.
    // Возвращает true и заполняет parent[], если путь найден.
    bool bfsAugmentingPath(const Matrix& capacity, const Matrix& flow,
                           int s, int t, std::vector<int>& parent,
                           long long& iterations) const;

    // Поток заданной величины targetValue с минимальной стоимостью.
    // Возвращает фактически проведённую величину потока (может быть < target,
    // если сеть не пропускает столько) и стоимость через costOut.
    int minCostFlow(int targetValue, Matrix& flowOut,
                    long long& costOut, long long& iterations) const;

    void printMaxFlowResult() const;
    void printMinCostFlowResult() const;
};

#endif