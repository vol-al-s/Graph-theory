#include "../header/general.h"
#include <iostream>
#include <queue>
#include <limits>
#include <cstdlib>
#include <iomanip>
#include <functional>
#include <algorithm>
#include <random>

Matrix::Matrix() {}

Matrix::Matrix(int size) {
    resize(size);
}

void Matrix::resize(int size) { // делает матрицу указанного размера с нулями
    data.assign(size, std::vector<int>(size, 0));
}

int Matrix::size() const { // возвращает число вершин
    return (int)data.size();
}

int& Matrix::at(int i, int j) { // изменяем значение
    return data[i][j];
}

int Matrix::at(int i, int j) const { // читаем значение
    return data[i][j];
}

const std::vector<std::vector<int>>& Matrix::getData() const { // возвращаем матрицу целиком
    return data;
}

void Matrix::print(const std::string& title) const { // Выводим значение матрицы
    int n = size();

    std::cout << "\n" << title << "\n";
    std::cout << "\n";
        for (int j = 0; j < (*this).size(); j++) {
        if (j == 0) {
            std::cout << std::setw(6) << "| " << std::setw(1) << "v" <<j+1;
        } else {
            std::cout << std::setw(2) << "v" <<j+1;
        }
    }
    std::cout<<std::endl;
    for (int k = 0; k < (6+(*this).size() * (2+1)); k++) {
        std::cout << "-";
    }

    std::cout << "\n";
    for (int i = 0; i < n; i++) {
        std::cout << std::setw(2) << "v" << i+1 << " |";
        for (int j = 0; j < n; j++) {
            std::cout << std::setw(3) << data[i][j];
        }
        std::cout << "\n";
    }
    std::cout << std::endl;
}

Graph::Graph() : vertexCount(0) {} // конструктор (изначально число вершин 0)

//параметризированный конструктор
Graph::Graph(int n) : vertexCount(n), orientedMatrix(n), undirectedMatrix(n) {}

void Graph::setVertexCount(int n) { // задаем число вершин
    vertexCount = n;
    orientedMatrix.resize(n); 
    undirectedMatrix.resize(n); 
    weightMatrix.resize(n);
}

int Graph::getVertexCount() const { // возвращаем количество вершин
    return vertexCount;
}

//создаем ориентированный граф
void Graph::generateOrientedAcyclic(const PascalDistribution& distribution) {
    std::vector<int> outDegrees = distribution.generateOutDegreeSequence(vertexCount);

    generateOrientedAcyclicFromDegrees(outDegrees);
}

void Graph::generateOrientedAcyclicFromDegrees(const std::vector<int>& outDegrees) {
    orientedMatrix.resize(vertexCount);

    if (vertexCount <= 1) {
        return;
    }

    for (int i = 0; i < vertexCount; i++) {
        for (int j = 0; j < vertexCount; j++) {
            orientedMatrix.at(i, j) = 0;
        }
    }

    std::vector<int> remaining = outDegrees;

    for (int j = 1; j < vertexCount; j++) {
        int parent = rand() % j;

        orientedMatrix.at(parent, j) = 1;

        if (remaining[parent] > 0) {
            remaining[parent]--;
        }
    }

    for (int i = 0; i < vertexCount; i++) {
        int freePositions = 0;
        for (int j = i + 1; j < vertexCount; j++) {
            if (orientedMatrix.at(i, j) == 0) {
                freePositions++;
            }
        }

        if (freePositions == 0 || remaining[i] <= 0) {
            continue;
        }

        if (remaining[i] > freePositions) {
            remaining[i] = freePositions;
        }

        int added = 0;

        while (added < remaining[i]) {
            int j = i + 1 + rand() % (vertexCount - i - 1);

            if (orientedMatrix.at(i, j) == 0) {
                orientedMatrix.at(i, j) = 1;
                added++;
            }
        }
    }
}

void Graph::buildUndirectedFromOriented() {
    undirectedMatrix.resize(vertexCount);

    for (int i = 0; i < vertexCount; i++) {
        for (int j = 0; j < vertexCount; j++) {
            if (orientedMatrix.at(i, j) == 1 || orientedMatrix.at(j, i) == 1) {
                undirectedMatrix.at(i, j) = 1;
                undirectedMatrix.at(j, i) = 1;
            }
        }
    }
}

//считаем исходящие степени
std::vector<int> Graph::calculateOutDegrees() const {
    std::vector<int> degrees(vertexCount, 0);

    for (int i = 0; i < vertexCount; i++) {
        for (int j = 0; j < vertexCount; j++) {
            degrees[i] += orientedMatrix.at(i, j); 
        }
    }

    return degrees;
}
//для неориентированного графа
std::vector<int> Graph::calculateUndirectedDegrees() const {
    std::vector<int> degrees(vertexCount, 0);

    for (int i = 0; i < vertexCount; i++) {
        for (int j = 0; j < vertexCount; j++) {
            degrees[i] += undirectedMatrix.at(i, j);
        }
    }

    return degrees;
}

void Graph::printOriented() const {
    orientedMatrix.print("Матрица смежности ориентированного графа:");

    /*std::vector<int> degrees = calculateOutDegrees();
    std::cout << "\nИсходящие степени вершин ориентированного графа:\n";
    for (int i = 0; i < vertexCount; i++) {
        std::cout << "Вершина " << i+1 << ": " << degrees[i] << "\n";
    }*/
}

void Graph::printUndirected() const {
    undirectedMatrix.print("Матрица смежности неориентированного графа:");

    std::vector<int> degrees = calculateUndirectedDegrees();
    /*std::cout << "\nСтепени вершин неориентированного графа:\n";
    for (int i = 0; i < vertexCount; i++) {
        std::cout << "Вершина " << i+1 << ": " << degrees[i] << "\n";
    }*/
}

const Matrix& Graph::getOrientedMatrix() const {
    return orientedMatrix;
}

const Matrix& Graph::getUndirectedMatrix() const {
    return undirectedMatrix;
}



