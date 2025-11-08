#pragma once
#include <nlohmann/json.hpp>
#include "graph.hpp"
#include "knn.hpp"

using json = nlohmann::json;

// Process a single query event (remove_edge, modify_edge, shortest_path, knn, etc.)
json process_query(Graph& g, const json& query);
