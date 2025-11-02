#include <bits/stdc++.h>
using namespace std;

class Solution {
public:
    int shortestPathLengthWeighted(vector<vector<pair<int, int>>>& graph) {
        int n = graph.size();
        int totalMask = 1 << n;

        // dist[mask][node] = shortest distance to reach this state
        vector<vector<int>> dist(totalMask, vector<int>(n, INT_MAX));

        // Min-heap: {distance, node, mask}
        priority_queue<tuple<int, int, int>,
                       vector<tuple<int, int, int>>,
                       greater<>> pq;

        // Initialize: starting from each node
        for (int i = 0; i < n; i++) {
            int mask = (1 << i);
            dist[mask][i] = 0;
            pq.push({0, i, mask});
        }

        while (!pq.empty()) {
            auto [d, node, mask] = pq.top();
            pq.pop();

            // If already found shorter path to this state, skip
            if (d > dist[mask][node])
                continue;

            // If all nodes visited
            if (mask == (1 << n) - 1)
                return d;  // shortest path covering all nodes

            // Explore neighbors
            for (auto [nei, w] : graph[node]) {
                int newMask = mask | (1 << nei);
                int newDist = d + w;

                if (newDist < dist[newMask][nei]) {
                    dist[newMask][nei] = newDist;
                    pq.push({newDist, nei, newMask});
                }
            }
        }

        return -1; // theoretically unreachable
    }
};
