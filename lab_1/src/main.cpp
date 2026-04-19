#include <iostream>
#include <ctime>
#include <cstdlib>
#include "../header/general.h"
#include "../header/distribution.h"

using namespace std;

void mainMenu() {
    cout << "\n========== МЕНЮ ==========\n";
    cout << "1. Сгенерировать граф\n";
    cout << "2. Перегенерировать граф\n";
    cout << "3. Показать текущий граф\n";
    cout << "4. Посчитать эксцентриситеты, центр и диаметральные вершины\n";
    cout << "5. Сгенерировать весовую матрицу\n";
    cout << "6. Показать текущую весовую матрицу\n";
    cout << "7. Выполнить метод Шимбелла\n";
    cout << "8. Проверить существование маршрута и количество маршрутов\n";
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

void shimbellWeightModeMenu() {
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
    cout << "Введите количество вершин: ";
    cin >> n;

    if (n <= 0) {
        cout << "Количество вершин должно быть положительным.\n";
        return;
    }

    graph.setVertexCount(n);
    graph.generateOrientedAcyclic(distribution);
    graph.buildUndirectedFromOriented();

    cout << "Граф успешно сгенерирован и сохранён в памяти.\n";
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

void generateWeights(Graph& graph, const PascalDistribution& distribution, bool& weightMatrixCreated) {
    if (graph.getVertexCount() == 0) {
        cout << "Сначала необходимо сгенерировать граф.\n";
        return;
    }

    int weightModeChoice;
    shimbellWeightModeMenu();
    cin >> weightModeChoice;

    int mode;
    if (weightModeChoice == 1) {
        mode = 0;
    }
    else if (weightModeChoice == 2) {
        mode = 1;
    }
    else if (weightModeChoice == 3) {
        mode = 2;
    }
    else {
        cout << "Некорректный выбор режима генерации весов.\n";
        return;
    }

    graph.generateWeightMatrix(distribution, mode);
    weightMatrixCreated = true;

    cout << "Весовая матрица успешно сгенерирована и сохранена.\n";
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

    int edgesCount;
    cout << "Введите количество рёбер в пути: ";
    cin >> edgesCount;

    if (edgesCount < 0) {
        cout << "Количество рёбер не может быть отрицательным.\n";
        return;
    }

    if (edgesCount > graph.getVertexCount() - 1) {
        cout << "Для ациклического графа длина пути не может быть больше "
             << graph.getVertexCount() - 1 << ".\n";
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

    // Внутри Graph используется нумерация с 0
    graph.printRouteInfoOriented(start - 1, finish - 1);
}

int main() {
    srand((unsigned)time(0));

    const int r = 3;
    const double p = 0.5;

    PascalDistribution distribution(r, p);
    Graph graph;

    bool weightMatrixCreated = false;

    int choice;

    do {
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
            if (graph.getVertexCount() == 0) {
                cout << "Сначала необходимо сгенерировать граф.\n";
            } else {
                graph.printGraphCharacteristicsUndirected();
            }
        }
        else if (choice == 5) {
            generateWeights(graph, distribution, weightMatrixCreated);
        }
        else if (choice == 6) {
            if (!weightMatrixCreated) {
                cout << "Сначала необходимо сгенерировать весовую матрицу.\n";
            } else {
                graph.printWeightMatrix();
            }
        }
        else if (choice == 7) {
            runShimbell(graph, weightMatrixCreated);
        }
        else if (choice == 8) {
            runRoutes(graph);
        }
        else if (choice == 0) {
            cout << "Выход из программы.\n";
        }
        else {
            cout << "Некорректный пункт меню.\n";
        }

    } while (choice != 0);

    return 0;
}