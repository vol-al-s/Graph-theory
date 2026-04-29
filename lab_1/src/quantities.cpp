#include "../header/general.h"
#include <iostream>
#include <queue>
#include <limits>
#include <cstdlib>
#include <iomanip>
#include <functional>


    //--------------------------------------
    //          часть 2
    //--------------------------------------

// поиск в глубину 
std::vector<int> Graph::bfsDistances(int start, bool useOriented) const {
    std::vector<int> dist(vertexCount, -1);

    if (start < 0 || start >= vertexCount) {
        return dist;
    }

    const Matrix& matrix = useOriented ? orientedMatrix : undirectedMatrix;

    std::queue<int> q;
    dist[start] = 0;
    q.push(start);

    while (!q.empty()) {
        int v = q.front();
        q.pop();

        for (int u = 0; u < vertexCount; u++) {
            if (matrix.at(v, u) == 1 && dist[u] == -1) {
                dist[u] = dist[v] + 1;
                q.push(u);
            }
        }
    }

    return dist;
}

//эксцентриситет
std::vector<int> Graph::findEccentricities(bool useOriented) const {
    std::vector<int> eccentricities(vertexCount, 0);

    for (int v = 0; v < vertexCount; v++) {
        std::vector<int> dist = bfsDistances(v, useOriented);

        int maxDistance = 0;

        for (int u = 0; u < vertexCount; u++) {
            if (dist[u] == -1) {
                continue;
            }
            if (dist[u] > maxDistance) {
                maxDistance = dist[u];
            }
        }
        eccentricities[v] = maxDistance;
    }

    return eccentricities;
}

//радиус
int Graph::findRadius(bool useOriented) const {
    std::vector<int> eccentricities = findEccentricities(useOriented);

    int radius = INF;

    for (int e : eccentricities) {
        if (e < radius) {
            radius = e;
        }
    }

    return radius;
}

//диаметр
int Graph::findDiameter(bool useOriented) const {
    std::vector<int> eccentricities = findEccentricities(useOriented);

    int diameter = 0;

    for (int e : eccentricities) {
        if (e != INF && e > diameter) {
            diameter = e;
        }
    }

    return diameter;
}


//центр
std::vector<int> Graph::findCenter(bool useOriented) const {
    std::vector<int> eccentricities = findEccentricities(useOriented);
    int radius = findRadius(useOriented);

    std::vector<int> center;

    for (int v = 0; v < vertexCount; v++) {
        if (eccentricities[v] == radius) {
            center.push_back(v);
        }
    }
    return center;
}

//диаметр вершины
std::vector<int> Graph::findDiametralVertices(bool useOriented) const {
    std::vector<int> eccentricities = findEccentricities(useOriented);
    int diameter = findDiameter(useOriented);

    std::vector<int> diametralVertices;

    for (int v = 0; v < vertexCount; v++) {
        if (eccentricities[v] == diameter) {
            diametralVertices.push_back(v);
        }
    }

    return diametralVertices;
}

void Graph::printGraphCharacteristics(bool useOriented) const {
    std::vector<int> eccentricities = findEccentricities(useOriented);
    int radius = findRadius(useOriented);
    int diameter = findDiameter(useOriented);
    std::vector<int> center = findCenter(useOriented);
    std::vector<int> diametralVertices = findDiametralVertices(useOriented);

    if (useOriented) {
        std::cout << "\nХарактеристики ориентированного графа:\n";
    } else {
        std::cout << "\nХарактеристики неориентированного графа:\n";
    }

    std::cout << "\nЭксцентриситеты вершин:\n";
    for (int v = 0; v < vertexCount; v++) {
        std::cout << "Вершина " << v + 1 << ": ";
        if (eccentricities[v] == INF) {
            std::cout << "INF";
        } else {
            std::cout << eccentricities[v];
        }
        std::cout << "\n";
    }

    std::cout << "\nРадиус графа: ";
    if (radius == INF) {
        std::cout << "INF\n";
    } else {
        std::cout << radius << "\n";
    }

    std::cout << "Центр графа: ";
    
    if (center.size() == 1) {
        std::cout << "вершина ";
    } else if (center.size() > 1) {
        std::cout << "вершины ";
    }
    for (int v : center) {
            std::cout << v + 1 << " ";
    }
    std::cout << "\n";

    std::cout << "Диаметр графа: " << diameter << "\n";

    std::cout << "Диаметральные вершины: ";
    for (int v : diametralVertices) {
        std::cout << v + 1 << " ";
    }
    std::cout << "\n";
}