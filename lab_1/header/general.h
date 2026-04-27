#ifndef GRAPH_H
#define GRAPH_H

#include <vector>
#include <string>
#include <queue>
#include "distribution.h"

const int INF = 1000000000;


class Matrix {
private:
    std::vector<std::vector<int>> data; // сама матрица

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
};


class Graph {
private:
    int vertexCount; // количество вершин
    Matrix orientedMatrix; // ориентированный граф
    Matrix undirectedMatrix; // неориентированный граф
    Matrix weightMatrix; // весовая матрица

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
    void dfsEdgesUtil(bool useOriented, int v, std::vector<bool>& visited, std::vector<std::vector<bool>>& visitedEdges, int& iterations) const;

    void dfsEdges(bool useOriented, int start) const;
    void printDfsTraversal(bool useOriented, int start) const;

    BellmanFordResult bellmanFord(int start) const;
    std::vector<int> restoreBellmanFordPath(int start, int finish, const std::vector<int>& parent) const;
    void printBellmanFordResult(int start, int finish) const;

};

#endif