#include <iostream>
#include <ctime>
#include <limits>
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

int get_valid_input(const string& prompt) {
    int value;
    while (true) {
        cout << prompt;
        if (cin >> value) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return value;
        } else {
            cout << "Ошибка: введено некорректное значение. Пожалуйста, введите целое число.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    }
}

int main() {
    srand(time(NULL));
    int choice;
    Graph* g = nullptr;
    
    int r = 3;      
    double p = 0.5;

    while (true) {
        print_menu();
        
        if (!(cin >> choice)) {
            cout << "Ошибка: введите число от 0 до 4.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }
        
        if (choice == 0) break;
        
        if (choice == 1) {
            int n, graph_type;
            while (true) {
                n = get_valid_input("Введите количество вершин графа (n > 1): ");
                if (n > 1) break;
                cout << "Граф должен содержать как минимум 2 вершины.\n";
            }
            
            while (true) {
                graph_type = get_valid_input("Тип графа (1 - Ориентированный, 2 - Неориентированный): ");
                if (graph_type == 1 || graph_type == 2) break;
                cout << "Пожалуйста, введите 1 или 2.\n";
            }
            
            bool is_directed = (graph_type == 1);
            
            if (g) delete g;
            g = new Graph(n, is_directed);
            g->generate_acyclic_connected(r, p);
            
            if (is_directed) {
                cout << "\nСгенерирована матрица смежности (Ориентированный ациклический граф):\n";
            } else {
                cout << "\nСгенерирована матрица смежности (Неориентированное дерево):\n";
            }
            g->print();
        } 
        else if (choice == 2) {
            if (!g) { cout << "Ошибка: Сначала выполните пункт 1!\n"; continue; }
            g->calculate_eccentricities();
        } 
        else if (choice == 3) {
            int n, edges, mode;
            while (true) {
                n = get_valid_input("Введите количество вершин для генерации весовой матрицы (n > 1): ");
                if (n > 1) break;
                cout << "Матрица должна содержать как минимум 2 вершины.\n";
            }
            
            int max_edges = n * (n - 1); 
            while (true) {
                edges = get_valid_input("Введите количество ребер: ");
                if (edges >= 1 && edges <= max_edges) break;
                cout << "Количество ребер для " << n << " вершин должно быть от 1 до " << max_edges << ".\n";
            }
            
            while (true) {
                mode = get_valid_input("Режим (0:положительные, 1:отрицательные, 2:смешанные): ");
                if (mode == 0 || mode == 1 || mode == 2) break;
                cout << "Доступные режимы: 0, 1 или 2.\n";
            }
            
            Shimbell sh(n);
            sh.generate_weights(edges, r, p, mode);
            cout << "\nСгенерированная весовая матрица:\n";
            sh.print_matrix(sh.W);
            
            sh.find_shortest_paths();
            sh.find_longest_paths();
        } 
        else if (choice == 4) {
            if (!g) { cout << "Ошибка: Сначала выполните пункт 1!\n"; continue; }
            int u, v;
            
            while (true) {
                u = get_valid_input("Введите начальную вершину (1-" + to_string(g->n) + "): ");
                if (u >= 1 && u <= g->n) break;
                cout << "Вершина должна быть в диапазоне от 1 до " << g->n << ".\n";
            }
            
            while (true) {
                v = get_valid_input("Введите конечную вершину (1-" + to_string(g->n) + "): ");
                if (v >= 1 && v <= g->n) break;
                cout << "Вершина должна быть в диапазоне от 1 до " << g->n << ".\n";
            }
            
            g->count_paths(u, v);
        }
        else {
            cout << "Неверный пункт меню. Выберите от 0 до 4.\n";
        }
    }
    
    if (g) delete g;
    return 0;
}
