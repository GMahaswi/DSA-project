#include "queries.hpp"
#include <queue>

PathResult shortestPath(Graph& G, int src, int tgt, string mode) {
    unordered_map<int, double> dist;
    unordered_map<int, int> parent;

    for (auto& [node, _] : G.coords) dist[node] = 1e18;
    dist[src] = 0;

    using P = pair<double, int>;
    priority_queue<P, vector<P>, greater<P>> pq;
    pq.push({0, src});

    while (!pq.empty()) {
        auto [d, u] = pq.top(); pq.pop();
        if (d != dist[u]) continue;
        if (u == tgt) break;

        for (auto& e : G.adj[u]) {
            double w = (mode == "distance") ? e.length : e.average_time;
            if (dist[e.v] > d + w) {
                dist[e.v] = d + w;
                parent[e.v] = u;
                pq.push({dist[e.v], e.v});
            }
        }
    }

    PathResult res;
    res.cost = dist[tgt];
    vector<int> path;
    if (dist[tgt] < 1e18) {
        int cur = tgt;
        while (cur != src) {
            path.push_back(cur);
            cur = parent[cur];
        }
        path.push_back(src);
        reverse(path.begin(), path.end());
    }
    res.path = path;
    return res;
}
