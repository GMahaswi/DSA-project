#pragma once
#include <nlohmann/json.hpp>
#include "Graph.hpp"
using json=nlohmann::json;

json process_query(Graph& g, const json& query);
