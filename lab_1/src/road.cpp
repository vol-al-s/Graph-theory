
#include "../header/general.h"
#include <iostream>
#include <queue>
#include <limits>
#include <cstdlib>
#include <iomanip>
#include <functional>
 
    //--------------------------------------
    //          часть 4
    //--------------------------------------




//находим пути
bool Graph::routeExistsOriented(int start, int finish) const {
    if (start < 0 || start >= vertexCount || finish < 0 || finish >= vertexCount) {
        return false;
    }

    std::vector<bool> visited(vertexCount, false);

    std::function<bool(int)> dfs = [&](int v) -> bool {
        if (v == finish) {
            return true;
        }

        visited[v] = true;

        for (int u = 0; u < vertexCount; u++) {
            if (orientedMatrix.at(v, u) == 1 && !visited[u]) {
                if (dfs(u)) {
                    return true;
                }
            }
        }

        return false;
    };

    return dfs(start);
}

//считаем пути
long long Graph::countRoutesOriented(int start, int finish) const {
    if (start < 0 || start >= vertexCount || finish < 0 || finish >= vertexCount) {
        return 0;
    }

    std::vector<long long> memo(vertexCount, -1);

    std::function<long long(int)> dfsCount = [&](int v) -> long long {
        if (v == finish) {
            return 1;
        }

        if (memo[v] != -1) {
            return memo[v];
        }

        long long total = 0;

        for (int u = 0; u < vertexCount; u++) {
            if (orientedMatrix.at(v, u) == 1) {
                total += dfsCount(u);
            }
        }

        memo[v] = total;
        return total;
    };

    return dfsCount(start);
}

void Graph::printRouteInfoOriented(int start, int finish) const {
    if (start < 0 || start >= vertexCount || finish < 0 || finish >= vertexCount) {
        std::cout << "Некорректные номера вершин.\n";
        return;
    }

    (*this).printOriented();

    bool exists = routeExistsOriented(start, finish);
    long long routesCount = countRoutesOriented(start, finish);

    std::cout << "\nМаршрут из вершины " << start + 1
              << " в вершину " << finish + 1 << ":\n";

    if (exists) {
        std::cout << "Маршрут существует.\n";
    } else {
        std::cout << "Маршрут не существует.\n";
    }

    std::cout << "Количество маршрутов: " << routesCount << "\n";
}