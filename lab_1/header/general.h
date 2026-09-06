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
    
    std::vector<int> dfsTraversal(bool useOriented, int start, long long& iterations) const;
    void printDfsTraversal(bool useOriented, int start) const;

    BellmanFordResult bellmanFord(int start) const;
    std::vector<int> restoreBellmanFordPath(int start, int finish,
                                            const std::vector<int>& parent) const;
    void printBellmanFordResult(int start, int finish) const;

    //--------------------------------------
    //          лабораторная 3
    //--------------------------------------
    void generateCapacityAndCostMatrices(const PascalDistribution& distribution);

    const Matrix& getCapacityMatrix() const;
    const Matrix& getCostMatrix() const;
    void printCapacityMatrix() const;
    void printCostMatrix() const;
    void printFlowMatrix(const Matrix& flow, const std::string& title) const;

    std::vector<int> findSources() const;
    std::vector<int> findSinks()   const;

    struct AugmentingPath {
        std::vector<int> vertices;
        int delta;
    };

    int fordFulkersonMaxFlow(int source, int sink,
                             Matrix& flowOut,
                             std::vector<AugmentingPath>& augmentingPaths,
                             long long& iterations) const;

    bool bfsAugmentingPath(const Matrix& capacity, const Matrix& flow,
                           int s, int t, std::vector<int>& parent,
                           long long& iterations) const;

    int minCostFlow(int source, int sink, int targetValue,
                    Matrix& flowOut,
                    long long& costOut, long long& iterations) const;

    void printMaxFlowResult(int source, int sink) const;
    void printMinCostFlowResult(int source, int sink) const;

    //--------------------------------------
    //          лабораторная 4
    //--------------------------------------
    long long countSpanningTreesKirchhoff() const;
    void printKirchhoffResult() const;

    struct MstEdge { int u; int v; int weight; };
    std::vector<MstEdge> kruskalMST(long long& totalWeight, bool& isConnected) const;
    void printKruskalResult() const;

    void encodePrufer(const std::vector<MstEdge>& mstEdges,
                      std::vector<int>& codeVertices,
                      std::vector<int>& codeWeights) const;


    std::vector<MstEdge> decodePrufer(const std::vector<int>& codeVertices,
                                      const std::vector<int>& codeWeights) const;

    void printPruferResult() const;

    std::vector<int> minVertexCover(bool useSpanning) const;
    void printVertexCoverResult(bool useSpanning) const;
};

#endif