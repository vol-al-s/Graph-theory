#include "../header/distribution.h"
#include <cstdlib>
#include <algorithm>

PascalDistribution::PascalDistribution(int r, double p) : r(r), p(p) {}

void PascalDistribution::setR(int r) {
    this->r = r;
}

void PascalDistribution::setP(double p) {
    this->p = p;
}

int PascalDistribution::getR() const {
    return r;
}

double PascalDistribution::getP() const {
    return p;
}

int PascalDistribution::generate() const {
    int failures = 0;
    int successes = 0;

    while (successes < r) {
        double u = (double)std::rand() / RAND_MAX;

        if (u < p) {
            successes++;
        } else {
            failures++;
        }
    }

    return failures;
}

std::vector<int> PascalDistribution::generateOutDegreeSequence(int n) const {
    std::vector<int> outDegrees(n, 0);

    if (n <= 1) {
        return outDegrees;
    }

    // Для последней вершины исходящая степень всегда 0
    outDegrees[n - 1] = 0;

    for (int i = 0; i < n - 1; i++) {
        int maxOut = n - i - 1;

        // Генерируем число по Паскалю
        int value = generate();

        // Приводим к допустимому диапазону
        // Хотим, чтобы вершины 0..n-2 имели хотя бы 1 исходящую дугу,
        // чтобы можно было гарантированно построить связную цепочку.
        value = value % maxOut;
        value += 1;

        if (value > maxOut) {
            value = maxOut;
        }

        outDegrees[i] = value;
    }

    return outDegrees;
}