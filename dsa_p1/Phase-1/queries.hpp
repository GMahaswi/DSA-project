#pragma once
#include "graph.hpp"
#include "nlohmann/json.hpp"
using json = nlohmann::json;

struct PathResult {
    double cost;
    vector<int> path;
};

PathResult shortestPath(Graph& G, int src, int tgt, string mode, int departure_slot = 0);
json pathToJson(const PathResult& res, int src, int tgt, string mode, int departure_slot = 0);
