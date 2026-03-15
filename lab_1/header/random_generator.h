#ifndef RANDOM_GENERATOR_H
#define RANDOM_GENERATOR_H

#include <random>
#include <vector>

/**
 * Генератор случайных чисел с распределением Паскаля
 * На основе справочника Вадзинского (раздел 2.6.2)
 * 
 * Распределение Паскаля (отрицательное биномиальное):
 * P(w) = C(w-1, m-1) * p^m * (1-p)^(w-m), где w = m, m+1, ...
 * 
 * Параметры:
 * - m: количество успехов (порядок распределения)
 * - p: вероятность успеха
 */
class PascalRandomGenerator {
public:
    PascalRandomGenerator(int m = 2, double p = 0.5);
    
    // Генерация одного случайного числа
    int generate();
    
    // Генерация нескольких чисел
    std::vector<int> generate(int count);
    
    // Установка параметров
    void setParameters(int m, double p);
    int getM() const;
    double getP() const;
    
    // Статистика
    double getExpectedValue() const;
    double getVariance() const;
    
private:
    int m;          // Параметр m (количество успехов)
    double p;       // Вероятность успеха
    std::mt19937 generator;
    
    // Метод обратных функций для генерации
    int inverseTransform();
    
    // Вычисление биномиального коэффициента
    long long binomialCoefficient(int n, int k) const;
    
    // Вычисление вероятности P(X = k)
    double probabilityMass(int k) const;
};

#endif // RANDOM_GENERATOR_H