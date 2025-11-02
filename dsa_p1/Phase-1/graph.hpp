#pragma once
#include <bits/stdc++.h>
using namespace std;

struct Edge {
    int id;
    int u, v;
    double length;
    double average_time;
    bool oneway;
    string road_type;
    vector<double> speed_profile;
};

class Graph {
public:
    unordered_map<int, vector<Edge>> adj; // adjacency list
    unordered_map<int, pair<double, double>> coords; // node → (lat, lon)
    unordered_map<int, vector<string>> pois; // node → list of POIs
    unordered_map<int, Edge> edge_map; // edge_id → Edge

    void loadGraph(const string& filename);
    void removeEdge(int edge_id);
    void modifyEdge(int edge_id, double new_length, double new_avg_time);
};
