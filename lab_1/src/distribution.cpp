#include "../header/distribution.h"
#include <cstdlib>

int generate_pascal(int r, double p) {
    int failures = 0;
    int successes = 0;
    // Моделируем испытания до достижения r успехов
    while (successes < r) {
        double u = (double)std::rand() / RAND_MAX;
        if (u < p) {
            successes++;
        } else {
            failures++;
        }
    }
    return failures; // Количество неудач имеет распределение Паскаля
}

int generate_weight(int r, double p, int mode) {
    // Добавляем +1, чтобы не генерировать нулевые веса
    int val = generate_pascal(r, p) + 1; 
    
    if (mode == 1) {
        return -val;
    } else if (mode == 2) {
        if (std::rand() % 2 == 0) return val;
        else return -val;
    }
    return val;
}
