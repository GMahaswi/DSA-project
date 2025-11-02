#pragma once
#include "graph.hpp"
#include "nlohmann/json.hpp"
using json = nlohmann::json;

struct PathResult {
    double cost;
    vector<int> path;
};

PathResult shortestPath(Graph& G, int src, int tgt, string mode);
