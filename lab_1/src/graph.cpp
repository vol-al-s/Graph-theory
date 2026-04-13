#include "../header/graph.h"
#include <iostream>
#include <queue>
#include <limits>
#include <cstdlib>
#include <iomanip>
#include <functional>

const int INF = 1000000000;

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

    std::cout << "\n" << title << "\n\n";
    std::cout << "\n";
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            std::cout << std::setw(3) << data[i][j];
        }
        std::cout << "\n";
    }
}

Graph::Graph() : vertexCount(0) {} // конструктор (изначально число вершин 0)

//параметризированный конструктор
Graph::Graph(int n) : vertexCount(n), orientedMatrix(n), undirectedMatrix(n) {}

void Graph::setVertexCount(int n) { // задаем число вершин
    vertexCount = n;
    orientedMatrix.resize(n); // переобозначаем размер матрицы
    undirectedMatrix.resize(n); // для этой тоже
    weightMatrix.resize(n);
}

int Graph::getVertexCount() const { // возвращаем количество вершин
    return vertexCount;
}

//создаем ориентированный граф
void Graph::generateOrientedAcyclic(const PascalDistribution& distribution) {
    orientedMatrix.resize(vertexCount); // пересоздаем матрицу по актуальному кол-ву вершин

    if (vertexCount <= 1) {
        return; // если <= 1, матрицу не строим
    }

    std::queue<int> q; // создаем очередь вершин, которые еще не использованы
    q.push(0); // первая вешина 0

    int nextVertex = 1; // следующая вершина будет 1


    // пока еще остались свободные вершины
    while (!q.empty() && nextVertex < vertexCount) {
        int parent = q.front(); // родителем будет первый элемент в очереди
        q.pop(); // как только зафиксировали родителя, сразу удаляем

        int childrenCount = distribution.generate(); // генерируем количество наследников

        //если очередь пуста, остались еще вершины, на генерация пустая
        if (q.empty() && nextVertex < vertexCount && childrenCount == 0) {
            childrenCount = 1; // строго задаем минимум одного наследника
        }

        int remaining = vertexCount - nextVertex; // сколько осталось вершин
        if (childrenCount > remaining) { // наследников не может быть больше, чем остаток
            childrenCount = remaining;
        }

        for (int i = 0; i < childrenCount; i++) {
            orientedMatrix.at(parent, nextVertex) = 1; //устанавливаем связь с родителем и наследником для матрицы смежности
            q.push(nextVertex); // наследник становится родителем
            nextVertex++; // указываем следующего наследника
        }
    }
    //если остались всободные вешины, то досоединяем
    int lastConnected = 0;
    for (int v = 1; v < nextVertex; v++) {
        lastConnected = v;
    }

    while (nextVertex < vertexCount) {
        orientedMatrix.at(lastConnected, nextVertex) = 1;
        lastConnected = nextVertex;
        nextVertex++;
    }
}

void Graph::buildUndirectedFromOriented() {
    undirectedMatrix.resize(vertexCount); // снова пересоздаем матрицу тогоже размера

    for (int i = 0; i < vertexCount; i++) {
        for (int j = 0; j < vertexCount; j++) {
            if (orientedMatrix.at(i, j) == 1 || orientedMatrix.at(j, i) == 1) {
                undirectedMatrix.at(i, j) = 1;
                undirectedMatrix.at(j, i) = 1; // просто зеркалим значения
            }
        }
    }
}

//считаем исходящие степени
std::vector<int> Graph::calculateOutDegrees() const {
    std::vector<int> degrees(vertexCount, 0); // массив сепеней

    for (int i = 0; i < vertexCount; i++) {
        for (int j = 0; j < vertexCount; j++) {
            degrees[i] += orientedMatrix.at(i, j); //сумма строки = степень вершины
        }
    }

    return degrees;
}
//для неориентированного графа
std::vector<int> Graph::calculateUndirectedDegrees() const {
    std::vector<int> degrees(vertexCount, 0); // массив степеней

    for (int i = 0; i < vertexCount; i++) {
        for (int j = 0; j < vertexCount; j++) {
            degrees[i] += undirectedMatrix.at(i, j); // также сумма = степень
        }
    }

    return degrees;
}

void Graph::printOriented() const {
    orientedMatrix.print("Матрица смежности ориентированного графа:");

    std::vector<int> degrees = calculateOutDegrees();
    std::cout << "\nИсходящие степени вершин ориентированного графа:\n";
    for (int i = 0; i < vertexCount; i++) {
        std::cout << "Вершина " << i+1 << ": " << degrees[i] << "\n";
    }
}

void Graph::printUndirected() const {
    undirectedMatrix.print("Матрица смежности неориентированного графа:");

    std::vector<int> degrees = calculateUndirectedDegrees();
    std::cout << "\nСтепени вершин неориентированного графа:\n";
    for (int i = 0; i < vertexCount; i++) {
        std::cout << "Вершина " << i+1 << ": " << degrees[i] << "\n";
    }
}

