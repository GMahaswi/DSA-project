#pragma once
#include "Graph.hpp"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

json processDeliverySchedule(Graph& g, const json& query);
