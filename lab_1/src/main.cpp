#include <iostream>
#include <ctime>
#include <cstdlib>
#include "../header/general.h"
#include "../header/distribution.h"

using namespace std;

void clearConsole() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void mainMenu() {
    cout << "\n========== МЕНЮ ==========\n";
    cout << "1. Сгенерировать граф\n";
    cout << "2. Перегенерировать граф\n";
    cout << "3. Показать текущий граф\n";
    cout << "4. Посчитать эксцентриситеты, радиус, центр и диаметральные вершины\n";
    cout << "5. Сгенерировать весовую матрицу\n";
    cout << "6. Показать весовую матрицу\n";
    cout << "7. Выполнить метод Шимбелла\n";
    cout << "8. Проверить существование маршрута и количество маршрутов\n";
    cout << "----------------- 2 лаба -------------------------------\n";
    cout << "9. Выполнить обход вершин графа поиском в глубину\n";
    cout << "10. Найти кратчайший путь алгоритмом Беллмана-Форда\n";
    cout << "0. Выход\n";
    cout << "Ваш выбор: ";
}

void graphOutputMenu() {
    cout << "\nКакой граф вывести?\n";
    cout << "1. Только ориентированный\n";
    cout << "2. Только неориентированный\n";
    cout << "3. Оба графа\n";
    cout << "Ваш выбор: ";
}

void characteristicsGraphTypeMenu() {
    cout << "\nДля какого графа вычислять характеристики?\n";
    cout << "1. Для ориентированного\n";
    cout << "2. Для неориентированного\n";
    cout << "Ваш выбор: ";
}

void dfsGraphTypeMenu() {
    cout << "\nПо какому графу выполнить поиск в глубину?\n";
    cout << "1. По ориентированному\n";
    cout << "2. По неориентированному\n";
    cout << "Ваш выбор: ";
}

void weightModeMenu() {
    cout << "\nВыберите режим генерации весов:\n";
    cout << "1. Только положительные\n";
    cout << "2. Только отрицательные\n";
    cout << "3. Смешанные\n";
    cout << "Ваш выбор: ";
}

void shimbellResultMenu() {
    cout << "\nЧто нужно найти методом Шимбелла?\n";
    cout << "1. Минимальные пути\n";
    cout << "2. Максимальные пути\n";
    cout << "3. И минимальные, и максимальные\n";
    cout << "Ваш выбор: ";
}

void generateGraph(Graph& graph, const PascalDistribution& distribution) {
    int n;
    cout << "Введите количество вершин (для отмены введите '-1'): ";
    cin >> n;

    if (n == 0) {
        cout << "Количество вершин должно быть положительным.\n";
        return;
    }
    if (n <= -1) {
        cout << "Генерация отменена";
        return;
    }

    graph.setVertexCount(n);
    graph.generateOrientedAcyclic(distribution);
    graph.buildUndirectedFromOriented();

    cout << "Граф успешно сгенерирован.\n";
    graph.printOriented();
    graph.printUndirected();
}

void showCurrentGraph(const Graph& graph) {
    if (graph.getVertexCount() == 0) {
        cout << "Сначала необходимо сгенерировать граф.\n";
        return;
    }

    int graphChoice;
    graphOutputMenu();
    cin >> graphChoice;

    if (graphChoice == 1) {
        graph.printOriented();
    }
    else if (graphChoice == 2) {
        graph.printUndirected();
    }
    else if (graphChoice == 3) {
        graph.printOriented();
        graph.printUndirected();
    }
    else {
        cout << "Некорректный выбор типа графа.\n";
    }
}

void runCharacteristics(Graph& graph) {
    if (graph.getVertexCount() == 0) {
        cout << "Сначала необходимо сгенерировать граф.\n";
        return;
    }

    int typeChoice;
    characteristicsGraphTypeMenu();
    cin >> typeChoice;

    if (typeChoice == 1) {
        graph.printOriented();
        graph.printGraphCharacteristics(true);
    }
    else if (typeChoice == 2) {
        graph.printUndirected();
        graph.printGraphCharacteristics(false);
    }
    else {
        cout << "Некорректный выбор типа графа.\n";
    }
}

void generateWeights(Graph& graph,
                     const PascalDistribution& distribution,
                     bool& weightMatrixCreated) {
    if (graph.getVertexCount() == 0) {
        cout << "Сначала необходимо сгенерировать граф.\n";
        return;
    }

    int choice;
    weightModeMenu();
    cin >> choice;

    int mode;
    if (choice == 1) {
        mode = 0;
    }
    else if (choice == 2) {
        mode = 1;
    }
    else if (choice == 3) {
        mode = 2;
    }
    else {
        cout << "Некорректный выбор режима весов.\n";
        return;
    }

    graph.generateWeightMatrix(distribution, mode);
    weightMatrixCreated = true;

    cout << "Весовая матрица успешно сгенерирована.\n";
    graph.printWeightMatrix();
}