const Matrix& Graph::getOrientedMatrix() const {
    return orientedMatrix;
}

const Matrix& Graph::getUndirectedMatrix() const {
    return undirectedMatrix;
}

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

    //--------------------------------------
    //          часть 3
    //--------------------------------------


//строим матрицу весов
void Graph::generateWeightMatrix(const PascalDistribution& distribution, int mode) {
    weightMatrix.resize(vertexCount); //генерируем матрицу нужного размера 

    // Сначала считаем, что путей нигде нет.
    // Поэтому во все ячейки ставим INF.
    for (int i = 0; i < vertexCount; i++) {
        for (int j = 0; j < vertexCount; j++) {
            weightMatrix.at(i, j) = INF;
        }
    }

    // Генерируем веса только для существующих дуг ориентированного графа
    for (int i = 0; i < vertexCount; i++) {
        for (int j = 0; j < vertexCount; j++) {
            if (orientedMatrix.at(i, j) == 1) {
                // Получаем положительное число по распределению Паскаля
                int value = distribution.generate() + 1;

                
                // 0 - только положительные
                // 1 - только отрицательные
                // 2 - смешанные
                if (mode == 0) {
                    weightMatrix.at(i, j) = value;
                } else if (mode == 1) {
                    weightMatrix.at(i, j) = -value;
                } else if (mode == 2) {
                    if (std::rand() % 2 == 0) {
                        weightMatrix.at(i, j) = value;
                    } else {
                        weightMatrix.at(i, j) = -value;
                    }
                } else {
                    // Если режим ошибочный, по умолчанию делаем положительный вес
                    weightMatrix.at(i, j) = value;
                }
            }
        }
    }
}

//возвращение матрицы весов
const Matrix& Graph::getWeightMatrix() const {
    return weightMatrix;
}

//вывод матрицы весов
void Graph::printWeightMatrix() const {
    std::cout << "\nВесовая матрица:\n\n    ";
    for (int j = 0; j < vertexCount; j++) {
        std::cout << std::setw(6) << j+1;
    }
    std::cout << "\n";

    for (int i = 0; i < vertexCount; i++) {
        std::cout << std::setw(3) << "v" << i+1 << " ";
        for (int j = 0; j < vertexCount; j++) {
            if (weightMatrix.at(i, j) == INF) {
                std::cout << std::setw(6) << ".";
            } else {
                std::cout << std::setw(6) << weightMatrix.at(i, j);
            }
        }
        std::cout << "\n";
    }
}

//Один шаг для минимального пути
Matrix Graph::shimbellStepMin(const Matrix& left, const Matrix& right) const {
    // Результирующая матрица
    Matrix result(vertexCount);

    // Изначально считаем, что пути между любыми вершинами нет
    for (int i = 0; i < vertexCount; i++) {
        for (int j = 0; j < vertexCount; j++) {
            result.at(i, j) = INF;
        }
    }

    // Формула:
    // result[i][j] = min_k ( left[i][k] + right[k][j] )
    for (int i = 0; i < vertexCount; i++) {
        for (int j = 0; j < vertexCount; j++) {
            for (int k = 0; k < vertexCount; k++) {
                // Если одного из путей нет, такой вариант не рассматриваем
                if (left.at(i, k) == INF || right.at(k, j) == INF) {
                    continue;
                }

                int candidate = left.at(i, k) + right.at(k, j);

                if (candidate < result.at(i, j)) {
                    result.at(i, j) = candidate;
                }
            }
        }
    }

    return result;
}

Matrix Graph::shimbellStepMax(const Matrix& left, const Matrix& right) const {
    // Для максимального пути тоже используем INF
    Matrix result(vertexCount);

    for (int i = 0; i < vertexCount; i++) {
        for (int j = 0; j < vertexCount; j++) {
            result.at(i, j) = INF;
        }
    }

    // Формула:
    // result[i][j] = max_k ( left[i][k] + right[k][j] )
    for (int i = 0; i < vertexCount; i++) {
        for (int j = 0; j < vertexCount; j++) {
            bool found = false;

            for (int k = 0; k < vertexCount; k++) {
                if (left.at(i, k) == INF || right.at(k, j) == INF) {
                    continue;
                }

                int candidate = left.at(i, k) + right.at(k, j);

                if (!found || candidate > result.at(i, j)) {
                    result.at(i, j) = candidate;
                    found = true;
                }
            }
        }
    }

    return result;
}

Matrix Graph::shimbellMin(int edgesCount) const {
    // Если пользователь ввёл некорректное число рёбер,
    // просто возвращаем текущую весовую матрицу
    if (edgesCount <= 1) {
        return weightMatrix;
    }

    // Для путей длины 1 ребро матрица уже есть: это weightMatrix
    Matrix result = weightMatrix;

    // Каждый следующий шаг увеличивает длину пути ровно на 1 ребро
    for (int step = 2; step <= edgesCount; step++) {
        result = shimbellStepMin(result, weightMatrix);
    }

    return result;
}

