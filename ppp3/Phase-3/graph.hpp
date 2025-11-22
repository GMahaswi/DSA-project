#pragma once
#include <unordered_map>
#include <vector>
#include <tuple>
#include <string>
#include <cmath>
#include <nlohmann/json.hpp>

using namespace std;
using json = nlohmann::json;

// ------------------ Edge / Graph ------------------
struct Edge {
    int id;
    int u,v;
    double length;
    double average_time;
    bool oneway;
    string road_type;
};

struct Order {
    int id;
    int pickup;
    int drop;
};

struct Driver {
    int id;
    double score;
    double accidentRisk;
};

struct Cluster {
    int id;
    vector<Order> orders;
};

struct Assignment {
    int driverId;
    vector<Order> assignedOrders;
};

struct DFSState {
    vector<int> path;
    double totalDist;
};

class Graph {
public:
    unordered_map<int, unordered_map<int, Edge>> adj;
    unordered_map<int, pair<double,double>> coords;
    unordered_map<int, Edge> edge_map;

    unordered_map<int, unordered_map<int,double>> distCache;

    void loadGraph(const string& filename);
    void addEdge(int u,int v,double length,double avgTime,bool oneway=false);
    double euclideanDistance(int a, int b) const;

    tuple<bool,double,vector<int>> shortestPath(int source,int target,const string& mode="average") const;

    double getDist(int a, int b);

    vector<Cluster> clusterOrders(vector<Order>& orders,int maxClusterSize);
    double getClusterDistance(const Cluster& c,int depotNode=0);
    double computeClusterScore(const Cluster& c);
    vector<Assignment> assignClustersToDrivers(vector<Cluster>& clusters, vector<Driver>& drivers);
};

// Phase-3 external function:
json processDeliverySchedule(Graph& g, const json& query);