void showWeightMatrix(const Graph& graph, bool weightMatrixCreated) {
    if (graph.getVertexCount() == 0) {
        cout << "Сначала необходимо сгенерировать граф.\n";
        return;
    }

    if (!weightMatrixCreated) {
        cout << "Сначала необходимо сгенерировать весовую матрицу.\n";
        return;
    }

    graph.printWeightMatrix();
}

void runShimbell(Graph& graph, bool weightMatrixCreated) {
    if (graph.getVertexCount() == 0) {
        cout << "Сначала необходимо сгенерировать граф.\n";
        return;
    }

    if (!weightMatrixCreated) {
        cout << "Сначала необходимо сгенерировать весовую матрицу.\n";
        return;
    }
    int limit = graph.findMaxPathLengthOriented();
    int edgesCount;
    cout << "Введите количество рёбер в пути (0 - " << limit << "): ";
    cin >> edgesCount;
    if (edgesCount == 0) {
        graph.printShimbellResult(0, true, true);
        return;
    }

    if (edgesCount < 0) {
        cout << "Количество рёбер не может быть отрицательным.\n";
        return;
    }

    if (edgesCount > graph.getVertexCount() - 1) {
        cout << "Для ациклического графа длина пути не может быть больше "
             << limit << ".\n";
        return;
    }

    int resultChoice;
    shimbellResultMenu();
    cin >> resultChoice;

    bool findMin = false;
    bool findMax = false;

    if (resultChoice == 1) {
        findMin = true;
    }
    else if (resultChoice == 2) {
        findMax = true;
    }
    else if (resultChoice == 3) {
        findMin = true;
        findMax = true;
    }
    else {
        cout << "Некорректный выбор результата метода Шимбелла.\n";
        return;
    }

    graph.printShimbellResult(edgesCount, findMin, findMax);
}

void runRoutes(Graph& graph) {
    if (graph.getVertexCount() == 0) {
        cout << "Сначала необходимо сгенерировать граф.\n";
        return;
    }

    int start, finish;

    cout << "Введите начальную вершину: ";
    cin >> start;
    cout << "Введите конечную вершину: ";
    cin >> finish;

    graph.printRouteInfoOriented(start - 1, finish - 1);
}

void runDfs(Graph& graph) {
    if (graph.getVertexCount() == 0) {
        cout << "Сначала необходимо сгенерировать граф.\n";
        return;
    }

    int typeChoice;
    dfsGraphTypeMenu();
    cin >> typeChoice;

    bool useOriented;
    if (typeChoice == 1) {
        useOriented = true;
    }
    else if (typeChoice == 2) {
        useOriented = false;
    }
    else {
        cout << "Некорректный выбор типа графа.\n";
        return;
    }

    int start;
    cout << "Введите стартовую вершину: ";
    cin >> start;

    graph.printDfsTraversal(useOriented, start - 1);
}

void runBellmanFord(Graph& graph, bool weightMatrixCreated) {
    if (graph.getVertexCount() == 0) {
        cout << "Сначала необходимо сгенерировать граф.\n";
        return;
    }

    if (!weightMatrixCreated) {
        cout << "Сначала необходимо сгенерировать весовую матрицу.\n";
        return;
    }

    int start, finish;

    cout << "Введите начальную вершину: ";
    cin >> start;
    cout << "Введите конечную вершину: ";
    cin >> finish;

    graph.printBellmanFordResult(start - 1, finish - 1);
}

int main() {
    srand((unsigned)time(0));

    const int r = 3;
    const double p = 0.6;

    PascalDistribution distribution(r, p);
    Graph graph;

    bool weightMatrixCreated = false;

    int choice;

    do {
        clearConsole();

        mainMenu();
        cin >> choice;

        if (choice == 1) {
            if (graph.getVertexCount() != 0) {
                cout << "Граф уже существует.\n";
                cout << "Используйте пункт 2, если хотите перегенерировать его.\n";
            } else {
                generateGraph(graph, distribution);
                weightMatrixCreated = false;
            }
        }
        else if (choice == 2) {
            generateGraph(graph, distribution);
            weightMatrixCreated = false;
        }
        else if (choice == 3) {
            showCurrentGraph(graph);
        }
        else if (choice == 4) {
            runCharacteristics(graph);
        }
        else if (choice == 5) {
            generateWeights(graph, distribution, weightMatrixCreated);
        }
        else if (choice == 6) {
            showWeightMatrix(graph, weightMatrixCreated);
        }
        else if (choice == 7) {
            runShimbell(graph, weightMatrixCreated);
        }
        else if (choice == 8) {
            runRoutes(graph);
        }
        else if (choice == 9) {
            runDfs(graph);
        }
        else if (choice == 10) {
            runBellmanFord(graph, weightMatrixCreated);
        }
        else if (choice == 0) {
            cout << "Выход из программы.\n";
        }
        else {
            cout << "Некорректный пункт меню.\n";
        }

        if (choice != 0) {
            cout << "\n";
            cin.ignore(10000, '\n');
            cin.get();
        }

    } while (choice != 0);

    return 0;
}