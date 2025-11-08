#pragma once
#include "graph.hpp"
#include "knn.hpp"
#include "nlohmann/json.hpp"
#include <bits/stdc++.h>

using json = nlohmann::json;

json shortest_path_query(const Graph& g, const json& query);
json knn_query(const Graph& g, const json& query);
json process_query(const Graph& g, const json& query);
