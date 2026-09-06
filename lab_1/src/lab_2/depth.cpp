#include "../header/general.h"
#include <iostream>
#include <stack>


std::vector<int> Graph::dfsTraversal(bool useOriented, int start,
                                     long long& iterations) const {
    std::vector<int> order; 
    iterations = 0;

    if (vertexCount == 0) return order;
    if (start < 0 || start >= vertexCount) return order;

    const Matrix& matrix = useOriented ? orientedMatrix : undirectedMatrix;

    std::vector<bool> x(vertexCount, false);

    std::stack<int> T;

    T.push(start);
    x[start] = true;

    while (!T.empty()) {
        iterations++;

        int u = T.top();
        T.pop();

        order.push_back(u);

        for (int w = vertexCount - 1; w >= 0; w--) {
            if (matrix.at(u, w) == 1 && !x[w]) {
                T.push(w);
                x[w] = true;
            }
        }
    }

    for (int v = 0; v < vertexCount; v++) {
        if (!x[v]) {
            T.push(v);
            x[v] = true;
            while (!T.empty()) {
                iterations++;
                int u = T.top();
                T.pop();
                order.push_back(u);
                for (int w = vertexCount - 1; w >= 0; w--) {
                    if (matrix.at(u, w) == 1 && !x[w]) {
                        T.push(w);
                        x[w] = true;
                    }
                }
            }
        }
    }

    return order;
}

void Graph::printDfsTraversal(bool useOriented, int start) const {
    if (start < 0 || start >= vertexCount) {
        std::cout << "Некорректная стартовая вершина.\n";
        return;
    }

    if (useOriented) {
        this->printOriented();
        std::cout << "\nОбход вершин ориентированного графа поиском в глубину:\n";
    } else {
        this->printUndirected();
        std::cout << "\nОбход вершин неориентированного графа поиском в глубину:\n";
    }

    long long iterations = 0;
    std::vector<int> order = dfsTraversal(useOriented, start, iterations);

    for (size_t i = 0; i + 1 < order.size(); i++) {
        std::cout << order[i] + 1 << " -> " << order[i + 1] + 1 << "\n";
    }

    std::cout << "Количество итераций: " << iterations << "\n";
}