#include <iostream>
#include <vector>
using namespace std;

struct Edge {
    int to;
    double distance; // km
    function<double(double)> speedFunc; // speed in km/h as function of departure time in minutes
};

struct NodeState {
    int node;
    double arrivalTime; // in minutes
    bool operator>(const NodeState &other) const {
        return arrivalTime > other.arrivalTime;
    }
};

// Function to compute minimum time from src to dest
double timeDependentShortestPath(int n, int src, int dest, 
                                 const vector<vector<Edge>> &graph, double startTime) {
    vector<double> arrival(n+1, 1e18);
    priority_queue<NodeState, vector<NodeState>, greater<NodeState>> pq;

    arrival[src] = startTime;
    pq.push({src, startTime});

    while(!pq.empty()) {
        auto [u, currTime] = pq.top(); pq.pop();

        if(u == dest) return currTime - startTime; // total travel time
        if(currTime > arrival[u]) continue;

        for(const Edge &e : graph[u]) {
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

//------------------ Example Usage ------------------
int main() {
    int n = 4; // nodes 1..4
    vector<vector<Edge>> graph(n+1);

    // Define time-dependent speed function (example: morning traffic slower)
    auto speedEdge12 = [](double t){ // t in minutes
        if(t < 480) return 60.0;       // before 8 AM
        else if(t < 600) return 20.0;  // 8-10 AM traffic jam
        else return 50.0;               // after 10 AM
    };

    auto speedEdge23 = [](double t){ return 40.0; };
    auto speedEdge34 = [](double t){ return 30.0; };
    auto speedEdge14 = [](double t){ return 25.0; };

    // Build graph (undirected)
    graph[1].push_back({2, 10, speedEdge12});
    graph[2].push_back({1, 10, speedEdge12});

    graph[2].push_back({3, 15, speedEdge23});
    graph[3].push_back({2, 15, speedEdge23});

    graph[3].push_back({4, 10, speedEdge34});
    graph[4].push_back({3, 10, speedEdge34});

    graph[1].push_back({4, 40, speedEdge14});
    graph[4].push_back({1, 40, speedEdge14});

    double startTime = 450; // 7:30 AM = 450 minutes
    double minTime = timeDependentShortestPath(n, 1, 4, graph, startTime);
    cout << "Minimum travel time from 1 -> 4 starting at 7:30 AM: " << minTime << " minutes\n";

    startTime = 630; // 10:30 AM
    minTime = timeDependentShortestPath(n, 1, 4, graph, startTime);
    cout << "Minimum travel time from 1 -> 4 starting at 10:30 AM: " << minTime << " minutes\n";

    return 0;
}
