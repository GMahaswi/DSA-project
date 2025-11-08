#include "graph.hpp"
#include <queue>

std::vector<std::pair<int, double>> KNN_ShortestPath(const Graph &g, int source, int k) {
    const double INF = std::numeric_limits<double>::infinity();
    std::unordered_map<int, double> dist;
    for (auto &[id, node] : g.nodes)
        dist[id] = INF;

    dist[source] = 0.0;

    using P = std::pair<double, int>; // (distance, node)
    std::priority_queue<P, std::vector<P>, std::greater<P>> pq;
    pq.push({0.0, source});

    std::vector<std::pair<int, double>> result; 
    std::unordered_set<int> visited;

    while (!pq.empty() && (int)result.size() < k) {
        auto [d, u] = pq.top();
        pq.pop();

        if (visited.count(u)) continue;
        visited.insert(u);

        if (u != source)
            result.push_back({u, d});

        for (auto &[v, w] : g.adj.at(u)) {
            if (d + w < dist[v]) {
                dist[v] = d + w;
                pq.push({dist[v], v});
            }
        }
    }

    return result;
}
