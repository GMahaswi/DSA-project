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
        result["done"] = g.removeEdge(edge_id);
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
                    forbidden_nodes.insert(n.get<int>());
            }
            if (constraints.contains("forbidden_road_types")) {
                for (auto &r : constraints["forbidden_road_types"])
                    forbidden_roads.insert(r.get<string>());
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

    // 4️⃣ KNN QUERY
    else if (type == "knn") {
        int k = query["k"];
        string poi_type = query["poi"];
        string metric = query["metric"];
        double qlat = query["query_point"]["lat"];
        double qlon = query["query_point"]["lon"];

        vector<pair<int, double>> candidates;
        vector<int> poi_nodes;

        // Step 1️⃣: Filter nodes matching POI type
        for (const auto& [node_id, poi_list] : g.pois) {
            for (const string& t : poi_list) {
                if (t == poi_type) {
                    poi_nodes.push_back(node_id);
                    break;
                }
            }
        }

        if (poi_nodes.empty()) {
            result["nodes"] = json::array();
            return result;
        }

        // Step 2️⃣: Compute distances
        if (metric == "euclidean") {
            for (int nid : poi_nodes) {
                auto [lat, lon] = g.coords[nid];
                double dist = g.euclideanDistanceLatLon(qlat, qlon, lat, lon);
                candidates.push_back({nid, dist});
            }
        } else if (metric == "shortest_path") {
            int nearest_node = -1;
            double nearest_dist = 1e18;
            for (const auto& [nid, coord] : g.coords) {
                double dist = g.euclideanDistanceLatLon(qlat, qlon, coord.first, coord.second);
                if (dist < nearest_dist) {
                    nearest_dist = dist;
                    nearest_node = nid;
                }
            }

            if (nearest_node == -1) {
                result["nodes"] = json::array();
                return result;
            }

            for (int nid : poi_nodes) {
                auto [possible, dist, path] = g.shortestPath(nearest_node, nid, "distance", {}, {});
                if (possible)
                    candidates.push_back({nid, dist});
            }
        }

        // Step 3️⃣: Sort and take top k
        sort(candidates.begin(), candidates.end(),
             [](auto &a, auto &b){ return a.second < b.second; });
        if ((int)candidates.size() > k)
            candidates.resize(k);

        // Step 4️⃣: Output
        json nodes = json::array();
        for (auto &p : candidates) nodes.push_back(p.first);
        result["nodes"] = nodes;
        return result;
    }

    // 🧩 Default fallback
    result["error"] = "Unknown query type";
    return result;
}
