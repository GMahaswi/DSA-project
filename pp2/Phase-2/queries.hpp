#pragma once
#include <nlohmann/json.hpp>
#include "graph.hpp"
#include "knn.hpp"

using json = nlohmann::json;

// Process a single query (Phase-1 + Phase-2)
json process_query(Graph& g, const json& query);

// Phase-2 helper functions
json handleKShortestPathsExact(Graph& g, const json& query);
json handleKShortestPathsHeuristic(Graph& g, const json& query);
json handleApproxShortestPaths(Graph& g, const json& query);
