#include "Graph.hpp"
#include <queue>
#include <limits>
#include <iostream>
#include <cmath>
#include <algorithm>

std::vector<std::pair<int, double>> KNN_ShortestPath(const Graph &g, int source, int k) {
    const double INF = std::numeric_limits<double>::infinity();
    std::unordered_map<int, double> dist;

    // Initialize distances
    for (auto &[id, node] : g.nodes)
        dist[id] = INF;

    dist[source] = 0.0;

    using P = std::pair<double, int>; // (distance, node)
    std::priority_queue<P, std::vector<P>, std::greater<P>> pq;
    pq.push({0.0, source});

    while (!pq.empty()) {
        auto [d, u] = pq.top();
        pq.pop();
        if (d > dist[u]) continue;

        for (auto &[v, w] : g.adj.at(u)) {
            if (dist[u] + w < dist[v]) {
                dist[v] = dist[u] + w;
                pq.push({dist[v], v});
            }
        }
    }

    // Collect distances
    std::vector<std::pair<int, double>> res;
    for (auto &[id, d] : dist) {
        if (id == source || d == INF) continue;
        res.push_back({id, d});
    }

    std::sort(res.begin(), res.end(),
              [](auto &a, auto &b) { return a.second < b.second; });

    if ((int)res.size() > k)
        res.resize(k);

    return res;
}
