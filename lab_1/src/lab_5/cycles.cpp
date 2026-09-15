#include "../header/general.h"
#include <iostream>
#include <vector>
#include <queue>
#include <set>

// ============================================================
//   ЛАБОРАТОРНАЯ 5, ЧАСТЬ 2: ФУНДАМЕНТАЛЬНАЯ СИСТЕМА ЦИКЛОВ
// ============================================================

// Ребро всегда храним как (меньшая вершина, большая).
// Без этого 2-5 и 5-2 считались бы разными рёбрами и симметрическая
// разность работала бы неправильно.
std::pair<int, int> Graph::normalizeEdge(int u, int v) {
    if (u < v) return std::make_pair(u, v);
    return std::make_pair(v, u);
}

// Симметрическая разность: рёбра, входящие ровно в одно из двух множеств.
// Общие рёбра взаимно уничтожаются — это сложение по модулю 2.
EdgeSet Graph::symmetricDifference(const EdgeSet& a, const EdgeSet& b) {
    EdgeSet result;

    for (EdgeSet::const_iterator it = a.begin(); it != a.end(); ++it) {
        if (b.count(*it) == 0) result.insert(*it);
    }
    for (EdgeSet::const_iterator it = b.begin(); it != b.end(); ++it) {
        if (a.count(*it) == 0) result.insert(*it);
    }

    return result;
}

void Graph::printEdgeSet(const EdgeSet& edges) {
    std::cout << "{ ";

    bool first = true;
    for (EdgeSet::const_iterator it = edges.begin(); it != edges.end(); ++it) {
        if (!first) std::cout << ", ";
        std::cout << it->first + 1 << "-" << it->second + 1;
        first = false;
    }

    std::cout << " }";
}

// Путь между двумя вершинами по рёбрам остова.
// Остов — дерево, поэтому путь между любыми двумя вершинами единственный,
// и обычный обход в ширину его находит: идём от src, запоминаем предков,
// потом разворачиваем цепочку предков от dst обратно.
std::vector<int> Graph::pathInTree(const std::vector<MstEdge>& mstEdges,
                                   int src, int dst) const {
    std::vector<std::vector<int> > adj(vertexCount);

    for (size_t i = 0; i < mstEdges.size(); i++) {
        adj[mstEdges[i].u].push_back(mstEdges[i].v);
        adj[mstEdges[i].v].push_back(mstEdges[i].u);
    }

    std::vector<int> parent(vertexCount, -1);
    std::vector<bool> visited(vertexCount, false);

    std::queue<int> q;
    q.push(src);
    visited[src] = true;

    while (!q.empty()) {
        int v = q.front();
        q.pop();
        for (size_t k = 0; k < adj[v].size(); k++) {
            int u = adj[v][k];
            if (!visited[u]) {
                visited[u] = true;
                parent[u] = v;
                q.push(u);
            }
        }
    }

    std::vector<int> reversed;
    for (int v = dst; v != -1; v = parent[v]) reversed.push_back(v);

    if (reversed.empty() || reversed[reversed.size() - 1] != src) {
        return std::vector<int>();  // вершины в разных компонентах
    }

    std::vector<int> path;
    for (int i = (int)reversed.size() - 1; i >= 0; i--) path.push_back(reversed[i]);

    return path;
}

// Фундаментальная система циклов.
// Рёбра графа, не вошедшие в остов, называются хордами.
// Добавление одной хорды к дереву даёт ровно один цикл:
// сама хорда + единственный путь по дереву между её концами.
// Число таких циклов = m - (n - 1) — цикломатическое число графа.
std::vector<EdgeSet> Graph::buildFundamentalCycles(
        const std::vector<MstEdge>& mstEdges) const {

    std::set<std::pair<int, int> > treeEdges;
    for (size_t i = 0; i < mstEdges.size(); i++) {
        treeEdges.insert(normalizeEdge(mstEdges[i].u, mstEdges[i].v));
    }

    std::vector<EdgeSet> cycles;

    for (int i = 0; i < vertexCount; i++) {
        for (int j = i + 1; j < vertexCount; j++) {
            if (undirectedMatrix.at(i, j) != 1) continue;
            if (treeEdges.count(normalizeEdge(i, j)) != 0) continue;  // это ребро остова

            std::vector<int> path = pathInTree(mstEdges, i, j);
            if (path.empty()) continue;

            EdgeSet cycle;
            for (size_t k = 0; k + 1 < path.size(); k++) {
                cycle.insert(normalizeEdge(path[k], path[k + 1]));
            }
            cycle.insert(normalizeEdge(i, j));

            cycles.push_back(cycle);
        }
    }

    return cycles;
}

// Печатает остов и фундаментальную систему циклов,
// а заодно возвращает её вызывающему коду, чтобы не считать дважды.
// Пустой результат означает "дальше работать не с чем".
std::vector<EdgeSet> Graph::printFundamentalCyclesResult() const {
    std::vector<EdgeSet> empty;

    if (vertexCount == 0) {
        std::cout << "Сначала необходимо сгенерировать граф.\n";
        return empty;
    }
    if (weightMatrix.size() != vertexCount) {
        std::cout << "Сначала необходимо сгенерировать весовую матрицу "
                     "(остов строится по ней).\n";
        return empty;
    }

    long long total = 0;
    bool connected = false;
    std::vector<MstEdge> mst = kruskalMST(total, connected);

    if (!connected) {
        std::cout << "Граф несвязный — остова не существует.\n";
        return empty;
    }

    std::cout << "\n===== ФУНДАМЕНТАЛЬНАЯ СИСТЕМА ЦИКЛОВ (лаб. 5, часть 2) =====\n";
    std::cout << "  Рёбра минимального остова (Краскал):\n";
    for (size_t i = 0; i < mst.size(); i++) {
        std::cout << "    " << mst[i].u + 1 << " -- " << mst[i].v + 1
                  << "   вес = " << mst[i].weight << "\n";
    }
    std::cout << "  Суммарный вес остова: " << total << "\n";

    int edgeCount = 0;
    for (int i = 0; i < vertexCount; i++) {
        for (int j = i + 1; j < vertexCount; j++) {
            if (undirectedMatrix.at(i, j) == 1) edgeCount++;
        }
    }

    std::vector<EdgeSet> cycles = buildFundamentalCycles(mst);

    std::cout << "------------------------------------------------------------\n";

    if (cycles.empty()) {
        std::cout << "  Хорд нет — граф сам является деревом, циклов не существует.\n";
        std::cout << "============================================================\n";
        return empty;
    }

    std::cout << "  Рёбер в графе m = " << edgeCount
              << ", вершин n = " << vertexCount << "\n";
    std::cout << "  Число фундаментальных циклов = m - (n - 1) = "
              << cycles.size() << "\n";
    std::cout << "------------------------------------------------------------\n";

    for (size_t i = 0; i < cycles.size(); i++) {
        std::cout << "  C" << i + 1 << ": ";
        printEdgeSet(cycles[i]);
        std::cout << "\n";
    }
    std::cout << "============================================================\n";

    return cycles;
}