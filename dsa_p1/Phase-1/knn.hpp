#pragma once
#include "graph.hpp"
#include <bits/stdc++.h>

std::vector<std::pair<int, double>> KNN_Euclidean(const Graph& g, int source, int k);
std::vector<std::pair<int, double>> KNN_ShortestPath(const Graph& g, int source, int k);
