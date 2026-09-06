#include "../header/general.h"
#include <iostream>
#include <vector>
#include <cmath>


long long Graph::countSpanningTreesKirchhoff() const {

    if (vertexCount == 0) return 0;
    if (vertexCount == 1) return 1;

    if (vertexCount == 2) {
        return undirectedMatrix.at(0, 1) == 1 ? 1 : 0;
    }

    int n = vertexCount;
    std::vector<std::vector<double>> B(n, std::vector<double>(n, 0.0));

    for (int i = 0; i < n; i++) {
        int deg = 0;
        for (int j = 0; j < n; j++) {
            if (i == j) continue;
            if (undirectedMatrix.at(i, j) == 1) {
                B[i][j] = -1.0;
                deg++;
            }
        }
        B[i][i] = (double)deg;
    }

    int m = n - 1;
    std::vector<std::vector<double>> M(m, std::vector<double>(m, 0.0));
    for (int i = 0; i < m; i++)
        for (int j = 0; j < m; j++)
            M[i][j] = B[i + 1][j + 1];

    double det = 1.0;
    for (int col = 0; col < m; col++) {
        int pivot = col;
        for (int row = col + 1; row < m; row++) {
            if (std::fabs(M[row][col]) > std::fabs(M[pivot][col])) pivot = row;
        }
        if (std::fabs(M[pivot][col]) < 1e-12) {
            return 0;
        }
        if (pivot != col) {
            std::swap(M[pivot], M[col]);
            det = -det;
        }

        det *= M[col][col];
        for (int row = col + 1; row < m; row++) {
            double factor = M[row][col] / M[col][col];
            for (int k = col; k < m; k++) {
                M[row][k] -= factor * M[col][k];
            }
        }
    }

    return (long long)std::llround(det);
}

void Graph::printKirchhoffResult() const {
    if (vertexCount == 0) {
        std::cout << "Сначала необходимо сгенерировать граф.\n";
        return;
    }
    this->printUndirected();

    long long count = countSpanningTreesKirchhoff();

    std::cout << "\nЧисло остовных деревьев (теорема Кирхгофа): " << count << "\n";

    if (count == 0) {
        std::cout << "(граф несвязный — остовного дерева не существует)\n";
    } else if (vertexCount == 2) {
        std::cout << "(K2: единственное остовное дерево — это само ребро)\n";
    } else if (vertexCount == 1) {
        std::cout << "(одна вершина — тривиальный случай)\n";
    }
}