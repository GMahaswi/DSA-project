#pragma once
#include <bits/stdc++.h>
#include "nlohmann/json.hpp"
using namespace std;
using json = nlohmann::json;

struct Edge {
    int id;
    int u, v;
    double length;
    double average_time;
    bool oneway;
    string road_type;
    vector<double> speed_profile;
    bool active = true;
};

class Graph {
public:
    unordered_map<int, vector<Edge>> adj; // adjacency list
    unordered_map<int, pair<double, double>> coords; // node → (lat, lon)
    unordered_map<int, vector<string>> pois; // node → list of POIs
    unordered_map<int, Edge> edge_map; // edge_id → Edge
    unordered_map<int, Edge> removed_edges; // backup for removed roads

    void loadGraph(const string& filename);
    bool removeEdge(int edge_id);
    bool modifyEdge(int edge_id, double new_length, double new_avg_time); // ✅ changed from void → bool
    double euclideanDistance(int a, int b) const;
    
    tuple<bool, double, vector<int>> shortestPath(
        int source, int target,
        const string& mode,
        const unordered_set<int>& forbidden_nodes,
        const unordered_set<string>& forbidden_roads
    ) const;
};
