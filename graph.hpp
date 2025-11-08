#pragma once
#include <vector>
#include <unordered_map>
#include <cmath>
#include <queue>
#include <utility>
#include <unordered_set>
using namespace std;
struct Node {
    int id;
    double x, y;  // coordinates
};

class Graph {
public:
    std::unordered_map<int, Node> nodes;
    std::unordered_map<int, std::vector<std::pair<int, double>>> adj; // neighbor, weight
    
    std::unordered_map<int,unordered_map<int,double>>edgeweight;
    void addNode(int id, double x, double y) {
        nodes[id] = {id, x, y};
    }

    void addEdge(int u, int v, double w) {
        adj[u].push_back({v, w});
        adj[v].push_back({u, w}); // undirected graph
    }

    double euclideanDistance(int a, int b) const {
        auto &na = nodes.at(a);
        auto &nb = nodes.at(b);
        double dx = na.x - nb.x;
        double dy = na.y - nb.y;
        return std::sqrt(dx * dx + dy * dy);
    }
};
