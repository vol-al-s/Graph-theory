#include "../header/general.h"
#include <iostream>
#include <queue>
#include <limits>
#include <cstdlib>
#include <iomanip>
#include <functional>

//const int INF = 1000000000;
    //--------------------------------------
    //          часть 3
    //--------------------------------------


//строим матрицу весов
void Graph::generateWeightMatrix(const PascalDistribution& distribution, int mode) {
    weightMatrix.resize(vertexCount); //генерируем матрицу нужного размера 

    // Сначала считаем, что путей нигде нет.
    // Поэтому во все ячейки ставим INF.
    for (int i = 0; i < vertexCount; i++) {
        for (int j = 0; j < vertexCount; j++) {
            weightMatrix.at(i, j) = INF;
        }
    }

    // Генерируем веса только для существующих дуг ориентированного графа
    for (int i = 0; i < vertexCount; i++) {
        for (int j = 0; j < vertexCount; j++) {
            if (orientedMatrix.at(i, j) == 1) {
                // Получаем положительное число по распределению Паскаля
                int value = distribution.generate() + 1;

                
                // 0 - только положительные
                // 1 - только отрицательные
                // 2 - смешанные
                if (mode == 0) {
                    weightMatrix.at(i, j) = value;
                } else if (mode == 1) {
                    weightMatrix.at(i, j) = -value;
                } else if (mode == 2) {
                    if (std::rand() % 2 == 0) {
                        weightMatrix.at(i, j) = value;
                    } else {
                        weightMatrix.at(i, j) = -value;
                    }
                } else {
                    // Если режим ошибочный, по умолчанию делаем положительный вес
                    weightMatrix.at(i, j) = value;
                }
            }
        }
    }
}

//возвращение матрицы весов
const Matrix& Graph::getWeightMatrix() const {
    return weightMatrix;
}

//вывод матрицы весов
void Graph::printWeightMatrix() const {
    std::cout << "\nВесовая матрица:\n\n    ";
    for (int j = 0; j < vertexCount; j++) {
        if (j == 0) {
            std::cout << std::setw(1) << "| " << std::setw(4) << "v" <<j+1;
        } else {
            std::cout << std::setw(5) << "v" <<j+1;
        }
    }
    std::cout<<std::endl;
    for (int k = 0; k < (6+(vertexCount * (5+1))); k++) {
        std::cout << "-";
    }
    std::cout << "\n";

    for (int i = 0; i < vertexCount; i++) {
        std::cout << std::setw(2) << "v" << i+1 << " |";
        for (int j = 0; j < vertexCount; j++) {
            if (weightMatrix.at(i, j) == INF) {
                std::cout << std::setw(6) << ".";
            } else {
                std::cout << std::setw(6) << weightMatrix.at(i, j);
            }
        }
        std::cout << "\n";
    }
}

//Один шаг для минимального пути
Matrix Graph::shimbellStepMin(const Matrix& left, const Matrix& right) const {
    // Результирующая матрица
    Matrix result(vertexCount);

    // Изначально считаем, что пути между любыми вершинами нет
    for (int i = 0; i < vertexCount; i++) {
        for (int j = 0; j < vertexCount; j++) {
            result.at(i, j) = INF;
        }
    }

    // Формула:
    // result[i][j] = min_k ( left[i][k] + right[k][j] )
    for (int i = 0; i < vertexCount; i++) {
        for (int j = 0; j < vertexCount; j++) {
            for (int k = 0; k < vertexCount; k++) {
                // Если одного из путей нет, такой вариант не рассматриваем
                if (left.at(i, k) == INF || right.at(k, j) == INF) {
                    continue;
                }

                int candidate = left.at(i, k) + right.at(k, j);

                if (candidate < result.at(i, j)) {
                    result.at(i, j) = candidate;
                }
            }
        }
    }

    return result;
}

Matrix Graph::shimbellStepMax(const Matrix& left, const Matrix& right) const {
    // Для максимального пути тоже используем INF
    Matrix result(vertexCount);

    for (int i = 0; i < vertexCount; i++) {
        for (int j = 0; j < vertexCount; j++) {
            result.at(i, j) = INF;
        }
    }

    // Формула:
    // result[i][j] = max_k ( left[i][k] + right[k][j] )
    for (int i = 0; i < vertexCount; i++) {
        for (int j = 0; j < vertexCount; j++) {
            bool found = false;

            for (int k = 0; k < vertexCount; k++) {
                if (left.at(i, k) == INF || right.at(k, j) == INF) {
                    continue;
                }

                int candidate = left.at(i, k) + right.at(k, j);

                if (!found || candidate > result.at(i, j)) {
                    result.at(i, j) = candidate;
                    found = true;
                }
            }
        }
    }

    return result;
}

