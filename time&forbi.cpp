#include <bits/stdc++.h>
using namespace std;

struct Edge {
    int to;
    double distance; // in km
    function<double(double)> speedFunc; // speed in km/h as function of departure time in minutes
    vector<pair<double,double>> closedIntervals; // list of {start,end} in minutes
};

struct NodeState {
    int node;
    double arrivalTime; // in minutes
    bool operator>(const NodeState &other) const {
        return arrivalTime > other.arrivalTime;
    }
};

// Check if edge is closed at time t
bool isEdgeClosed(const Edge &e, double t) {
    for(auto &[start,end] : e.closedIntervals)
        if(t >= start && t <= end) return true;
    return false;
}

// Time-dependent Dijkstra with forbidden edges/nodes
double timeDependentDijkstra(int n, int src, int dest, 
                             const vector<vector<Edge>> &graph, 
                             double startTime,
                             const unordered_set<int> &forbiddenNodes) {

    vector<double> arrival(n+1, 1e18);
    priority_queue<NodeState, vector<NodeState>, greater<NodeState>> pq;

    if(forbiddenNodes.count(src) || forbiddenNodes.count(dest))
        return -1; // source or destination forbidden

    arrival[src] = startTime;
    pq.push({src, startTime});

    while(!pq.empty()) {
        auto [u, currTime] = pq.top(); pq.pop();

        if(u == dest) return currTime - startTime; // total travel time
        if(currTime > arrival[u]) continue;
        if(forbiddenNodes.count(u)) continue;

        for(const Edge &e : graph[u]) {
            if(isEdgeClosed(e, currTime)) continue;
            if(forbiddenNodes.count(e.to)) continue;

            double spd = e.speedFunc(currTime); // km/h
            double travelTime = (e.distance / spd) * 60.0; // minutes
            double newTime = currTime + travelTime;

            if(newTime < arrival[e.to]) {
                arrival[e.to] = newTime;
                pq.push({e.to, newTime});
            }
        }
    }

    return -1; // destination unreachable
}

//---------------- Example Usage ----------------
int main() {
    int n = 4; // nodes 1..4
    vector<vector<Edge>> graph(n+1);

    // Time-dependent speed functions
    auto speed12 = [](double t){ return (t < 480) ? 60 : ((t<600)? 20 : 50); };
    auto speed23 = [](double t){ return 40; };
    auto speed34 = [](double t){ return 30; };
    auto speed14 = [](double t){ return 25; };

    // Build graph with some closed intervals
    graph[1].push_back({2, 10, speed12, {{480,600}}}); // closed 8:00–10:00
    graph[2].push_back({1, 10, speed12, {{480,600}}}); 

    graph[2].push_back({3, 15, speed23, {}}); // always open
    graph[3].push_back({2, 15, speed23, {}});

    graph[3].push_back({4, 10, speed34, {}}); // always open
    graph[4].push_back({3, 10, speed34, {}});

    graph[1].push_back({4, 40, speed14, {}}); // always open
    graph[4].push_back({1, 40, speed14, {}});

    // Forbidden nodes
    unordered_set<int> forbiddenNodes;
    forbiddenNodes.insert(3); // cannot go through node 3

    double startTime = 450; // 7:30 AM
    double minTime = timeDependentDijkstra(n, 1, 4, graph, startTime, forbiddenNodes);
    if(minTime >= 0)
        cout << "Minimum travel time 1->4 starting 7:30 AM: " << minTime << " minutes\n";
    else
        cout << "Destination unreachable (constraints)\n";

    startTime = 630; // 10:30 AM
    minTime = timeDependentDijkstra(n, 1, 4, graph, startTime, forbiddenNodes);
    if(minTime >= 0)
        cout << "Minimum travel time 1->4 starting 10:30 AM: " << minTime << " minutes\n";
    else
        cout << "Destination unreachable (constraints)\n";

    return 0;
}
