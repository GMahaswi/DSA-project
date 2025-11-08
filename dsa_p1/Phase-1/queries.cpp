#include "queries.hpp"
#include <bits/stdc++.h>
using namespace std;
using json = nlohmann::json;

json process_query(Graph& g, const json& query) {
    json result;
    result["id"] = query["id"];

    string type = query["type"];

    // 1️⃣ REMOVE EDGE
    if (type == "remove_edge") {
        int edge_id = query["edge_id"];
        bool done = g.removeEdge(edge_id);
        result["done"] = done;
        return result;
    }

    // 2️⃣ MODIFY EDGE
    else if (type == "modify_edge") {
        int edge_id = query["edge_id"];
        bool done = false;

        if (query.contains("patch")) {
            double new_length = query["patch"].value("length", -1.0);
            double new_avg_time = query["patch"].value("average_time", -1.0);
            done = g.modifyEdge(edge_id, new_length, new_avg_time);
        }

        result["done"] = done;
        return result;
    }

    // 3️⃣ SHORTEST PATH
    else if (type == "shortest_path") {
        int source = query["source"];
        int target = query["target"];
        string mode = query["mode"];

        unordered_set<int> forbidden_nodes;
        unordered_set<string> forbidden_roads;

        if (query.contains("constraints")) {
            auto constraints = query["constraints"];
            if (constraints.contains("forbidden_nodes")) {
                for (auto &n : constraints["forbidden_nodes"])
                    forbidden_nodes.insert(n.get<int>());  // ✅ fixed
            }
            if (constraints.contains("forbidden_road_types")) {
                for (auto &r : constraints["forbidden_road_types"])
                    forbidden_roads.insert(r.get<string>());  // ✅ fixed
            }
        }

        auto [possible, dist, path] = g.shortestPath(source, target, mode, forbidden_nodes, forbidden_roads);

        result["possible"] = possible;
        if (possible) {
            if (mode == "time")
                result["minimum_time"] = dist;
            else
                result["minimum_distance"] = dist;
            result["path"] = path;
        }

        return result;
    }

    // 4️⃣ KNN
    else if (type == "knn") {
        int source = query["source"];
        int k = query["k"];
        string method = query["method"];

        vector<pair<int, double>> res;
        if (method == "euclidean") {
            res = KNN_Euclidean(g, source, k);  // ✅ no g.
        } else {
            res = KNN_ShortestPath(g, source, k);  // ✅ no g.
        }

        json neighbors = json::array();
        for (auto &p : res) {
            neighbors.push_back({{"id", p.first}, {"distance", p.second}});
        }

        result["neighbors"] = neighbors;
        return result;
    }

    // Default
    result["error"] = "Unknown query type";
    return result;
}
