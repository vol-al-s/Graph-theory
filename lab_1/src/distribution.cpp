#include "../header/distribution.h"
#include <cstdlib>

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
    int failures = 0; // провалы
    int successes = 0; // победы

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