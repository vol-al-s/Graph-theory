#include "../header/graph.h"
#include "distribution.h"
#include <iostream>
#include <iomanip>
#include <algorithm>

Graph::Graph(int n) : n(n) {
    adj.assign(n, std::vector<int>(n, 0));
}

// Умножение матриц
Matrix matrix_multiply(const Matrix& A, const Matrix& B) {
    int n = A.size();
    Matrix C(n, std::vector<int>(n, 0));
    for(int i = 0; i < n; ++i)
        for(int k = 0; k < n; ++k)
            for(int j = 0; j < n; ++j)
                C[i][j] += A[i][k] * B[k][j];
    return C;
}

// Сложение матриц
Matrix matrix_add(const Matrix& A, const Matrix& B) {
    int n = A.size();
    Matrix C(n, std::vector<int>(n, 0));
    for(int i = 0; i < n; ++i)
        for(int j = 0; j < n; ++j)
            C[i][j] = A[i][j] + B[i][j];
    return C;
}

bool Graph::is_connected() {
    // Для связности графа строим матрицу смежности неориентированного эквивалента
    Matrix P = adj;
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j)
            if (adj[i][j]) { P[i][j] = 1; P[j][i] = 1; }
            
    Matrix sumP = P;
    Matrix currentP = P;
    // Сложение всех степеней матрицы от 1 до n-1
    for (int k = 2; k <= n - 1; ++k) {
        currentP = matrix_multiply(currentP, P);
        sumP = matrix_add(sumP, currentP);
    }
    
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            if (i != j && sumP[i][j] == 0) return false;
        }
    }
    return true;
}

void Graph::generate_acyclic_connected(int r, double p) {
    while (true) {
        for (int i = 0; i < n; ++i) std::fill(adj[i].begin(), adj[i].end(), 0);
        
        for (int i = 0; i < n - 1; ++i) {
            // Генерация по степеням вершин:
            int deg = generate_pascal(r, p) + 1; 
            if (deg > n - 1 - i) deg = n - 1 - i;
            
            std::vector<int> targets;
            for (int j = i + 1; j < n; ++j) targets.push_back(j);
            std::random_shuffle(targets.begin(), targets.end());
            
            for (int k = 0; k < deg; ++k) {
                adj[i][targets[k]] = 1; // Треугольная матрица гарантирует ацикличность
            }
        }
        if (is_connected()) break; // Проверка связности по теореме Шимбелла
    }
}

void Graph::print() {
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) std::cout << adj[i][j] << " ";
        std::cout << "\n";
    }
}

void Graph::calculate_eccentricities() {
    Matrix dist(n, std::vector<int>(n, INF));
    for (int i = 0; i < n; ++i) dist[i][i] = 0;
    
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j)
            if (adj[i][j]) { dist[i][j] = 1; dist[j][i] = 1; }
            
    for (int k = 0; k < n; ++k)
        for (int i = 0; i < n; ++i)
            for (int j = 0; j < n; ++j)
                if (dist[i][k] < INF && dist[k][j] < INF)
                    dist[i][j] = std::min(dist[i][j], dist[i][k] + dist[k][j]);
                    
    std::vector<int> ecc(n, 0);
    int min_ecc = INF, max_ecc = 0;
    
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) if (dist[i][j] != INF) ecc[i] = std::max(ecc[i], dist[i][j]);
        min_ecc = std::min(min_ecc, ecc[i]);
        max_ecc = std::max(max_ecc, ecc[i]);
    }
    
    std::cout << "Эксцентриситеты:\n";
    for (int i = 0; i < n; ++i) std::cout << "v" << i+1 << ": " << ecc[i] << "\n";
    
    std::cout << "Центр графа (вершины): ";
    for (int i = 0; i < n; ++i) if (ecc[i] == min_ecc) std::cout << i+1 << " ";
    
    std::cout << "\nДиаметральные вершины (пары): ";
    for (int i = 0; i < n; ++i)
        for (int j = i + 1; j < n; ++j)
            if (dist[i][j] == max_ecc) std::cout << "(" << i+1 << ", " << j+1 << ") ";
    std::cout << "\n";
}

void Graph::count_paths(int u, int v) {
    u--; v--; // 0-индексация
    Matrix sumP = adj;
    Matrix currentP = adj;
    for (int k = 2; k <= n - 1; ++k) {
        currentP = matrix_multiply(currentP, adj);
        sumP = matrix_add(sumP, currentP);
    }
    int routes = sumP[u][v];
    if (routes > 0) std::cout << "Маршрут возможен. Количество: " << routes << "\n";
    else std::cout << "Построение маршрута невозможно.\n";
}

Shimbell::Shimbell(int n) : n(n) {
    W.assign(n, std::vector<int>(n, INF));
    for(int i = 0; i < n; ++i) W[i][i] = 0;
}

void Shimbell::generate_weights(int edges, int r, double p, int mode) {
    int added = 0;
    while(added < edges) {
        int u = std::rand() % n;
        int v = std::rand() % n;
        if (u != v && W[u][v] == INF) {
            W[u][v] = generate_weight(r, p, mode);
            added++;
        }
    }
}

void Shimbell::print_matrix(const Matrix& m) {
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            if (m[i][j] >= INF/2) std::cout << std::setw(5) << "INF";
            else if (m[i][j] <= -INF/2) std::cout << std::setw(5) << "-INF";
            else std::cout << std::setw(5) << m[i][j];
        }
        std::cout << "\n";
    }
}

void Shimbell::find_shortest_paths() {
    Matrix currentW = W;
    for (int step = 2; step <= n - 1; ++step) {
        Matrix nextW = currentW;
        for (int i = 0; i < n; ++i)
            for (int j = 0; j < n; ++j)
                for (int k = 0; k < n; ++k)
                    if (currentW[i][k] != INF && W[k][j] != INF)
                        nextW[i][j] = std::min(nextW[i][j], currentW[i][k] + W[k][j]);
        currentW = nextW;
    }
    std::cout << "\nМатрица минимальных путей:\n";
    print_matrix(currentW);
}

void Shimbell::find_longest_paths() {
    Matrix currentW = W;
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j)
            if (currentW[i][j] == INF) currentW[i][j] = -INF;
            
    Matrix baseW = currentW;
    for (int step = 2; step <= n - 1; ++step) {
        Matrix nextW = currentW;
        for (int i = 0; i < n; ++i)
            for (int j = 0; j < n; ++j)
                for (int k = 0; k < n; ++k)
                    if (currentW[i][k] != -INF && baseW[k][j] != -INF)
                        nextW[i][j] = std::max(nextW[i][j], currentW[i][k] + baseW[k][j]);
        currentW = nextW;
    }
    
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j)
            if (currentW[i][j] <= -INF/2) currentW[i][j] = INF;
            
    std::cout << "\nМатрица максимальных путей:\n";
    print_matrix(currentW);
}
