#include "random_generator.h"
#include <cmath>
#include <stdexcept>

PascalRandomGenerator::PascalRandomGenerator(int m, double p) 
    : m(m), p(p), generator(std::random_device{}()) {
    if (m < 1) throw std::invalid_argument("Параметр m должен быть >= 1");
    if (p <= 0 || p >= 1) throw std::invalid_argument("Вероятность p должна быть в интервале (0, 1)");
}

int PascalRandomGenerator::generate() {
    return inverseTransform();
}

std::vector<int> PascalRandomGenerator::generate(int count) {
    std::vector<int> result;
    result.reserve(count);
    for (int i = 0; i < count; i++) {
        result.push_back(generate());
    }
    return result;
}

void PascalRandomGenerator::setParameters(int m, double p) {
    if (m < 1) throw std::invalid_argument("Параметр m должен быть >= 1");
    if (p <= 0 || p >= 1) throw std::invalid_argument("Вероятность p должна быть в интервале (0, 1)");
    this->m = m;
    this->p = p;
}

int PascalRandomGenerator::getM() const {
    return m;
}

double PascalRandomGenerator::getP() const {
    return p;
}

double PascalRandomGenerator::getExpectedValue() const {
    // M(X) = m/p (по Вадзинскому, раздел 2.6.2)
    return static_cast<double>(m) / p;
}

double PascalRandomGenerator::getVariance() const {
    // D(X) = m(1-p)/p^2 (по Вадзинскому, раздел 2.6.2)
    return static_cast<double>(m) * (1 - p) / (p * p);
}

int PascalRandomGenerator::inverseTransform() {
    // Метод обратных функций (Вадзинский, раздел 1.5)
    std::uniform_real_distribution<double> uniform(0.0, 1.0);
    double u = uniform(generator);
    
    // Находим k такое, что F(k-1) < u <= F(k)
    double cumulativeProbability = 0.0;
    int k = m;
    
    while (true) {
        cumulativeProbability += probabilityMass(k);
        if (u <= cumulativeProbability) {
            return k;
        }
        k++;
        
        // Защита от бесконечного цикла
        if (k > m + 1000) {
            return k;
        }
    }
}

long long PascalRandomGenerator::binomialCoefficient(int n, int k) const {
    if (k < 0 || k > n) return 0;
    if (k == 0 || k == n) return 1;
    
    long long result = 1;
    for (int i = 0; i < k; i++) {
        result = result * (n - i) / (i + 1);
    }
    return result;
}

double PascalRandomGenerator::probabilityMass(int k) const {
    // P(X = k) = C(k-1, m-1) * p^m * (1-p)^(k-m)
    // По формуле из Вадзинского (раздел 2.6.2)
    if (k < m) return 0.0;
    
    long long comb = binomialCoefficient(k - 1, m - 1);
    double prob = comb * std::pow(p, m) * std::pow(1 - p, k - m);
    
    return prob;
}