#include <nlohmann/json.hpp>
#include <iostream>
#include <fstream>
#include <chrono>
#include "graph.hpp" 
/*
    Add other includes that you require, only write code wherever indicated
*/

using json = nlohmann::json;
Graph g;
json process_query(const json& query) {
    json result;

    string type = query["type"];

    if (type == "remove_edge") {
        int edge_id = query["edge_id"];
        g.removeEdge(edge_id);
        result["status"] = "edge_removed";
        result["edge_id"] = edge_id;
    } 
    else if (type == "modify_edge") {
        int edge_id = query["edge_id"];
        double new_length = query.value("new_length", -1.0);
        double new_avg_time = query.value("new_avg_time", -1.0);
        g.modifyEdge(edge_id, new_length, new_avg_time);
        result["status"] = "edge_modified";
        result["edge_id"] = edge_id;
    } 
    else {
        result["status"] = "unknown_query_type";
    }

    return result;
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <graph.json> <queries.json>" << std::endl;
        return 1;
    }

    // Read graph from first file
    /*
        Add your graph reading and processing code here
        Initialize any classes and data structures needed for query processing
    */

    // Read queries from second file
    g.loadGraph(argv[1]);

    std::ifstream queries_file(argv[2]);
    if (!queries_file.is_open()) {
        std::cerr << "Failed to open " << argv[2] << std::endl;
        return 1;
    }
    json queries_json;
    queries_file >> queries_json;

    std::ofstream output_file("output.json");
    if (!output_file.is_open()) {
        std::cerr << "Failed to open output.json for writing" << std::endl;
        return 1;
    }

    for (const auto& query : queries_json) {
        auto start_time = std::chrono::high_resolution_clock::now();

        /*
            Add your query processing code here
            Each query should return a json object which should be printed to sample.json
        */

        // Answer each query replacing the function process_query using 
        // whatever function or class methods that you have implemented
        json result = process_query(query);

        auto end_time = std::chrono::high_resolution_clock::now();
        result["processing_time"] = std::chrono::duration<double, std::milli>(end_time - start_time).count();

        output_file << result.dump(4) << '\n';
    }

    output_file.close();
    return 0;
}