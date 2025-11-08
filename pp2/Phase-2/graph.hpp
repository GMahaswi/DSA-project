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

struct Path {
    vector<int> nodes;
    double length;
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
    bool modifyEdge(int edge_id, double new_length, double new_avg_time); // ✅ Phase-1 already changed
    double euclideanDistance(int a, int b) const;
    double euclideanDistanceLatLon(double lat1, double lon1, double lat2, double lon2) const;

    // Phase-1 shortest path
    tuple<bool, double, vector<int>> shortestPath(
        int source, int target,
        const string& mode,
        const unordered_set<int>& forbidden_nodes = {},
        const unordered_set<string>& forbidden_roads = {}
    ) const;

    // -------------------- PHASE-2 ADDITIONS --------------------
vector<Path> kShortestPathsExact(int source, int target, int k, const string& mode="distance");
vector<Path> kShortestPathsHeuristic(int source, int target, int k, int overlap_threshold, const string& mode="distance");
vector<double> approxShortestPaths(const vector<pair<int,int>>& queries, int time_budget_ms, double acceptable_error_pct, const string& mode="distance");

};
