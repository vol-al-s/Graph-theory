#include <iostream>
#include <ctime>
#include <cstdlib>
#include "../header/graph.h"
#include "../header/distribution.h"

using namespace std;

void mainMenu() {
    cout << "\n========== МЕНЮ ==========\n";
    cout << "1. Сгенерировать граф\n";
    cout << "2. Перегенерировать граф\n";
    cout << "3. Показать текущий граф\n";
    cout << "4. Посчитать эксцентриситеты, центр и диаметральные вершины\n";
    cout << "5. Выполнить метод Шимбелла\n";
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

void runShimbell(Graph& graph, const PascalDistribution& distribution) {
    if (graph.getVertexCount() == 0) {
        cout << "Сначала необходимо сгенерировать граф.\n";
        return;
    }

    int weightModeChoice;
    shimbellWeightModeMenu();
    cin >> weightModeChoice;

    int mode;
    if (weightModeChoice == 1) {
        mode = 0; // положительные
    }
    else if (weightModeChoice == 2) {
        mode = 1; // отрицательные
    }
    else if (weightModeChoice == 3) {
        mode = 2; // смешанные
    }
    else {
        cout << "Некорректный выбор режима генерации весов.\n";
        return;
    }

    int edgesCount;
    cout << "Введите количество рёбер в пути: ";
    cin >> edgesCount;

    if (edgesCount <= 0) {
        cout << "Количество рёбер должно быть положительным.\n";
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

    graph.generateWeightMatrix(distribution, mode);

    cout << "\nСгенерированная весовая матрица:\n";
    graph.printWeightMatrix();

    graph.printShimbellResult(edgesCount, findMin, findMax);
}

int main() {
    srand((unsigned)time(0));

    const int r = 3;
    const double p = 0.5;

    PascalDistribution distribution(r, p);
    Graph graph;

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
            }
        }
        else if (choice == 2) {
            generateGraph(graph, distribution);
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
            runShimbell(graph, distribution);
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