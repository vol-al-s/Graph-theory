#include <iostream>
#include <ctime>
#include "../header/graph.h"

using namespace std;

void print_menu() {
    cout << "\n=== ЛАБОРАТОРНАЯ РАБОТА № 1 ===" << endl;
    cout << "1. Сформировать связный ациклический граф" << endl;
    cout << "2. Посчитать эксцентриситеты, центры и диаметральные вершины" << endl;
    cout << "3. Метод Шимбелла (минимальные и максимальные пути)" << endl;
    cout << "4. Найти возможность и количество маршрутов от А до B" << endl;
    cout << "0. Выход" << endl;
    cout << "Ваш выбор: ";
}

int main() {
    srand(time(NULL));
    int choice;
    Graph* g = nullptr;
    
    // Параметры распределения Паскаля 
    int r = 3;      
    double p = 0.5;

    while (true) {
        print_menu();
        cin >> choice;
        
        if (choice == 0) break;
        
        if (choice == 1) {
            int n;
            cout << "Введите количество вершин графа: ";
            cin >> n;
            if (g) delete g;
            g = new Graph(n);
            g->generate_acyclic_connected(r, p);
            
            cout << "Сгенерированная матрица смежности:\n";
            g->print();
        } 
        else if (choice == 2) {
            if (!g) { cout << "Сначала выполните пункт 1!\n"; continue; }
            g->calculate_eccentricities();
        } 
        else if (choice == 3) {
            int n, edges, mode;
            cout << "Введите количество вершин для генерации весовой матрицы: ";
            cin >> n;
            cout << "Введите количество ребер: ";
            cin >> edges;
            cout << "Режим (0:положительные, 1:отрицательные, 2:смешанные): ";
            cin >> mode;
            
            Shimbell sh(n);
            sh.generate_weights(edges, r, p, mode);
            cout << "\nСгенерированная весовая матрица:\n";
            sh.print_matrix(sh.W);
            
            sh.find_shortest_paths();
            sh.find_longest_paths();
        } 
        else if (choice == 4) {
            if (!g) { cout << "Сначала выполните пункт 1!\n"; continue; }
            int u, v;
            cout << "Введите начальную вершину (1-" << g->n << "): ";
            cin >> u;
            cout << "Введите конечную вершину (1-" << g->n << "): ";
            cin >> v;
            g->count_paths(u, v);
        }
    }
    
    if (g) delete g;
    return 0;
}
