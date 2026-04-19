#ifndef DISTRIBUTION_H
#define DISTRIBUTION_H

#include <vector>

class PascalDistribution {
private:
    int r;      // число успехов
    double p;   // вероятность успеха

public:
    PascalDistribution(int r = 3, double p = 0.5);

    void setR(int r);
    void setP(double p);

    int getR() const;
    double getP() const;

    // Одно число по распределению Паскаля
    int generate() const;

    // Новый метод:
    // генерирует вектор исходящих степеней для DAG
    std::vector<int> generateOutDegreeSequence(int n) const;
};

#endif