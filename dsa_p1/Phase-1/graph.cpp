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
        edge.active=true;

        if (e.contains("speed_profile"))
            for (auto& sp : e["speed_profile"])
                edge.speed_profile.push_back(sp);

        adj[edge.u].push_back(edge);
        if (!edge.oneway) {
            Edge rev = edge;
            rev.u = edge.v;
            rev.v = edge.u;
            rev.active=true;
            adj[rev.u].push_back(rev);
        }
        edge_map[edge.id] = edge;
    }
}

double Graph::travelTimeWithProfile(const Edge& e, double t_start) const {
    if (e.speed_profile.empty())
        return e.average_time;  

    const int SLOT_SECS = 900;  
    double remaining = e.length;
    double t_current = t_start;

    for (int iter = 0; iter < 1000 && remaining > 1e-6; ++iter) {
        int slot = int(t_current / SLOT_SECS) % 96;
        double v = e.speed_profile[slot];

        if (v <= 1e-9) { 

            t_current += 1.0;
            continue;
        }

        double slot_end = (slot + 1) * SLOT_SECS;
        double time_left = slot_end - t_current;
        double dist_possible = v * time_left;

        if (dist_possible >= remaining) {
            double time_needed = remaining / v;
            t_current += time_needed;
            remaining = 0;
        } else {
            t_current += time_left;
            remaining -= dist_possible;
        }
    }

    return t_current - t_start;
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
                                 return ed.id == edge_id;
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
        return true;
    }  else if (removed_edges.count(edge_id)) {
        Edge e = removed_edges[edge_id];
        if (new_length > 0) e.length = new_length;
        if (new_avg_time > 0) e.average_time = new_avg_time;
        e.active = true;

        adj[e.u].push_back({e.v, e.id});
        if (!e.oneway)
            adj[e.v].push_back({e.u, e.id});
        edge_map[edge_id] = e;
        removed_edges.erase(edge_id);
        return true;
    }
    return false;
}

double Graph::euclideanDistance(int a, int b) const {
    const double R = 6371000.0;
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
    const double R = 6371000.0; 
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


tuple<bool, double, vector<int>> Graph::shortestPath(int source, int target,const string& mode,const unordered_set<int>& forbidden_nodes,const unordered_set<string>& forbidden_roads) const {
    if (forbidden_nodes.count(source) || forbidden_nodes.count(target))
        return {false, -1.0, {}};

    if (source == target)
        return {true, 0.0, {source}};  
        
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
        if (forbidden_nodes.count(u)) continue;
        if (!adj.count(u)) continue;

        for (auto& e : adj.at(u)) {
            if (!e.active) continue;
            if (forbidden_roads.count(e.road_type)) continue;
            if (forbidden_nodes.count(e.v)) continue;
            double w;
            if (mode == "time") {
                w = travelTimeWithProfile(e, dist[u]);
            } 
            else {
                w = e.length;
            }
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
    int cur = target;
    while (cur != source) {
        if (!parent.count(cur))
            return {false, -1.0, {}};
        path.push_back(cur);
        cur = parent[cur];
    }
    path.push_back(source);
    reverse(path.begin(), path.end());
    return {true, dist[target], path};
}