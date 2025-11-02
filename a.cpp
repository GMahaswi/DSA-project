#include "Graph.hpp"
#include <algorithm>
#include <iostream>

std::vector<std::pair<int, double>> KNN_Euclidean(const Graph &g, int source, int k) {
    std::vector<std::pair<int, double>> distances;

    for (auto &[id, node] : g.nodes) {
        if (id == source) continue;
        double dist = g.euclideanDistance(source, id);
        distances.push_back({id, dist});
    }

    std::sort(distances.begin(), distances.end(),
              [](auto &a, auto &b) { return a.second < b.second; });

    if ((int)distances.size() > k)
        distances.resize(k);

    return distances;
}
