#include "../header/general.h"
#include <iostream>
#include <vector>
#include <set>
#include <algorithm>

// =============================================================================
//                              КОД ПРЮФЕРА
//
// Источник: tg9.pdf, слайды 29-30.
//
// Кодирование (слайд 29):
//   for i from 1 to p-1 do
//       v := min { k ∈ V | deg(k) = 1 }   { висячая вершина с наим. номером }
//       A[i] := Γ(v)                       { единственный сосед v }
//       V := V - v                          { удаляем v из дерева }
//   end for
//
// Замечание из лекции: "существует всего одно дерево с двумя вершинами — K2,
// а потому информацию о последнем ребре можно не хранить".
// Стандартный код Прюфера имеет длину p-2 (а не p-1!), но в лекции
// используется вариант длины p-1, чтобы упростить декодирование.
// Я следую варианту из лекции — длина кода p-1.
//
// ТРЕБОВАНИЕ ЗАДАНИЯ: "Сохранять веса при кодировании обязательно".
// Поэтому параллельно с массивом A[] (вершин-соседей) формируется массив W[]
// весов рёбер, по которым происходило удаление вершины. Длины массивов
// совпадают, и пара (A[i], W[i]) однозначно описывает удалённое на шаге i ребро.
//
// Декодирование (слайд 30):
//   E := ∅;  B := {1..p}
//   for i from 1 to p-1 do
//       v := min { k ∈ B | ∀ j ≥ i: k ≠ A[j] }
//       E := E + (v, A[i])     { восстановили ребро с весом W[i] }
//       B := B - v
//   end for
//
// На последнем шаге в B остаётся одна вершина — она будет автоматически
// соединена с тем, что осталось от кода (это требует длины p-1).
// =============================================================================

void Graph::encodePrufer(const std::vector<MstEdge>& mstEdges,
                         std::vector<int>& codeVertices,
                         std::vector<int>& codeWeights) const {
    codeVertices.clear();
    codeWeights.clear();

    if (vertexCount <= 1) return;
    if ((int)mstEdges.size() != vertexCount - 1) return;  // не дерево

    int n = vertexCount;

    // Список смежности дерева: для каждой вершины храним пары (сосед, вес ребра).
    std::vector<std::vector<std::pair<int,int>>> adj(n);
    for (const MstEdge& e : mstEdges) {
        adj[e.u].push_back({e.v, e.weight});
        adj[e.v].push_back({e.u, e.weight});
    }

    std::vector<int> deg(n, 0);
    for (int v = 0; v < n; v++) deg[v] = (int)adj[v].size();

    std::vector<bool> removed(n, false);

    // Длина кода = p-1 (по версии из лекции).
    for (int step = 0; step < n - 1; step++) {
        // Найти висячую вершину (deg == 1) с минимальным номером.
        int leaf = -1;
        for (int v = 0; v < n; v++) {
            if (!removed[v] && deg[v] == 1) { leaf = v; break; }
        }
        if (leaf == -1) break;  // защита: не должно случаться для дерева

        // Найти единственного оставшегося соседа листа и вес ребра к нему.
        int neighbor = -1, weight = 0;
        for (const auto& pr : adj[leaf]) {
            if (!removed[pr.first]) { neighbor = pr.first; weight = pr.second; break; }
        }
        if (neighbor == -1) break;

        codeVertices.push_back(neighbor);
        codeWeights.push_back(weight);

        // Удаляем лист.
        removed[leaf] = true;
        deg[neighbor]--;
        deg[leaf] = 0;
    }
}

std::vector<Graph::MstEdge> Graph::decodePrufer(
    const std::vector<int>& codeVertices,
    const std::vector<int>& codeWeights) const {

    std::vector<MstEdge> result;
    if (vertexCount <= 1) return result;
    if (codeVertices.size() != codeWeights.size()) return result;
    if ((int)codeVertices.size() != vertexCount - 1) return result;

    int n = vertexCount;
    int p1 = (int)codeVertices.size();

    // B — множество ещё не использованных номеров вершин.
    // Реализуем std::set, чтобы быстро находить min с условием.
    std::set<int> B;
    for (int v = 0; v < n; v++) B.insert(v);

    // Чтобы быстро проверять "∀ j ≥ i: k ≠ A[j]", удобно поддерживать счётчик
    // оставшихся вхождений каждой вершины в "хвосте" кода (от i до конца).
    // Изначально count[v] = число вхождений v в codeVertices.
    std::vector<int> count(n, 0);
    for (int v : codeVertices) count[v]++;

    for (int i = 0; i < p1; i++) {
        // Найти минимальную из B вершину v, для которой count[v] == 0
        // (т.е. v не встречается в codeVertices[i..p1-1]).
        int v = -1;
        for (int candidate : B) {
            if (count[candidate] == 0) { v = candidate; break; }
        }
        if (v == -1) break;  // защита

        int neighbor = codeVertices[i];
        int weight   = codeWeights[i];

        result.push_back({std::min(v, neighbor), std::max(v, neighbor), weight});

        B.erase(v);
        // Снимаем учёт текущего символа кода из счётчика "хвоста"
        // — после i ребро уже не будет рассмотрено.
        count[neighbor]--;
    }

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
    encodePrufer(mst, codeV, codeW);

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
    std::vector<MstEdge> restored = decodePrufer(codeV, codeW);

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