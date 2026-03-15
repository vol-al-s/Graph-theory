#include <iostream>
#include <iomanip>
#include <limits>
#include "../header/graph.h"
#include "../header/random_generator.h"
#include "../header/algorithms.h"

using namespace std;
const int INF = numeric_limits<int>::max() / 2;

// Глобальные объекты
Graph* graph = nullptr;
PascalRandomGenerator* rng = nullptr;

// Константы для распределения Паскаля (подбираются экспериментально)
const int PASCAL_M = 2;        // Параметр m
const double PASCAL_P = 0.5;   // Вероятность p

void clearInput() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

int getValidInt(const string& prompt, int minVal = 0, int maxVal = 1000) {
    int value;
    while (true) {
        cout << prompt;
        if (cin >> value && value >= minVal && value <= maxVal) {
            clearInput();
            return value;
        }
        cout << "Неверный ввод! Попробуйте снова." << endl;
        clearInput();
    }
}

void generateTree() {
    if (graph) delete graph;
    
    int n = getValidInt("Введите количество вершин: ", 1, 100);
    
    graph = new Graph(n);
    
    if (n == 1) {
        cout << "Дерево с одной вершиной создано." << endl;
        return;
    }
    
    // Генерация степеней вершин по распределению Паскаля
    if (rng) delete rng;
    rng = new PascalRandomGenerator(PASCAL_M, PASCAL_P);
    
    vector<int> degrees(n);
    int sumDegrees = 0;
    
    // Для дерева сумма степеней должна быть 2(n-1)
    int targetSum = 2 * (n - 1);
    
    // Генерируем степени
    for (int i = 0; i < n; i++) {
        degrees[i] = rng->generate();
        // Степень не может быть 0 для связного графа (кроме случая n=1)
        if (degrees[i] < 1) degrees[i] = 1;
        sumDegrees += degrees[i];
    }
    
    // Корректируем суммы до 2(n-1)
    while (sumDegrees < targetSum) {
        int idx = rng->generate() % n;
        degrees[idx]++;
        sumDegrees++;
    }
    
    while (sumDegrees > targetSum) {
        for (int i = 0; i < n && sumDegrees > targetSum; i++) {
            if (degrees[i] > 1) {
                degrees[i]--;
                sumDegrees--;
            }
        }
    }
    
    // Построение дерева (алгоритм соединения вершин)
    vector<int> available;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < degrees[i]; j++) {
            available.push_back(i);
        }
    }
    
    // Соединяем вершины, избегая циклов
    int edgesAdded = 0;
    for (size_t i = 0; i < available.size() && edgesAdded < n - 1; i++) {
        for (size_t j = i + 1; j < available.size() && edgesAdded < n - 1; j++) {
            int u = available[i];
            int v = available[j];
            
            if (u != v && !graph->hasEdge(u, v)) {
                // Проверяем, не создаст ли это цикл
                graph->addEdge(u, v, 1);
                edgesAdded++;
                
                if (graph->isAcyclic()) {
                    break;
                } else {
                    graph->removeEdge(u, v);
                    edgesAdded--;
                }
            }
        }
    }
    
    // Если дерево не связное, соединяем компоненты
    while (!graph->isConnected() && edgesAdded < n - 1) {
        for (int i = 0; i < n; i++) {
            for (int j = i + 1; j < n; j++) {
                if (!graph->hasEdge(i, j)) {
                    graph->addEdge(i, j, 1);
                    edgesAdded++;
                    
                    if (graph->isConnected() && graph->isAcyclic()) {
                        goto done;
                    } else if (!graph->isAcyclic()) {
                        graph->removeEdge(i, j);
                        edgesAdded--;
                    }
                }
            }
        }
    }
    
done:
    cout << "Дерево создано!" << endl;
    cout << "Вершин: " << graph->getVertexCount() << endl;
    cout << "Рёбер: " << graph->getEdgeCount() << endl;
    cout << "Связный: " << (graph->isConnected() ? "Да" : "Нет") << endl;
    cout << "Ациклический: " << (graph->isAcyclic() ? "Да" : "Нет") << endl;
}

void calculateEccentricities() {
    if (!graph || graph->getVertexCount() == 0) {
        cout << "Сначала создайте граф!" << endl;
        return;
    }
    
    cout << "\n=== Экцентриситеты вершин ===" << endl;
    auto eccentricities = GraphAlgorithms::calculateEccentricities(*graph);
    
    for (size_t i = 0; i < eccentricities.size(); i++) {
        cout << "Вершина " << i << ": " << eccentricities[i] << endl;
    }
    
    cout << "\n=== Центр графа ===" << endl;
    auto center = GraphAlgorithms::findGraphCenter(*graph);
    cout << "Центральные вершины: ";
    for (int v : center) {
        cout << v << " ";
    }
    cout << endl;
    
    cout << "\n=== Диаметральные вершины ===" << endl;
    auto diametral = GraphAlgorithms::findDiametralVertices(*graph);
    cout << "Пары вершин с максимальным расстоянием:" << endl;
    for (const auto& p : diametral) {
        cout << "(" << p.first << ", " << p.second << ")" << endl;
    }
    
    cout << "\nДиаметр графа: " << GraphAlgorithms::calculateDiameter(*graph) << endl;
}

