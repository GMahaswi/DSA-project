#include "graph.hpp"
#include "nlohmann/json.hpp"
using json = nlohmann::json;

void Graph::loadGraph(const string& filename) {
    ifstream f(filename);
    if (!f.is_open()) {
        cerr << "Error: Cannot open " << filename << endl;
        exit(1);
    }

    json data;
    f >> data;

    // Load nodes
    for (auto& node : data["nodes"]) {
        int id = node["id"];
        double lat = node["lat"], lon = node["lon"];
        coords[id] = {lat, lon};
        if (node.contains("pois"))
            for (auto& p : node["pois"])
                pois[id].push_back(p);
    }

    // Load edges
    for (auto& e : data["edges"]) {
        Edge edge;
        edge.id = e["id"];
        edge.u = e["u"];
        edge.v = e["v"];
        edge.length = e["length"];
        edge.average_time = e["average_time"];
        edge.oneway = e["oneway"];
        edge.road_type = e["road_type"];

        if (e.contains("speed_profile"))
            for (auto& sp : e["speed_profile"])
                edge.speed_profile.push_back(sp);

        adj[edge.u].push_back(edge);
        if (!edge.oneway) {
            Edge rev = edge;
            rev.u = edge.v;
            rev.v = edge.u;
            adj[rev.u].push_back(rev);
        }
        edge_map[edge.id] = edge;
    }
}

bool Graph::removeEdge(int edge_id) {
    if (!edge_map.count(edge_id)) return false;
    Edge e = edge_map[edge_id];
    removed_edges[edge_id] = e;
    edge_map.erase(edge_id);
    
    auto& vec = adj[e.u];
    vec.erase(remove_if(vec.begin(), vec.end(),
                        [&](const Edge& ed) { return ed.id == edge_id; }),
              vec.end());
    if (!e.oneway) {
        auto& back = adj[e.v];
        back.erase(remove_if(back.begin(), back.end(),
                             [&](const Edge& ed) {
                                 return ed.v == e.u && ed.id == edge_id;
                             }),
                   back.end());
    }
    return true;
}

bool Graph::modifyEdge(int edge_id, double new_length, double new_avg_time) { // ✅ return type changed
    if (edge_map.count(edge_id)) {
        Edge& e = edge_map[edge_id];
        if (new_length > 0) e.length = new_length;
        if (new_avg_time > 0) e.average_time = new_avg_time;
        e.active = true;

        for (Edge& ed : adj[e.u])
            if (ed.id == edge_id) {
                ed.length = e.length;
                ed.average_time = e.average_time;
            }
        if (!e.oneway) {
            for (Edge& ed : adj[e.v])
                if (ed.v == e.u && ed.id == edge_id) {
                    ed.length = e.length;
                    ed.average_time = e.average_time;
                }
        }
        return true;
    } else if (removed_edges.count(edge_id)) {
        Edge e = removed_edges[edge_id];
        if (new_length > 0) e.length = new_length;
        if (new_avg_time > 0) e.average_time = new_avg_time;
        e.active = true;

        adj[e.u].push_back(e);
        if (!e.oneway) {
            Edge rev = e;
            rev.u = e.v;
            rev.v = e.u;
            adj[rev.u].push_back(rev);
        }

        edge_map[edge_id] = e;
        removed_edges.erase(edge_id);
        return true;
    }
    return false;
}

double Graph::euclideanDistance(int a, int b) const {
    const double R = 6371.0;
    auto &na = coords.at(a);
    auto &nb = coords.at(b);

    double lat1 = na.first * M_PI / 180.0;
    double lon1 = na.second * M_PI / 180.0;
    double lat2 = nb.first * M_PI / 180.0;
    double lon2 = nb.second * M_PI / 180.0;

    double dLat = lat2 - lat1;
    double dLon = lon2 - lon1;

    double h = sin(dLat / 2.0) * sin(dLat / 2.0) +
               cos(lat1) * cos(lat2) *
               sin(dLon / 2.0) * sin(dLon / 2.0);

    double c = 2.0 * atan2(sqrt(h), sqrt(1 - h));
    return R * c;
}
double Graph::euclideanDistanceLatLon(double lat1, double lon1, double lat2, double lon2) const {
    const double R = 6371.0; // km
    lat1 = lat1 * M_PI / 180.0;
    lon1 = lon1 * M_PI / 180.0;
    lat2 = lat2 * M_PI / 180.0;
    lon2 = lon2 * M_PI / 180.0;

    double dLat = lat2 - lat1;
    double dLon = lon2 - lon1;

    double h = sin(dLat/2) * sin(dLat/2) + cos(lat1) * cos(lat2) * sin(dLon/2) * sin(dLon/2);
    double c = 2 * atan2(sqrt(h), sqrt(1-h));

    return R * c;
}

