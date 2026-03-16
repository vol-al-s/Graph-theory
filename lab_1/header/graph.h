#ifndef GRAPH_H
#define GRAPH_H

#include <vector>

using Matrix = std::vector<std::vector<int>>;
const int INF = 1e9;

class Graph {
public:
    int n;
    bool is_directed;
    Matrix adj;

    Graph(int n, bool is_directed);
    void generate_acyclic_connected(int r, double p);
    void print();
    
    // Проверка связности (Следствие 1 из презентации)
    bool is_connected();
    
    // Подсчет эксцентриситетов, центров и диаметра
    void calculate_eccentricities();
    
    // Подсчет маршрутов методом Шимбелла
    void count_paths(int u, int v);
};

class Shimbell {
public:
    int n;
    Matrix W;

    Shimbell(int n);
    void generate_weights(int edges, int r, double p, int mode);
    void print_matrix(const Matrix& m);
    void find_shortest_paths();
    void find_longest_paths();
};

// Вспомогательные матричные операции
Matrix matrix_multiply(const Matrix& A, const Matrix& B);
Matrix matrix_add(const Matrix& A, const Matrix& B);

#endif
