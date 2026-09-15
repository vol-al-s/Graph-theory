#include <iostream>
#include <ctime>
#include <cstdlib>
#include "../header/general.h"
//#include "../header/flow.h"
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
    cout << "----------------- 2 лабораторная -------------------------------\n";
    cout << "9. Выполнить обход вершин графа поиском в глубину\n";
    cout << "10. Найти кратчайший путь алгоритмом Беллмана-Форда\n";
    cout << "----------------- 3 лабораторная -------------------------------\n";
    cout << "11. Сгенерировать матрицы пропускных способностей и стоимостей\n";
    cout << "12. Найти максимальный поток (Форд-Фалкерсон)\n";
    cout << "13. Найти поток заданной величины минимальной стоимости\n";
    cout << "----------------- 4 лабораторная -------------------------------\n";
    cout << "14. Найти число остовных деревьев (теорема Кирхгофа)\n";
    cout << "15. Построить минимальный остов (Краскал) + код Прюфера\n";
    cout << "16. Найти минимальное вершинное покрытие\n";
    cout << "----------------- 5 лабораторная -------------------------------\n";
    cout << "17. Проверить эйлеровость и построить эйлеров цикл\n";
    cout << "18. Фундаментальная система циклов + симметрическая разность\n";
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

    if (edgesCount > limit /*graph.getVertexCount() - 1*/) {
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

void runFlowMatrices(Graph& graph,
                     const PascalDistribution& distribution,
                     bool& flowMatricesCreated) {
    if (graph.getVertexCount() == 0) {
        cout << "Сначала необходимо сгенерировать граф.\n";
        return;
    }
    graph.generateCapacityAndCostMatrices(distribution);
    flowMatricesCreated = true;
    cout << "Матрицы успешно сгенерированы.\n";
    graph.printCapacityMatrix();
    graph.printCostMatrix();
}

bool selectSourceAndSink(const Graph& graph, int& source, int& sink) {
    std::vector<int> sources = graph.findSources();
    std::vector<int> sinks   = graph.findSinks();

    if (sources.empty()) {
        cout << "В сети нет ни одной вершины-истока (без входящих дуг).\n";
        return false;
    }
    if (sinks.empty()) {
        cout << "В сети нет ни одной вершины-стока (без исходящих дуг).\n";
        return false;
    }

    // Истоки
    cout << "\nДоступные истоки (вершины без входящих дуг): ";
    for (size_t i = 0; i < sources.size(); i++) {
        cout << sources[i] + 1;
        if (i + 1 < sources.size()) cout << ", ";
    }
    cout << "\n";

    if (sources.size() == 1) {
        source = sources[0];
        cout << "Автоматически выбран единственный исток: вершина "
             << source + 1 << "\n";
    } else {
        cout << "Введите номер истока: ";
        int chosen;
        cin >> chosen;
        chosen -= 1;
        bool ok = false;
        for (int s : sources) if (s == chosen) { ok = true; break; }
        if (!ok) {
            cout << "Указанная вершина не является истоком.\n";
            return false;
        }
        source = chosen;
    }

    // Стоки
    cout << "\nДоступные стоки (вершины без исходящих дуг): ";
    for (size_t i = 0; i < sinks.size(); i++) {
        cout << sinks[i] + 1;
        if (i + 1 < sinks.size()) cout << ", ";
    }
    cout << "\n";

    if (sinks.size() == 1) {
        sink = sinks[0];
        cout << "Автоматически выбран единственный сток: вершина "
             << sink + 1 << "\n";
    } else {
        cout << "Введите номер стока: ";
        int chosen;
        cin >> chosen;
        chosen -= 1;
        bool ok = false;
        for (int s : sinks) if (s == chosen) { ok = true; break; }
        if (!ok) {
            cout << "Указанная вершина не является стоком.\n";
            return false;
        }
        sink = chosen;
    }

    if (source == sink) {
        cout << "Исток и сток должны различаться.\n";
        return false;
    }

    return true;
}

void runMaxFlow(Graph& graph, bool flowMatricesCreated) {
    if (graph.getVertexCount() == 0) {
        cout << "Сначала необходимо сгенерировать граф.\n";
        return;
    }
    if (!flowMatricesCreated) {
        cout << "Сначала необходимо сгенерировать матрицы (пункт 11).\n";
        return;
    }

    int source, sink;
    if (!selectSourceAndSink(graph, source, sink)) return;

    graph.printMaxFlowResult(source, sink);
}

void runMinCostFlow(Graph& graph, bool flowMatricesCreated) {
    if (graph.getVertexCount() == 0) {
        cout << "Сначала необходимо сгенерировать граф.\n";
        return;
    }
    if (!flowMatricesCreated) {
        cout << "Сначала необходимо сгенерировать матрицы (пункт 11).\n";
        return;
    }

    int source, sink;
    if (!selectSourceAndSink(graph, source, sink)) return;

    graph.printMinCostFlowResult(source, sink);
}

void vertexCoverGraphTypeMenu() {
    cout << "\nНа каком графе искать минимальное вершинное покрытие?\n";
    cout << "1. На исходном неориентированном графе\n";
    cout << "2. На минимальном остове (Краскал)\n";
    cout << "Ваш выбор: ";
}

void runKirchhoff(Graph& graph) {
    if (graph.getVertexCount() == 0) {
        cout << "Сначала необходимо сгенерировать граф.\n";
        return;
    }
    graph.printKirchhoffResult();
}

void runKruskalPrufer(Graph& graph, bool weightMatrixCreated) {
    if (graph.getVertexCount() == 0) {
        cout << "Сначала необходимо сгенерировать граф.\n";
        return;
    }
    if (!weightMatrixCreated) {
        cout << "Сначала необходимо сгенерировать весовую матрицу.\n";
        return;
    }
    graph.printKruskalResult();
    graph.printPruferResult();
}

void runVertexCover(Graph& graph, bool weightMatrixCreated) {
    if (graph.getVertexCount() == 0) {
        cout << "Сначала необходимо сгенерировать граф.\n";
        return;
    }
    int choice;
    vertexCoverGraphTypeMenu();
    cin >> choice;

    bool useSpanning;
    if (choice == 1) useSpanning = false;
    else if (choice == 2) {
        useSpanning = true;
        if (!weightMatrixCreated) {
            cout << "Для остова нужна весовая матрица. Сначала её сгенерируйте.\n";
            return;
        }
    } else {
        cout << "Некорректный выбор.\n";
        return;
    }
    graph.printVertexCoverResult(useSpanning);
}

void runEuler(Graph& graph) {
    if (graph.getVertexCount() == 0) {
        cout << "Сначала необходимо сгенерировать граф.\n";
        return;
    }
    graph.printEulerResult();
}

void runFundamentalCycles(Graph& graph, bool weightMatrixCreated) {
    if (graph.getVertexCount() == 0) {
        cout << "Сначала необходимо сгенерировать граф.\n";
        return;
    }
    if (!weightMatrixCreated) {
        cout << "Сначала необходимо сгенерировать весовую матрицу.\n";
        return;
    }

    vector<EdgeSet> cycles = graph.printFundamentalCyclesResult();
    if (cycles.empty()) return;

    cout << "\nСимметрическая разность циклов.\n";
    cout << "Вводите номера циклов (1.." << cycles.size()
         << ") по одному, 0 — завершить ввод.\n";

    EdgeSet result;
    bool started = false;

    while (true) {
        cout << "  Номер цикла: ";
        int index;
        cin >> index;

        if (index == 0) break;

        if (index < 1 || index > (int)cycles.size()) {
            cout << "  Неверный номер (1.." << cycles.size() << ").\n";
            continue;
        }

        if (!started) {
            result = cycles[index - 1];
            started = true;
        } else {
            result = Graph::symmetricDifference(result, cycles[index - 1]);
        }

        cout << "  Текущий результат: ";
        Graph::printEdgeSet(result);
        cout << "\n";
    }

    if (!started) {
        cout << "Ни одного цикла не выбрано.\n";
        return;
    }

    cout << "\nИтоговое множество рёбер: ";
    Graph::printEdgeSet(result);
    if (result.empty()) cout << "  (пусто)";
    cout << "\n";
}

int main() {

    srand((unsigned)time(0));

    const int r = 3;
    const double p = 0.6;

    PascalDistribution distribution(r, p);
    Graph graph;

    bool weightMatrixCreated = false;
    bool flowMatricesCreated = false;

    int choice;

    //FlowNetwork flow(graph.getVertexCount());

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
            flowMatricesCreated = false;
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
        else if (choice == 11) {
            runFlowMatrices(graph, distribution, flowMatricesCreated);
        }
        else if (choice == 12) {
            runMaxFlow(graph, flowMatricesCreated);
        }
        else if (choice == 13) {
            runMinCostFlow(graph, flowMatricesCreated);
        }
        else if (choice == 14) { runKirchhoff(graph); }
        else if (choice == 15) { runKruskalPrufer(graph, weightMatrixCreated); }
        else if (choice == 16) { runVertexCover(graph, weightMatrixCreated); }
        else if (choice == 17) { runEuler(graph); }
        else if (choice == 18) { runFundamentalCycles(graph, weightMatrixCreated); }
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