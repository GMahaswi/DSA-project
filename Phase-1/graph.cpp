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

    for (auto& node : data["nodes"]) {
        int id = node["id"];
        double lat = node["lat"], lon = node["lon"];
        coords[id] = {lat, lon};
        if (node.contains("pois"))
            for (auto& p : node["pois"])
                pois[id].push_back(p);
    }

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

void Graph::removeEdge(int edge_id) {
    if (!edge_map.count(edge_id)) return;
    Edge e = edge_map[edge_id];
    auto& vec = adj[e.u];
    vec.erase(remove_if(vec.begin(), vec.end(), [&](const Edge& ed) {
        return ed.id == edge_id;
    }), vec.end());
    if (!e.oneway) {
        auto& back = adj[e.v];
        back.erase(remove_if(back.begin(), back.end(), [&](const Edge& ed) {
            return ed.v == e.u && ed.id == edge_id;
        }), back.end());
    }
    edge_map.erase(edge_id);
}

void Graph::modifyEdge(int edge_id, double new_length, double new_avg_time) {
    if (!edge_map.count(edge_id)) return;
    Edge& e = edge_map[edge_id];
    e.length = new_length;
    e.average_time = new_avg_time;

    // update both adjacency lists
    for (Edge& ed : adj[e.u])
        if (ed.id == edge_id) {
            ed.length = new_length;
            ed.average_time = new_avg_time;
        }
    if (!e.oneway) {
        for (Edge& ed : adj[e.v])
            if (ed.v == e.u && ed.id == edge_id) {
                ed.length = new_length;
                ed.average_time = new_avg_time;
            }
    }
}