tuple<bool, double, vector<int>> Graph::shortestPath(
    int source, int target,
    const string& mode,
    const unordered_set<int>& forbidden_nodes,
    const unordered_set<string>& forbidden_roads
) const {
    unordered_map<int, double> dist;
    unordered_map<int, int> parent;
    for (auto& p : coords)
        dist[p.first] = numeric_limits<double>::infinity();
    dist[source] = 0;

    using P = pair<double, int>;
    priority_queue<P, vector<P>, greater<P>> pq;
    pq.push({0, source});

    while (!pq.empty()) {
        auto [d, u] = pq.top();
        pq.pop();
        if (d > dist[u]) continue;
        if (u == target) break;
        if (forbidden_nodes.count(u)) continue;
        if (!adj.count(u)) continue;

        for (auto& e : adj.at(u)) {
            if (!e.active) continue;
            if (forbidden_roads.count(e.road_type)) continue;
            double w = (mode == "time") ? e.average_time : e.length;
            if (dist[e.v] > d + w) {
                dist[e.v] = d + w;
                parent[e.v] = u;
                pq.push({dist[e.v], e.v});
            }
        }
    }

    if (dist[target] == numeric_limits<double>::infinity())
        return {false, -1.0, {}};

    vector<int> path;
    for (int cur = target; cur != source; cur = parent[cur])
        path.push_back(cur);
    path.push_back(source);
    reverse(path.begin(), path.end());

    return {true, dist[target], path};
}


// ---------------------------------
// K Shortest Paths (Exact) - remove const
// ---------------------------------
vector<Path> Graph::kShortestPathsExact(int source, int target, int k, const string& mode) {
    vector<Path> result;
    auto [found, dist, path] = shortestPath(source, target, mode);
    if (!found) return result;
    result.push_back({path, dist});

    using Candidate = pair<double, vector<int>>;
    set<vector<int>> visited;
    visited.insert(path);
    priority_queue<Candidate, vector<Candidate>, greater<Candidate>> candidates;

    for (int i = 1; i < k; i++) {
        for (size_t j = 0; j < result.back().nodes.size()-1; j++) {
            int spur_node = result.back().nodes[j];
            vector<int> root_path(result.back().nodes.begin(), result.back().nodes.begin()+j+1);

            // Temporarily remove edges
            unordered_set<int> removed_edge_ids;
            for (auto& p : result) {
                if (p.nodes.size()>j && equal(root_path.begin(), root_path.end(), p.nodes.begin())) {
                    int u = p.nodes[j], v = p.nodes[j+1];
                    for (auto& e : adj[u])
                        if (e.v==v && e.active) removed_edge_ids.insert(e.id);
                }
            }
            for (int eid : removed_edge_ids) removeEdge(eid);

            auto [found_spur, spur_dist, spur_path] = shortestPath(spur_node, target, mode);
            if (found_spur) {
                vector<int> total_path = root_path;
                total_path.insert(total_path.end(), spur_path.begin()+1, spur_path.end());
                if (!visited.count(total_path)) {
                    visited.insert(total_path);
                    candidates.push({spur_dist, total_path});
                }
            }

            // Restore edges
            for (int eid : removed_edge_ids) modifyEdge(eid, edge_map[eid].length, edge_map[eid].average_time);
        }

        if (candidates.empty()) break;
        auto [d,p] = candidates.top(); candidates.pop();
        result.push_back({p,d});
    }
    return result;
}

// ---------------------------------
// K Shortest Paths (Heuristic)
// ---------------------------------
vector<Path> Graph::kShortestPathsHeuristic(int source, int target, int k, int overlap_threshold, const string& mode) {
    vector<Path> exact_paths = kShortestPathsExact(source, target, 20, mode);
    vector<Path> result;
    if (exact_paths.empty()) return result;

    result.push_back(exact_paths[0]);
    for (auto& p : exact_paths) {
        if (result.size() >= (size_t)k) break;
        int overlap_count = 0;
        for (auto& r : result) {
            unordered_set<int> edges_r, edges_p;
            for (size_t i=0;i<r.nodes.size()-1;i++) edges_r.insert(r.nodes[i]*100000+r.nodes[i+1]);
            for (size_t i=0;i<p.nodes.size()-1;i++) edges_p.insert(p.nodes[i]*100000+p.nodes[i+1]);
            int cnt=0;
            for(int e: edges_p) if(edges_r.count(e)) cnt++;
            double overlap_pct = cnt*100.0 / max(r.nodes.size()-1, p.nodes.size()-1);
            if(overlap_pct>overlap_threshold) overlap_count++;
        }
        double distance_penalty = (p.length - result[0].length)/result[0].length + 0.1;
        double penalty = overlap_count * distance_penalty;
        if (penalty < 1.0 || result.size() < (size_t)k) result.push_back(p);
    }
    return result;
}

// ---------------------------------
// Approximate Shortest Paths
// ---------------------------------
vector<double> Graph::approxShortestPaths(const vector<pair<int,int>>& queries, int time_budget_ms,
                                          double acceptable_error_pct, const string& mode) {
    vector<double> result;
    auto start_time = chrono::steady_clock::now();

    for (auto& q : queries) {
        auto [found, dist, path] = shortestPath(q.first, q.second, mode);
        if (!found) result.push_back(-1);
        else {
            double approx_dist = dist*(1.0 + (rand()%1000)/100000.0);
            if (abs(approx_dist-dist)/dist*100 > acceptable_error_pct)
                approx_dist = dist*(1.0 + acceptable_error_pct/100.0);
            result.push_back(approx_dist);
        }
        auto elapsed = chrono::duration_cast<chrono::milliseconds>(
                        chrono::steady_clock::now() - start_time).count();
        if (elapsed > time_budget_ms) break;
    }
    return result;
}