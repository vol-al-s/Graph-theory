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

    outDegrees[n - 1] = 0;

    for (int i = 0; i < n - 1; i++) {
        int maxOut = n - i - 1;

        int value = generate();

        value = value % maxOut;
        value += 1;

        if (value > maxOut) {
            value = maxOut;
        }

        outDegrees[i] = value;
    }

    return outDegrees;
}