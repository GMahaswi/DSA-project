#include <bits/stdc++.h>
using namespace std;

struct Edge {
    int to;
    double w;
};

struct Node {
    int id;
    double g, f;
    vector<int> path;
    bool operator>(const Node& other) const { return f > other.f; }
};

double heuristic(int u, int target) {
    // Simple heuristic example (replace with domain-specific one)
    // If no coordinates are known, just return 0 (behaves like Dijkstra)
    return 0.0;
}

vector<vector<int>> heuristicKShortestPaths(
    const vector<vector<Edge>>& graph, int src, int dest, int K)
{
    priority_queue<Node, vector<Node>, greater<Node>> pq;
    vector<vector<int>> results;

    pq.push({src, 0, heuristic(src, dest), {src}});

    while (!pq.empty() && results.size() < K) {
        Node curr = pq.top(); pq.pop();

        if (curr.id == dest) {
            results.push_back(curr.path);
            continue; // continue to find next best path
        }

        for (auto& e : graph[curr.id]) {
            double g2 = curr.g + e.w;
            double f2 = g2 + heuristic(e.to, dest);
            auto newPath = curr.path;
            newPath.push_back(e.to);
            pq.push({e.to, g2, f2, newPath});
        }
    }

    return results;
}

int main() {
    int n, m;
    cout << "Enter number of nodes and edges: ";
    cin >> n >> m;

    vector<vector<Edge>> graph(n);
    cout << "Enter edges (u v w):" << endl;
    for (int i = 0; i < m; i++) {
        int u, v; double w;
        cin >> u >> v >> w;
        graph[u].push_back({v, w});
        // Uncomment for undirected:
        // graph[v].push_back({u, w});
    }

    int src, dest, K;
    cout << "Enter source, destination, and K: ";
    cin >> src >> dest >> K;

    auto paths = heuristicKShortestPaths(graph, src, dest, K);

    cout << "\nTop " << paths.size() << " paths found:\n";
    for (int i = 0; i < paths.size(); i++) {
        double totalCost = 0;
        for (int j = 0; j + 1 < paths[i].size(); j++) {
            int u = paths[i][j], v = paths[i][j + 1];
            for (auto& e : graph[u]) if (e.to == v) totalCost += e.w;
        }
        cout << "Path " << i + 1 << " (Cost = " << totalCost << "): ";
        for (int x : paths[i]) cout << x << " ";
        cout << "\n";
    }
}
