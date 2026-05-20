#ifndef FLOW_H
#define FLOW_H

#include "general.h"

class FlowNetwork {
private:
    int vertexCount;

    Matrix capacity;
    Matrix cost;
    Matrix flow;

public:
    FlowNetwork(int n);

    void buildFromGraph(const Graph& graph);

    void generateCapacityMatrix();
    void generateCostMatrix(int mode);

    void printCapacity() const;
    void printCost() const;

    int fordFulkerson(int source, int sink);

    int minCostFlow(int source, int sink, int requiredFlow);
};

#endif