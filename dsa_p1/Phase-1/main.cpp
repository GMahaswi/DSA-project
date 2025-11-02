#include "graph.hpp"
#include "queries.hpp"
#include "nlohmann/json.hpp"
using namespace std;
using json = nlohmann::json;

int main(int argc, char* argv[]) {
    if (argc < 3) {
        cerr << "Usage: ./phase1 <graph.json> <queries.json>\n";
        return 1;
    }

    string graph_file = argv[1];
    string query_file = argv[2];

    Graph G;
    G.loadGraph(graph_file);

    ifstream qf(query_file);
    if (!qf.is_open()) {
        cerr << "Error: Cannot open " << query_file << endl;
        return 1;
    }

    json queries;
    qf >> queries;
    json output;
    output["meta"] = queries["meta"];
    output["results"] = json::array();

    for (auto& ev : queries["events"]) {
        string type = ev["type"];
        json res;
        res["id"] = ev["id"];

        if (type == "shortest_path") {
            int src = ev["source"], tgt = ev["target"];
            string mode = ev["mode"];
            PathResult ans = shortestPath(G, src, tgt, mode);
            res["path"] = ans.path;
            res["cost"] = ans.cost;
        }
        else if (type == "remove_edge") {
            int edge_id = ev["edge_id"];
            G.removeEdge(edge_id);
            res["status"] = "removed";
        }
        else if (type == "modify_edge") {
            int edge_id = ev["edge_id"];
            double new_length = ev["new_length"];
            double new_avg_time = ev["new_average_time"];
            G.modifyEdge(edge_id, new_length, new_avg_time);
            res["status"] = "modified";
        }
        else {
            res["error"] = "Unknown query type";
        }
        output["results"].push_back(res);
    }

    cout << output.dump(4);
}