Matrix Graph::shimbellMax(int edgesCount) const {
    if (edgesCount <= 1) {
        return weightMatrix;
    }

    Matrix result = weightMatrix;

    for (int step = 2; step <= edgesCount; step++) {
        result = shimbellStepMax(result, weightMatrix);
    }

    return result;
}

void Graph::printShimbellResult(int edgesCount, bool findMin, bool findMax) const {
    if (edgesCount == 0) {
        Matrix minMatrix = shimbellMin(edgesCount);

        std::cout << "\nМатрица минимальных путей методом Шимбелла"
                << " для длины " << edgesCount << " рёбер:\n\n    ";

        for (int j = 0; j < vertexCount; j++) {
            std::cout << std::setw(6) << j+1;
        }
        std::cout << "\n";

        for (int i = 0; i < vertexCount; i++) {
            std::cout << std::setw(3) << i+1 << " ";
            for (int j = 0; j < vertexCount; j++) {
                if (i != j) {
                    std::cout << std::setw(6) << ".";
                } else {
                    std::cout << std::setw(6) << 1;
                }
            }
            std::cout << "\n";
        }
    }
    if (edgesCount > 0) {
        if (findMin) {
            Matrix minMatrix = shimbellMin(edgesCount);

            std::cout << "\nМатрица минимальных путей методом Шимбелла"
                    << " для длины " << edgesCount << " рёбер:\n\n    ";

            for (int j = 0; j < vertexCount; j++) {
                std::cout << std::setw(6) << j+1;
            }
            std::cout << "\n";

            for (int i = 0; i < vertexCount; i++) {
                std::cout << std::setw(3) << i+1 << " ";
                for (int j = 0; j < vertexCount; j++) {
                    if (minMatrix.at(i, j) == INF) {
                        std::cout << std::setw(6) << ".";
                    } else {
                        std::cout << std::setw(6) << minMatrix.at(i, j);
                    }
                }
                std::cout << "\n";
            }
        }

        if (findMax) {
            Matrix maxMatrix = shimbellMax(edgesCount);

            std::cout << "\nМатрица максимальных путей методом Шимбелла"
                    << " для длины " << edgesCount << " рёбер:\n\n    ";

            for (int j = 0; j < vertexCount; j++) {
                std::cout << std::setw(6) << j+1;
            }
            std::cout << "\n";

            for (int i = 0; i < vertexCount; i++) {
                std::cout << std::setw(3) << i+1 << " ";
                for (int j = 0; j < vertexCount; j++) {
                    if (maxMatrix.at(i, j) == INF) {
                        std::cout << std::setw(6) << ".";
                    } else {
                        std::cout << std::setw(6) << maxMatrix.at(i, j);
                    }
                }
                std::cout << "\n";
            }
        }
    }

}



    //--------------------------------------
    //          часть 4
    //--------------------------------------




//находим пути
bool Graph::routeExistsOriented(int start, int finish) const {
    // Проверка корректности номеров вершин
    if (start < 0 || start >= vertexCount || finish < 0 || finish >= vertexCount) {
        return false;
    }

    // visited[i] = была ли вершина i уже посещена
    std::vector<bool> visited(vertexCount, false);

    // Рекурсивный DFS
    std::function<bool(int)> dfs = [&](int v) -> bool {
        // Если дошли до конечной вершины, маршрут существует
        if (v == finish) {
            return true;
        }

        visited[v] = true;

        // Просматриваем всех возможных соседей по ориентированному графу
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
    // Проверка корректности номеров вершин
    if (start < 0 || start >= vertexCount || finish < 0 || finish >= vertexCount) {
        return 0;
    }

    // memo[v] будет хранить количество маршрутов из v в finish.
    // -1 означает, что значение ещё не вычислено.
    std::vector<long long> memo(vertexCount, -1);

    std::function<long long(int)> dfsCount = [&](int v) -> long long {
        // Если пришли в конечную вершину, найден один маршрут
        if (v == finish) {
            return 1;
        }

        // Если уже считали раньше, используем готовый результат
        if (memo[v] != -1) {
            return memo[v];
        }

        long long total = 0;

        // Суммируем количество маршрутов через всех потомков
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
    // Проверка корректности номеров вершин
    if (start < 0 || start >= vertexCount || finish < 0 || finish >= vertexCount) {
        std::cout << "Некорректные номера вершин.\n";
        return;
    }

    bool exists = routeExistsOriented(start, finish);
    long long routesCount = countRoutesOriented(start, finish);

    std::cout << "\nМаршрут из вершины " << start
              << " в вершину " << finish << ":\n";

    if (exists) {
        std::cout << "Маршрут существует.\n";
    } else {
        std::cout << "Маршрут не существует.\n";
    }

    std::cout << "Количество маршрутов: " << routesCount << "\n";
}