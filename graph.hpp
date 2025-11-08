#pragma once
#include <vector>
#include <unordered_map>
#include <cmath>
#include <queue>
#include <utility>
#include <unordered_set>

using namespace std;

#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

>>>>>>> 003f36ab0132d98004a13a5a357dbbb1b29e90ac
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
        const double R=6371.0;
        auto &na = nodes.at(a);
        auto &nb = nodes.at(b);

        double lat1=na.y*M_PI/180.0;
        double lon1=na.x*M_PI/180.0;
        double lat2=nb.y*M_PI/180.0;
        double lon2=nb.x*M_PI/180.0;

        double dLat=lat2-lat1;
        double dLon=lon2-lon1;

        double h=sin(dLat/2.0)*sin(dLat/2.0)+cos(lat1)*cos(lat2)*sin(dLon/2.0)*sin(dLon/2.0);

        double c=2.0*atan2(sqrt(h),sqrt(1-h));
        
        return R*c;
    }
};
