#include <bits/stdc++.h>
using namespace std;

struct Edge {
    int to;
    double weight;
};

struct Path {
    vector<int> nodes;
    double cost;
    bool operator<(const Path& other) const {
        return cost > other.cost; // for min-heap
    }
};

double pathCost(const vector<int>& path, const vector<vector<Edge>>& graph) {
    double cost = 0;
    for (int i = 0; i + 1 < path.size(); i++) {
        int u = path[i], v = path[i + 1];
        bool found = false;
        for (auto& e : graph[u]) {
            if (e.to == v) {
                cost += e.weight;
                found = true;
                break;
            }
        }
        if (!found) return 1e9; // invalid path
    }
    return cost;
}

// Dijkstra for shortest path
vector<int> dijkstraPath(int src, int dest, const vector<vector<Edge>>& graph) {
    int n = graph.size();
    vector<double> dist(n, 1e9);
    vector<int> parent(n, -1);
    priority_queue<pair<double, int>, vector<pair<double, int>>, greater<>> pq;

    dist[src] = 0;
    pq.push({0, src});

    while (!pq.empty()) {
        auto [d, u] = pq.top(); pq.pop();
        if (d > dist[u]) continue;
        for (auto& e : graph[u]) {
            if (dist[e.to] > d + e.weight) {
                dist[e.to] = d + e.weight;
                parent[e.to] = u;
                pq.push({dist[e.to], e.to});
            }
        }
    }

    vector<int> path;
    if (dist[dest] == 1e9) return path;
    for (int v = dest; v != -1; v = parent[v]) path.push_back(v);
    reverse(path.begin(), path.end());
    return path;
}

// Yen's algorithm
vector<vector<int>> yenKShortestPaths(const vector<vector<Edge>>& graph, int src, int dest, int K) {
    vector<vector<int>> A; // shortest paths
    vector<Path> B;        // potential paths

    // 1st shortest path
    vector<int> firstPath = dijkstraPath(src, dest, graph);
    if (firstPath.empty()) return A;
    A.push_back(firstPath);

    for (int k = 1; k < K; ++k) {
        const vector<int>& prevPath = A[k - 1];
        for (int i = 0; i + 1 < prevPath.size(); ++i) {
            int spurNode = prevPath[i];
            vector<int> rootPath(prevPath.begin(), prevPath.begin() + i + 1);

            // backup and remove edges/nodes
            vector<vector<Edge>> tempGraph = graph;
            for (auto& path : A) {
                if (path.size() > i && equal(rootPath.begin(), rootPath.end(), path.begin())) {
                    int u = path[i], v = path[i + 1];
                    tempGraph[u].erase(
                        remove_if(tempGraph[u].begin(), tempGraph[u].end(),
                                  [&](const Edge& e){ return e.to == v; }),
                        tempGraph[u].end()
                    );
                }
            }

            vector<int> spurPath = dijkstraPath(spurNode, dest, tempGraph);
            if (spurPath.empty()) continue;

            vector<int> totalPath = rootPath;
            totalPath.pop_back();
            totalPath.insert(totalPath.end(), spurPath.begin(), spurPath.end());

            double cost = pathCost(totalPath, graph);
            B.push_back({totalPath, cost});
        }

        if (B.empty()) break;
        sort(B.begin(), B.end());
        A.push_back(B.front().nodes);
        B.erase(B.begin());
    }

    return A;
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
        // Uncomment next line if undirected
        // graph[v].push_back({u, w});
    }

    int src, dest, K;
    cout << "Enter source, target, and N (number of shortest paths): ";
    cin >> src >> dest >> K;

    vector<vector<int>> paths = yenKShortestPaths(graph, src, dest, K);

    cout << "\nTop " << paths.size() << " shortest paths:\n";
    for (int i = 0; i < paths.size(); i++) {
        cout << "Path " << i + 1 << ": ";
        for (int node : paths[i]) cout << node << " ";
        cout << " | Cost = " << pathCost(paths[i], graph) << endl;
    }
}
