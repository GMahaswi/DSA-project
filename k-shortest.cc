#include <iostream>
#include <vector>
#include <queue>
#include <unordered_map>
#include <algorithm>
#include <limits>
#include <utility>
#include <set>
#include <map> 
using namespace std;

struct Edge {
    int v;
    double w;
};
unordered_map<int, vector<Edge>> graph;
unordered_map<int, unordered_map<int, double>> edgeW;
for (auto &[u, adj] : graph)
    for (auto &e : adj)
        edgeW[u][e.v] = e.w;

vector<int> dijkstraPath(int src, int dest, double &cost) {
    unordered_map<int, double> dist;
    unordered_map<int, int> parent;
    for (auto &p : graph) dist[p.first] = 1e18;
    using P = pair<double,int>;
    priority_queue<P, vector<P>, greater<P>> pq;
    dist[src] = 0;
    pq.push({0, src});
    
    while (!pq.empty()) {
        auto [d, u] = pq.top(); pq.pop();
        if (u == dest) break;
        if (d > dist[u]) continue;
        for (auto &e : graph[u]) {
            if (dist[e.v] > d + e.w) {
                dist[e.v] = d + e.w;
                parent[e.v] = u;
                pq.push({dist[e.v], e.v});
            }
        }
    }

    vector<int> path;
    if (dist[dest] == 1e18) return path;
    for (int v = dest; v != src; v = parent[v]) path.push_back(v);
    path.push_back(src);
    reverse(path.begin(), path.end());
    cost = dist[dest];
    return path;
}

vector<pair<vector<int>, double>> yenKShortest(int src, int dest, int K) {
    vector<pair<vector<int>, double>> A; // result paths
    vector<pair<vector<int>, double>> B; // candidates

    double firstCost;
    vector<int> firstPath = dijkstraPath(src, dest, firstCost);
    if (firstPath.empty()) return A;
    A.push_back({firstPath, firstCost});

    for (int k = 1; k < K; k++) {
        const vector<int> &prevPath = A[k-1].first;
        
        
        for (int i = 0; i < (int)prevPath.size()-1; i++) {
            int spurNode = prevPath[i];
            vector<int> rootPath(prevPath.begin(), prevPath.begin()+i+1);

            // Remove edges that share the same prefix
            vector<pair<int,int>> removed;
            for (auto &p : A) {
                auto &path = p.first;
                if (path.size() > i+1 && equal(rootPath.begin(), rootPath.end(), path.begin()))
                {
                    int u = path[i], v = path[i+1];
                    auto &adj = graph[u];
                    auto it = find_if(adj.begin(), adj.end(), [&](Edge &e){return e.v == v;});
                    if (it != adj.end()) {
                        removed.push_back({u,v});
                        adj.erase(it);
                    }
                }
            }

            double spurCost;
            vector<int> spurPath = dijkstraPath(spurNode, dest, spurCost);
            if (!spurPath.empty()) {
                vector<int> totalPath = rootPath;
                totalPath.insert(totalPath.end(), spurPath.begin()+1, spurPath.end());
                                        
                double totalCost = 0;
                for (int j = 0; j+1 < totalPath.size(); j++) {
                    int u = totalPath[j], v = totalPath[j+1];
                    for (auto &e : graph[u])
                        if (e.v == v) totalCost += e.w;
                }
                bool exists = false;
                for (auto &b : B) if (b.first == totalPath) exists = true;
                if (!exists)
                    B.push_back({totalPath, totalCost});
            }

            // Restore edges
            for (auto &[u,v] : removed) {
                graph[u].push_back({v, 1}); // replace correct weight if stored separately
            }
        }

        if (B.empty()) break;
        auto it = min_element(B.begin(), B.end(), [](auto &a, auto &b){return a.second < b.second;});
        A.push_back(*it);
        B.erase(it);
    }

    return A;
}
