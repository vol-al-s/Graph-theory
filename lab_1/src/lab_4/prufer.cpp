#include "../header/general.h"
#include <iostream>
#include <vector>
#include <set>
#include <algorithm>


void Graph::encodePrufer(const std::vector<MstEdge>& mstEdges,
                         std::vector<int>& codeVertices,
                         std::vector<int>& codeWeights,
                         MstEdge& lastEdge) const {
    codeVertices.clear();
    codeWeights.clear();

    if (vertexCount <= 1) return;
    if ((int)mstEdges.size() != vertexCount - 1) return;

    int n = vertexCount;

    std::vector<std::vector<std::pair<int,int>>> adj(n);
    for (const MstEdge& e : mstEdges) {
        adj[e.u].push_back({e.v, e.weight});
        adj[e.v].push_back({e.u, e.weight});
    }

    std::vector<int> deg(n, 0);
    for (int v = 0; v < n; v++) deg[v] = (int)adj[v].size();

    std::vector<bool> removed(n, false);

    for (int step = 0; step < n - 2; step++) {
        int leaf = -1;
        for (int v = 0; v < n; v++) {
            if (!removed[v] && deg[v] == 1) { leaf = v; break; }
        }
        if (leaf == -1) break;

        int neighbor = -1, weight = 0;
        for (const auto& pr : adj[leaf]) {
            if (!removed[pr.first]) { neighbor = pr.first; weight = pr.second; break; }
        }
        if (neighbor == -1) break;

        codeVertices.push_back(neighbor);
        codeWeights.push_back(weight);

        removed[leaf] = true;
        deg[neighbor]--;
        deg[leaf] = 0;
    }

    // Две оставшиеся непомеченные вершины образуют последнее ребро,
    // которое в код Прюфера не входит по определению.
    int a = -1, b = -1;
    for (int v = 0; v < n; v++) {
        if (!removed[v]) {
            if (a == -1) a = v; else b = v;
        }
    }
    int lastWeight = 0;
    for (const auto& pr : adj[a]) {
        if (pr.first == b) { lastWeight = pr.second; break; }
    }
    lastEdge = {std::min(a, b), std::max(a, b), lastWeight};
}

std::vector<Graph::MstEdge> Graph::decodePrufer(
    const std::vector<int>& codeVertices,
    const std::vector<int>& codeWeights,
    const MstEdge& lastEdge) const {

    std::vector<MstEdge> result;
    if (vertexCount <= 1) return result;
    if (codeVertices.size() != codeWeights.size()) return result;
    if ((int)codeVertices.size() != vertexCount - 2) return result;

    int n = vertexCount;
    int p1 = (int)codeVertices.size();

    std::set<int> B;
    for (int v = 0; v < n; v++) B.insert(v);

    std::vector<int> count(n, 0);
    for (int v : codeVertices) count[v]++;

    for (int i = 0; i < p1; i++) {
   
        int v = -1;
        for (int candidate : B) {
            if (count[candidate] == 0) { v = candidate; break; }
        }
        if (v == -1) break;

        int neighbor = codeVertices[i];
        int weight   = codeWeights[i];

        result.push_back({std::min(v, neighbor), std::max(v, neighbor), weight});

        B.erase(v);
        count[neighbor]--;
    }

    result.push_back(lastEdge);
    return result;
}

void Graph::printPruferResult() const {
    if (vertexCount == 0) {
        std::cout << "Сначала необходимо сгенерировать граф.\n";
        return;
    }
    if (weightMatrix.size() != vertexCount) {
        std::cout << "Сначала необходимо сгенерировать весовую матрицу.\n";
        return;
    }

    long long total = 0;
    bool connected = false;
    std::vector<MstEdge> mst = kruskalMST(total, connected);

    if (!connected) {
        std::cout << "Граф несвязный — кодировать нечего.\n";
        return;
    }

    std::cout << "\nИсходный минимальный остов:\n";
    for (const MstEdge& e : mst) {
        std::cout << "  " << e.u + 1 << " -- " << e.v + 1
                  << "   вес = " << e.weight << "\n";
    }

    // Кодирование
    std::vector<int> codeV, codeW;
    MstEdge lastEdge;
    encodePrufer(mst, codeV, codeW, lastEdge);

    std::cout << "\nКод Прюфера (вершины): ";
    for (size_t i = 0; i < codeV.size(); i++) {
        std::cout << codeV[i] + 1;
        if (i + 1 < codeV.size()) std::cout << ", ";
    }
    std::cout << "\nВеса соответствующих рёбер: ";
    for (size_t i = 0; i < codeW.size(); i++) {
        std::cout << codeW[i];
        if (i + 1 < codeW.size()) std::cout << ", ";
    }
    std::cout << "\n";

    // Декодирование
    std::vector<MstEdge> restored = decodePrufer(codeV, codeW, lastEdge);;

    std::cout << "\nДерево, восстановленное из кода Прюфера:\n";
    for (const MstEdge& e : restored) {
        std::cout << "  " << e.u + 1 << " -- " << e.v + 1
                  << "   вес = " << e.weight << "\n";
    }

    // Сверка
    auto normalize = [](std::vector<MstEdge>& es){
        for (auto& e : es) if (e.u > e.v) std::swap(e.u, e.v);
        std::sort(es.begin(), es.end(),
                  [](const MstEdge& a, const MstEdge& b){
                      if (a.u != b.u) return a.u < b.u;
                      if (a.v != b.v) return a.v < b.v;
                      return a.weight < b.weight;
                  });
    };
    std::vector<MstEdge> a = mst, b = restored;
    normalize(a); normalize(b);

    bool equal = (a.size() == b.size());
    for (size_t i = 0; equal && i < a.size(); i++) {
        if (a[i].u != b[i].u || a[i].v != b[i].v || a[i].weight != b[i].weight)
            equal = false;
    }
    std::cout << "\nПроверка: восстановленное дерево "
              << (equal ? "ИДЕНТИЧНО исходному." : "ОТЛИЧАЕТСЯ от исходного.")
              << "\n";
}