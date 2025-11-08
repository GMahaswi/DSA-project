#include "queries.hpp"
#include <queue>
#include <limits>
#include <unordered_map>
#include <set>
#include <vector>
#include <algorithm>
using namespace std;
using json = nlohmann::json;

// ---- Shortest Path ----
json shortest_path_query(const Graph& g, const json& query) {
    json result;
    result["id"] = query["id"];

    int source = query["source"];
    int target = query["target"];
    string mode = query.value("mode", "distance");

    set<int> forbidden_nodes;
    set<string> forbidden_roads;

    if (query.contains("constraints")) {
        auto cons = query["constraints"];
        if (cons.contains("forbidden_nodes"))
            for (int n : cons["forbidden_nodes"])
                forbidden_nodes.insert(n);
        if (cons.contains("forbidden_road_types"))
            for (const auto& rt : cons["forbidden_road_types"])
                forbidden_roads.insert(rt);
    }

    if (forbidden_nodes.count(source) || forbidden_nodes.count(target)) {
        result["possible"] = false;
        return result;
    }

    const double INF = numeric_limits<double>::infinity();
    unordered_map<int, double> dist;
    unordered_map<int, int> parent;

    for (const auto& [id, _] : g.coords)
        dist[id] = INF;

    dist[source] = 0.0;
    using P = pair<double, int>;
    priority_queue<P, vector<P>, greater<P>> pq;
    pq.push({0.0, source});

    while (!pq.empty()) {
        auto [d, u] = pq.top();
        pq.pop();
        if (d > dist[u]) continue;
        if (u == target) break;
        if (!g.adj.count(u)) continue;

        for (const auto& edge : g.adj.at(u)) {
            if (!edge.active) continue;
            if (forbidden_nodes.count(edge.v)) continue;
            if (forbidden_roads.count(edge.road_type)) continue;

            double weight = (mode == "time") ? edge.average_time : edge.length;
            if (dist[u] + weight < dist[edge.v]) {
                dist[edge.v] = dist[u] + weight;
                parent[edge.v] = u;
                pq.push({dist[edge.v], edge.v});
            }
        }
    }

    if (dist[target] == INF) {
        result["possible"] = false;
        return result;
    }

    vector<int> path;
    for (int v = target; v != source; v = parent[v])
        path.push_back(v);
    path.push_back(source);
    reverse(path.begin(), path.end());

    result["possible"] = true;
    if (mode == "time")
        result["minimum_time"] = dist[target];
    else
        result["minimum_distance"] = dist[target];
    result["path"] = path;
    return result;
}

// ---- KNN ----
json knn_query(const Graph& g, const json& query) {
    json result;
    result["id"] = query["id"];
    int source = query["source"];
    int k = query["k"];
    string mode = query.value("mode", "euclidean");

    vector<pair<int, double>> neighbors;
    if (mode == "euclidean")
        neighbors = KNN_Euclidean(g, source, k);
    else
        neighbors = KNN_ShortestPath(g, source, k);

    result["neighbors"] = json::array();
    for (auto& [node, dist] : neighbors) {
        json obj;
        obj["node"] = node;
        obj["distance"] = dist;
        result["neighbors"].push_back(obj);
    }

    return result;
}

// ---- Dispatcher ----
json process_query(const Graph& g, const json& query) {
    string type = query.value("type", "");

    if (type == "shortest_path")
        return shortest_path_query(g, query);
    else if (type == "knn")
        return knn_query(g, query);
    else {
        json r;
        r["status"] = "unknown_query_type";
        return r;
    }
}