void shimbelMethod() {
    if (!graph || graph->getVertexCount() == 0) {
        cout << "Сначала создайте граф!" << endl;
        return;
    }
    
    cout << "\n=== Метод Шимбеля ===" << endl;
    
    int edgeCount = getValidInt("Введите количество рёбер для весовой матрицы: ", 
                                 graph->getEdgeCount(), 100);
    
    // Генерация весов по распределению Паскаля
    cout << "\nТип весов:" << endl;
    cout << "1. Только положительные" << endl;
    cout << "2. Только отрицательные" << endl;
    cout << "3. Смешанные" << endl;
    int weightType = getValidInt("Выберите тип: ", 1, 3);
    
    // Добавляем рёбра до нужного количества
    while (graph->getEdgeCount() < edgeCount) {
        int u = rng->generate() % graph->getVertexCount();
        int v = rng->generate() % graph->getVertexCount();
        
        if (u != v && !graph->hasEdge(u, v)) {
            int weight = rng->generate();
            
            if (weightType == 2) {
                weight = -weight;
            } else if (weightType == 3) {
                if (rng->generate() % 2 == 0) {
                    weight = -weight;
                }
            }
            
            graph->addEdge(u, v, weight);
        }
    }
    
    cout << "\nМатрица кратчайших путей:" << endl;
    auto distMatrix = GraphAlgorithms::shimbelAlgorithm(*graph, weightType != 1);
    
    int n = graph->getVertexCount();
    cout << "    ";
    for (int j = 0; j < n; j++) {
        cout << setw(6) << j;
    }
    cout << endl;
    
    for (int i = 0; i < n; i++) {
        cout << setw(4) << i << ":";
        for (int j = 0; j < n; j++) {
            if (distMatrix[i][j] == INF) {
                cout << setw(6) << "INF";
            } else {
                cout << setw(6) << distMatrix[i][j];
            }
        }
        cout << endl;
    }
}

void findRoutes() {
    if (!graph || graph->getVertexCount() == 0) {
        cout << "Сначала создайте граф!" << endl;
        return;
    }
    
    cout << "\n=== Поиск маршрутов ===" << endl;
    
    int start = getValidInt("Введите начальную вершину: ", 0, graph->getVertexCount() - 1);
    int end = getValidInt("Введите конечную вершину: ", 0, graph->getVertexCount() - 1);
    
    if (start == end) {
        cout << "Начальная и конечная вершины совпадают!" << endl;
        return;
    }
    
    bool hasRoute = GraphAlgorithms::hasRoute(*graph, start, end);
    cout << "\nМаршрут существует: " << (hasRoute ? "Да" : "Нет") << endl;
    
    if (hasRoute) {
        int count = GraphAlgorithms::countRoutes(*graph, start, end);
        cout << "Количество маршрутов: " << count << endl;
        
        if (count <= 20) {
            auto allRoutes = GraphAlgorithms::findAllRoutes(*graph, start, end);
            cout << "\nВсе маршруты:" << endl;
            for (size_t i = 0; i < allRoutes.size(); i++) {
                cout << (i + 1) << ": ";
                for (size_t j = 0; j < allRoutes[i].size(); j++) {
                    cout << allRoutes[i][j];
                    if (j < allRoutes[i].size() - 1) cout << " -> ";
                }
                cout << endl;
            }
        }
    }
}

void printMenu() {
    cout << "\n========================================" << endl;
    cout << "   ЛАБОРАТОРНАЯ РАБОТА ПО ТЕОРИИ ГРАФОВ" << endl;
    cout << "========================================" << endl;
    cout << "1. Сгенерировать дерево (распределение Паскаля)" << endl;
    cout << "2. Рассчитать эксцентриситеты и центр графа" << endl;
    cout << "3. Метод Шимбеля (весовая матрица)" << endl;
    cout << "4. Поиск маршрутов между вершинами" << endl;
    cout << "5. Показать информацию о графе" << endl;
    cout << "0. Выход" << endl;
    cout << "========================================" << endl;
}

void showGraphInfo() {
    if (!graph || graph->getVertexCount() == 0) {
        cout << "Граф не создан!" << endl;
        return;
    }
    
    cout << "\n=== Информация о графе ===" << endl;
    cout << "Вершин: " << graph->getVertexCount() << endl;
    cout << "Рёбер: " << graph->getEdgeCount() << endl;
    cout << "Связный: " << (graph->isConnected() ? "Да" : "Нет") << endl;
    cout << "Ациклический: " << (graph->isAcyclic() ? "Да" : "Нет") << endl;
    cout << "Дерево: " << (graph->isTree() ? "Да" : "Нет") << endl;
    
    if (rng) {
        cout << "\nПараметры распределения Паскаля:" << endl;
        cout << "m = " << rng->getM() << endl;
        cout << "p = " << rng->getP() << endl;
        cout << "Мат. ожидание: " << rng->getExpectedValue() << endl;
        cout << "Дисперсия: " << rng->getVariance() << endl;
    }
}

int main() {
    setlocale(LC_ALL, "Russian");
    
    int choice;
    
    do {
        printMenu();
        choice = getValidInt("Ваш выбор: ", 0, 5);
        
        switch (choice) {
            case 1:
                generateTree();
                break;
            case 2:
                calculateEccentricities();
                break;
            case 3:
                shimbelMethod();
                break;
            case 4:
                findRoutes();
                break;
            case 5:
                showGraphInfo();
                break;
            case 0:
                cout << "Программа завершена." << endl;
                break;
            default:
                cout << "Неверный выбор!" << endl;
        }
    } while (choice != 0);
    
    if (graph) delete graph;
    if (rng) delete rng;
    
    return 0;
}