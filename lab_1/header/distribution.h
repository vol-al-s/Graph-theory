#ifndef DISTRIBUTION_H
#define DISTRIBUTION_H

class PascalDistribution {
private:
    int r; // число успехов
    double p; // вероятность

public:
    PascalDistribution(int r = 3, double p = 0.5);

    void setR(int r);
    void setP(double p);

    int getR() const;
    double getP() const;

    int generate() const;
};

#endif