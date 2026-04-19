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

std::vector<int> Graph::bfsDistancesUndirected(int start) const {
    // массив расстояний от стартовой точки до все остальных
    // Значение -1 означает, что вершина ещё не посчитана
    std::vector<int> dist(vertexCount, -1);

    
    std::queue<int> q; // создается очередь для поиска

    
    dist[start] = 0; // сама к себе = 0
    q.push(start); // добавляем в очередь начальную вершину

    // Пока еще остались вершины
    while (!q.empty()) {
        int v = q.front(); // запоминаем первую вершину в очереди
        q.pop();// сразу удаляем

        for (int u = 0; u < vertexCount; u++) {
            // Если есть ребро v-u и вершина u ещё не посещена
            if (undirectedMatrix.at(v, u) == 1 && dist[u] == -1) {
                // Тогда расстояние до u равно расстоянию до v + 1
                dist[u] = dist[v] + 1;

                q.push(u); //становится радителем
            }
        }
    }

    return dist;
}

//эксцентриситет
std::vector<int> Graph::findEccentricitiesUndirected() const {
    // Здесь будем хранить эксцентриситет каждой вершины
    std::vector<int> eccentricities(vertexCount, 0);

    // Для каждой вершины запускаем BFS
    for (int v = 0; v < vertexCount; v++) {
        std::vector<int> dist = bfsDistancesUndirected(v);

        // Эксцентриситет = максимальное расстояние от вершины v до всех остальных
        int maxDistance = 0;

        for (int u = 0; u < vertexCount; u++) {
            if (dist[u] > maxDistance) {
                maxDistance = dist[u];
            }
        }

        eccentricities[v] = maxDistance;
    }

    return eccentricities;
}

//радиус
int Graph::findRadiusUndirected() const {
    // Радиус графа — минимальный из эксцентриситетов
    std::vector<int> eccentricities = findEccentricitiesUndirected();

    int radius = eccentricities[0];

    for (int i = 1; i < vertexCount; i++) {
        if (eccentricities[i] < radius) {
            radius = eccentricities[i];
        }
    }

    return radius;
}

//диаметр
int Graph::findDiameterUndirected() const {
    // Диаметр графа — максимальный из эксцентриситетов
    std::vector<int> eccentricities = findEccentricitiesUndirected();

    int diameter = eccentricities[0];

    for (int i = 1; i < vertexCount; i++) {
        if (eccentricities[i] > diameter) {
            diameter = eccentricities[i];
        }
    }

    return diameter;
}


//центр
std::vector<int> Graph::findCenterUndirected() const {
    // Центр — это вершины, эксцентриситет которых равен радиусу
    std::vector<int> eccentricities = findEccentricitiesUndirected();
    int radius = findRadiusUndirected();

    std::vector<int> center;

    for (int v = 0; v < vertexCount; v++) {
        if (eccentricities[v] == radius) {
            center.push_back(v+1);
        }
    }

    return center;
}

//диаметр вершины
std::vector<int> Graph::findDiametralVerticesUndirected() const {
    // Диаметральные вершины — те, чей эксцентриситет равен диаметру
    std::vector<int> eccentricities = findEccentricitiesUndirected();
    int diameter = findDiameterUndirected();

    std::vector<int> diametralVertices;

    for (int v = 0; v < vertexCount; v++) {
        if (eccentricities[v] == diameter) {
            diametralVertices.push_back(v+1);
        }
    }

    return diametralVertices;
}

void Graph::printGraphCharacteristicsUndirected() const {
    std::vector<int> eccentricities = findEccentricitiesUndirected();
    int radius = findRadiusUndirected();
    int diameter = findDiameterUndirected();
    std::vector<int> center = findCenterUndirected();
    std::vector<int> diametralVertices = findDiametralVerticesUndirected();

    std::cout << "\nЭксцентриситеты вершин:\n";
    for (int v = 0; v < vertexCount; v++) {
        std::cout << "Вершина " << v+1 << ": " << eccentricities[v] << "\n";
    }

    std::cout << "\nРадиус графа: " << radius << "\n";

    std::cout << "Центр графа: ";
    for (int v : center) {
        std::cout << v << " ";
    }
    std::cout << "\n";

    std::cout << "Диаметр графа: " << diameter << "\n";

    std::cout << "Диаметральные вершины: ";
    for (int v : diametralVertices) {
        std::cout << v << " ";
    }
    std::cout << "\n";
}