#include "../header/general.h"
#include <iostream>
#include <vector>
#include <queue>

// ============================================================
//   ЛАБОРАТОРНАЯ 5, ЧАСТЬ 1: ЭЙЛЕРОВ ЦИКЛ
// ============================================================

// Строим явный список рёбер неориентированного графа.
// Каждое ребро попадает в список ровно один раз (берём верхний треугольник),
// и у каждого появляется свой НОМЕР — индекс в векторе.
// Номер нужен, чтобы отличать кратные рёбра друг от друга.
std::vector<Graph::EulerEdge> Graph::buildEulerEdges() const {
    std::vector<EulerEdge> edges;

    for (int i = 0; i < vertexCount; i++) {
        for (int j = i + 1; j < vertexCount; j++) {
            if (undirectedMatrix.at(i, j) == 1) {
                EulerEdge e;
                e.u = i;
                e.v = j;
                e.used = false;
                edges.push_back(e);
            }
        }
    }

    return edges;
}

// Степени вершин считаем по списку рёбер, а не по матрице:
// после модификации графа матрица уже не актуальна, а список — да.
std::vector<int> Graph::eulerDegrees(const std::vector<EulerEdge>& edges) const {
    std::vector<int> degrees(vertexCount, 0);

    for (size_t i = 0; i < edges.size(); i++) {
        degrees[edges[i].u]++;
        degrees[edges[i].v]++;
    }

    return degrees;
}

// Проверка связности графа, заданного списком рёбер, с пропуском одного ребра.
// skipIndex == -1 означает "ничего не пропускать" — тогда проверяется весь граф.
// Изолированные вершины игнорируются: на существование эйлерова цикла они не влияют.
bool Graph::connectedWithout(const std::vector<EulerEdge>& edges, int skipIndex) const {
    std::vector<std::vector<int> > adj(vertexCount);

    for (int i = 0; i < (int)edges.size(); i++) {
        if (i == skipIndex) continue;
        adj[edges[i].u].push_back(edges[i].v);
        adj[edges[i].v].push_back(edges[i].u);
    }

    int start = -1;
    for (int v = 0; v < vertexCount; v++) {
        if (!adj[v].empty()) { start = v; break; }
    }

    if (start == -1) return true;  // рёбер нет вообще — считаем связным

    std::vector<bool> visited(vertexCount, false);
    std::queue<int> q;
    q.push(start);
    visited[start] = true;

    while (!q.empty()) {
        int v = q.front();
        q.pop();
        for (size_t k = 0; k < adj[v].size(); k++) {
            int u = adj[v][k];
            if (!visited[u]) {
                visited[u] = true;
                q.push(u);
            }
        }
    }

    for (int v = 0; v < vertexCount; v++) {
        if (!adj[v].empty() && !visited[v]) return false;
    }

    return true;
}

// Делаем граф эйлеровым: все степени должны стать чётными.
// Вершин нечётной степени всегда чётное количество, поэтому разбиваем их на пары.
// Для каждой пары (u, v):
//   1) если ребро u-v уже есть И граф без него останется связным — УДАЛЯЕМ его
//      (обе степени падают на 1 и становятся чётными);
//   2) иначе ДОБАВЛЯЕМ ребро u-v (обе степени растут на 1 и тоже становятся чётными).
// Все изменения логируются — это требование условия работы.
void Graph::makeEulerian(std::vector<EulerEdge>& edges) const {
    std::vector<int> degrees = eulerDegrees(edges);

    std::vector<int> odd;
    for (int v = 0; v < vertexCount; v++) {
        if (degrees[v] % 2 != 0) odd.push_back(v);
    }

    if (odd.empty()) return;

    std::cout << "  Модификация графа (удаление или добавление рёбер):\n";

    for (size_t i = 0; i + 1 < odd.size(); i += 2) {
        int u = odd[i];
        int v = odd[i + 1];

        int foundIndex = -1;
        for (int k = 0; k < (int)edges.size(); k++) {
            if ((edges[k].u == u && edges[k].v == v) ||
                (edges[k].u == v && edges[k].v == u)) {
                foundIndex = k;
                break;
            }
        }

        if (foundIndex != -1 && connectedWithout(edges, foundIndex)) {
            edges.erase(edges.begin() + foundIndex);

            std::cout << "    - удалено ребро   " << u + 1 << " -- " << v + 1
                      << "   (deg(" << u + 1 << "): " << degrees[u]
                      << " -> " << degrees[u] - 1
                      << ", deg(" << v + 1 << "): " << degrees[v]
                      << " -> " << degrees[v] - 1 << ")\n";

            degrees[u]--;
            degrees[v]--;
        } else {
            EulerEdge e;
            e.u = u;
            e.v = v;
            e.used = false;
            edges.push_back(e);

            std::cout << "    + добавлено ребро " << u + 1 << " -- " << v + 1
                      << "   (deg(" << u + 1 << "): " << degrees[u]
                      << " -> " << degrees[u] + 1
                      << ", deg(" << v + 1 << "): " << degrees[v]
                      << " -> " << degrees[v] + 1 << ")\n";

            degrees[u]++;
            degrees[v]++;
        }
    }

    std::cout << "  Граф модифицирован — теперь все степени чётные.\n";
}

