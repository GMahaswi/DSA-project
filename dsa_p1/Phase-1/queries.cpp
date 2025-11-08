// #include "queries.hpp"
// using namespace std;

// /*
//     Phase-2 version:
//     - Supports time-dependent edge weights using 96-slot speed profiles.
//     - Uses departure_time (in minutes or slot index) to pick appropriate slot.
//     - Falls back to average_time if speed_profile is missing.
// */

// PathResult shortestPath(Graph& G, int src, int tgt, string mode, int departure_slot) {
//     unordered_map<int, double> dist;
//     unordered_map<int, int> parent;

//     for (auto& [node, _] : G.coords)
//         dist[node] = 1e18;
//     dist[src] = 0.0;

//     using P = pair<double, int>;
//     priority_queue<P, vector<P>, greater<P>> pq;
//     pq.push({0.0, src});

//     while (!pq.empty()) {
//         auto [d, u] = pq.top();
//         pq.pop();
//         if (d != dist[u]) continue;
//         if (u == tgt) break;

//         for (auto& e : G.adj[u]) {
//             double w = 0.0;

//             // --- TIME-DEPENDENT HANDLING ---
//             if (mode == "distance") {
//                 w = e.length;
//             } 
//             else if (mode == "time") {
//                 if (e.speed_profile.empty()) {
//                     w = e.average_time;  // fallback if no profile
//                 } else {
//                     // each speed_profile[i] = speed in m/s or km/h at that slot
//                     int slot = departure_slot % 96; 
//                     double speed = e.speed_profile[slot];
//                     if (speed <= 0) speed = 1; // avoid div-by-zero
//                     w = e.length / speed;
//                 }
//             } 
//             else {
//                 cerr << "Warning: Unknown mode '" << mode 
//                     << "', defaulting to distance.\n";
//                 w = e.length;
//             }

//             if (dist[e.v] > d + w) {
//                 dist[e.v] = d + w;
//                 parent[e.v] = u;
//                 pq.push({dist[e.v], e.v});
//             }
//         }
//     }

//     PathResult res;
//     res.cost = dist[tgt];

//     if (dist[tgt] < 1e18) {
//         int cur = tgt;
//         while (cur != src) {
//             res.path.push_back(cur);
//             cur = parent[cur];
//         }
//         res.path.push_back(src);
//         reverse(res.path.begin(), res.path.end());
//     }

//     return res;
// }

// // Convert result into structured JSON for output.json
// json pathToJson(const PathResult& res, int src, int tgt, string mode, int departure_slot) {
//     json j;

//     if (res.path.empty()) {
//         j["status"] = "unreachable";
//         j["source"] = src;
//         j["target"] = tgt;
//     } else {
//         j["status"] = "success";
//         j["source"] = src;
//         j["target"] = tgt;
//         j["path"] = res.path;
//         j["total_cost"] = res.cost;
//         j["mode"] = mode;
//         j["departure_slot"] = departure_slot;
//     }

//     return j;
// }