Matrix Graph::shimbellMin(int edgesCount) const {
    // Если пользователь ввёл некорректное число рёбер,
    // просто возвращаем текущую весовую матрицу
    if (edgesCount <= 1) {
        return weightMatrix;
    }

    // Для путей длины 1 ребро матрица уже есть: это weightMatrix
    Matrix result = weightMatrix;

    // Каждый следующий шаг увеличивает длину пути ровно на 1 ребро
    for (int step = 2; step <= edgesCount; step++) {
        result = shimbellStepMin(result, weightMatrix);
    }

    return result;
}

Matrix Graph::shimbellMax(int edgesCount) const {
    if (edgesCount <= 1) {
        return weightMatrix;
    }

    Matrix result = weightMatrix;

    for (int step = 2; step <= edgesCount; step++) {
        result = shimbellStepMax(result, weightMatrix);
    }

    return result;
}

void Graph::printShimbellResult(int edgesCount, bool findMin, bool findMax) const {
    (*this).printWeightMatrix();
        if (edgesCount == 0) {
        Matrix minMatrix = shimbellMin(edgesCount);

        std::cout << "\nМатрица минимальных путей методом Шимбелла"
                << " для длины " << edgesCount << " рёбер:\n\n    ";

        for (int j = 0; j < vertexCount; j++) {
            if (j == 0) {
                std::cout << std::setw(1) << "| " << std::setw(4) << "v" <<j+1;
            } else {
                std::cout << std::setw(5) << "v" <<j+1;
            }
        }
        std::cout<<std::endl;
        for (int k = 0; k < (6+(vertexCount * (5+1))); k++) {
            std::cout << "-";
        }
        std::cout << "\n";

        for (int i = 0; i < vertexCount; i++) {
            std::cout << std::setw(2) << "v" << i+1 << " |";
            for (int j = 0; j < vertexCount; j++) {
                if (i != j) {
                    std::cout << std::setw(6) << ".";
                } else {
                    std::cout << std::setw(6) << 0;
                }
            }
            std::cout << "\n";
        }
    }
    if (edgesCount > 0) {
        if (findMin) {
            Matrix minMatrix = shimbellMin(edgesCount);

            std::cout << "\nМатрица минимальных путей методом Шимбелла"
                    << " для длины " << edgesCount << " рёбер:\n\n    ";

            for (int j = 0; j < vertexCount; j++) {
                if (j == 0) {
                    std::cout << std::setw(1) << "| " << std::setw(4) << "v" <<j+1;
                } else {
                    std::cout << std::setw(5) << "v" <<j+1;
                }
            }
            std::cout<<std::endl;
            for (int k = 0; k < (6+(vertexCount * (5+1))); k++) {
                std::cout << "-";
            }
            std::cout << "\n";

            for (int i = 0; i < vertexCount; i++) {
                std::cout << std::setw(2) << "v" << i+1 << " |";
                for (int j = 0; j < vertexCount; j++) {
                    if (minMatrix.at(i, j) == INF) {
                        std::cout << std::setw(6) << ".";
                    } else {
                        std::cout << std::setw(6) << minMatrix.at(i, j);
                    }
                }
                std::cout << "\n";
            }
        }

        if (findMax) {
            Matrix maxMatrix = shimbellMax(edgesCount);

            std::cout << "\nМатрица максимальных путей методом Шимбелла"
                    << " для длины " << edgesCount << " рёбер:\n\n    ";

            for (int j = 0; j < vertexCount; j++) {
                if (j == 0) {
                    std::cout << std::setw(1) << "| " << std::setw(4) << "v" <<j+1;
                } else {
                    std::cout << std::setw(5) << "v" <<j+1;
                }
            }
            std::cout<<std::endl;
            for (int k = 0; k < (6+(vertexCount * (5+1))); k++) {
                std::cout << "-";
            }
            std::cout << "\n";

            for (int i = 0; i < vertexCount; i++) {
                std::cout << std::setw(2) << "v" << i+1 << " |";
                for (int j = 0; j < vertexCount; j++) {
                    if (maxMatrix.at(i, j) == INF) {
                        std::cout << std::setw(6) << ".";
                    } else {
                        std::cout << std::setw(6) << maxMatrix.at(i, j);
                    }
                }
                std::cout << "\n";
            }
        }
    }

}


int Graph::findMaxPathLengthOriented() const {
    if (vertexCount == 0) {
        return 0;
    }

    std::vector<int> memo(vertexCount, -1);

    std::function<int(int)> dfsLongest = [&](int v) -> int {
        if (memo[v] != -1) {
            return memo[v];
        }

        int best = 0;

        for (int u = 0; u < vertexCount; u++) {
            if (orientedMatrix.at(v, u) == 1) {
                best = std::max(best, 1 + dfsLongest(u));
            }
        }

        memo[v] = best;
        return best;
    };

    int answer = 0;

    for (int v = 0; v < vertexCount; v++) {
        answer = std::max(answer, dfsLongest(v));
    }

    return answer;
}