// Алгоритм Хирхольцера, итеративная версия (без рекурсии).
//   adj[v]  — список пар (сосед, номер ребра)
//   ptr[v]  — докуда мы уже просмотрели список adj[v]; назад не откатывается
//   used    — флаг "ребро уже пройдено" (одно ребро лежит в списках обеих вершин)
// Пока у верхушки стека есть непройденное ребро — идём по нему вперёд.
// Если выйти некуда — снимаем вершину со стека и дописываем в результат.
std::vector<int> Graph::hierholzer(std::vector<EulerEdge>& edges) const {
    std::vector<std::vector<std::pair<int, int> > > adj(vertexCount);

    for (int i = 0; i < (int)edges.size(); i++) {
        adj[edges[i].u].push_back(std::make_pair(edges[i].v, i));
        adj[edges[i].v].push_back(std::make_pair(edges[i].u, i));
        edges[i].used = false;
    }

    std::vector<int> ptr(vertexCount, 0);

    int start = 0;
    for (int v = 0; v < vertexCount; v++) {
        if (!adj[v].empty()) { start = v; break; }
    }

    std::vector<int> stack;
    std::vector<int> circuit;
    stack.push_back(start);

    while (!stack.empty()) {
        int v = stack.back();
        bool moved = false;

        while (ptr[v] < (int)adj[v].size()) {
            int u = adj[v][ptr[v]].first;
            int id = adj[v][ptr[v]].second;
            ptr[v]++;

            if (!edges[id].used) {
                edges[id].used = true;
                stack.push_back(u);
                moved = true;
                break;
            }
        }

        if (!moved) {
            circuit.push_back(stack.back());
            stack.pop_back();
        }
    }

    // вершины выталкивались со стека в обратном порядке
    std::vector<int> result;
    for (int i = (int)circuit.size() - 1; i >= 0; i--) {
        result.push_back(circuit[i]);
    }

    return result;
}

void Graph::printEulerResult() const {
    if (vertexCount == 0) {
        std::cout << "Сначала необходимо сгенерировать граф.\n";
        return;
    }

    this->printUndirected();

    std::vector<EulerEdge> edges = buildEulerEdges();

    if (edges.empty()) {
        std::cout << "\nВ графе нет рёбер — эйлеров цикл не существует.\n";
        return;
    }

    if (!connectedWithout(edges, -1)) {
        std::cout << "\nГраф несвязный — эйлерова цикла не существует "
                     "ни в исходном графе, ни после изменения степеней.\n";
        return;
    }

    std::vector<int> degrees = eulerDegrees(edges);

    std::cout << "\n===== ЭЙЛЕРОВ ЦИКЛ (лаб. 5, часть 1) =====\n";
    std::cout << "  Степени вершин:\n";
    for (int v = 0; v < vertexCount; v++) {
        std::cout << "    deg(" << v + 1 << ") = " << degrees[v] << "\n";
    }

    std::vector<int> odd;
    for (int v = 0; v < vertexCount; v++) {
        if (degrees[v] % 2 != 0) odd.push_back(v);
    }

    std::cout << "------------------------------------------\n";

    if (odd.empty()) {
        std::cout << "  Граф является эйлеровым (все степени чётные).\n";
    } else {
        std::cout << "  Граф НЕ является эйлеровым.\n";
        std::cout << "  Вершины нечётной степени: ";
        for (size_t i = 0; i < odd.size(); i++) std::cout << odd[i] + 1 << " ";
        std::cout << "\n------------------------------------------\n";

        makeEulerian(edges);
    }

    std::cout << "\n  Построение эйлерова цикла (алгоритм Хирхольцера):\n";

    std::vector<int> circuit = hierholzer(edges);

    if (circuit.size() < 2) {
        std::cout << "  Цикл построить не удалось.\n";
        return;
    }

    std::cout << "  Рёбер в графе: " << edges.size()
              << ", рёбер в цикле: " << circuit.size() - 1 << "\n";
    std::cout << "  ";
    for (size_t i = 0; i < circuit.size(); i++) {
        if (i) std::cout << " -> ";
        std::cout << circuit[i] + 1;
    }
    std::cout << "\n==========================================\n";
}