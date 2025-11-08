#include <nlohmann/json.hpp>
#include <iostream>
#include <fstream>
#include <chrono>
#include "graph.hpp" 
#include "queries.hpp"
/*
    Add other includes that you require, only write code wherever indicated
*/

using json = nlohmann::json;

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <graph.json> <queries.json>" << std::endl;
        return 1;
    }
    Graph g;
    g.loadGraph(argv[1]);
    // Read graph from first file
    /*
        Add your graph reading and processing code here
        Initialize any classes and data structures needed for query processing
    */

    // Read queries from second file

    std::ifstream queries_file(argv[2]);
    if (!queries_file.is_open()) {
        std::cerr << "Failed to open " << argv[2] << std::endl;
        return 1;
    }
    json queries_json;
    queries_file >> queries_json;
    queries_file.close();

    std::ofstream output_file("output.json");
    if (!output_file.is_open()) {
        std::cerr << "Failed to open output.json for writing" << std::endl;
        return 1;
    }

    output_file << "{\n";
    output_file << "  \"meta\": " << queries_json["meta"].dump(4) << ",\n";
    output_file << "  \"results\": [\n";

    bool first = true;

    for (const auto& query : queries_json["events"]) {
        auto start_time = std::chrono::high_resolution_clock::now();

        /*
            Add your query processing code here
            Each query should return a json object which should be printed to sample.json
        */

        // Answer each query replacing the function process_query using 
        // whatever function or class methods that you have implemented
        json result = process_query(g,query);

        auto end_time = std::chrono::high_resolution_clock::now();
        result["processing_time"] = std::chrono::duration<double, std::milli>(end_time - start_time).count();

        if (!first) output_file << ",\n";
        first = false;

        output_file << result.dump(4) << '\n';
        output_file.flush(); 
    }

    output_file << "\n  ]\n}\n";
    output_file.close();
    return 0;
}