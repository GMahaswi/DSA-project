#include "knn.hpp"
using namespace std;

/*
    KNN based on Euclidean distance between coordinates.
*/
vector<pair<int, double>> KNN_Euclidean(const Graph& g, int source, int k) {
    vector<pair<int, double>> distances;

    if (!g.coords.count(source)) {
        cerr << "Error: Source node not found in graph.\n";
        return distances;
    }

    auto [lat1, lon1] = g.coords.at(source);

    for (const auto& [id, coord] : g.coords) {
        if (id == source) continue;
        double dist = sqrt(pow(lat1 - coord.first, 2) + pow(lon1 - coord.second, 2));
        distances.push_back({id, dist});
    }

    sort(distances.begin(), distances.end(),
         [](const auto& a, const auto& b) { return a.second < b.second; });

    if ((int)distances.size() > k)
        distances.resize(k);

    return distances;
}

/*
    KNN based on shortest path distance using Dijkstra's algorithm.
*/
vector<pair<int, double>> KNN_ShortestPath(const Graph& g, int source, int k) {
    const double INF = numeric_limits<double>::infinity();
    unordered_map<int, double> dist;

    for (const auto& [id, coord] : g.coords)
        dist[id] = INF;

    if (!g.coords.count(source)) {
        cerr << "Error: Source node not found in graph.\n";
        return {};
    }

    dist[source] = 0.0;

    using P = pair<double, int>;
    priority_queue<P, vector<P>, greater<P>> pq;
    pq.push({0.0, source});

    while (!pq.empty()) {
        auto [d, u] = pq.top();
        pq.pop();
        if (d > dist[u]) continue;

        if (!g.adj.count(u)) continue;
        for (const auto& edge : g.adj.at(u)) {
            if (!edge.active) continue;
            int v = edge.v;
            double w = edge.length;

            if (dist[u] + w < dist[v]) {
                dist[v] = dist[u] + w;
                pq.push({dist[v], v});
            }
        }
    }

    vector<pair<int, double>> res;
    for (const auto& [id, d] : dist) {
        if (id == source || d == INF) continue;
        res.push_back({id, d});
    }

    sort(res.begin(), res.end(),
        [](const auto& a, const auto& b) { return a.second < b.second; });

    if ((int)res.size() > k)
        res.resize(k);

    return res;
}
