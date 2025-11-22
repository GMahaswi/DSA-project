#include <nlohmann/json.hpp>
#include <iostream>
#include <fstream>
#include <chrono>
#include "graph.hpp"
#include "queries.hpp"

using json = nlohmann::json;

int main(int argc, char* argv[]) {

    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <graph.json> <queries.json>\n";
        return 1;
    }

    // ---------------- Load Graph ----------------
    Graph g;
    g.loadGraph(argv[1]);

    // ---------------- Load Queries ----------------
    std::ifstream queries_file(argv[2]);
    if (!queries_file.is_open()) {
        std::cerr << "Failed to open " << argv[2] << std::endl;
        return 1;
    }

    json queries_json;
    queries_file >> queries_json;
    queries_file.close();

    // ---------------- Open Output File ----------------
    std::ofstream output_file("output.json");
    if (!output_file.is_open()) {
        std::cerr << "Failed to open output.json for writing\n";
        return 1;
    }

    output_file << "{\n";
    output_file << "  \"meta\": " << queries_json["meta"].dump(4) << ",\n";
    output_file << "  \"results\": [\n";

    bool first = true;

    // ---------------- Process Events ----------------
    for (const auto& query : queries_json["events"]) {

        auto start_time = std::chrono::high_resolution_clock::now();

        json result = process_query(g, query);

        auto end_time = std::chrono::high_resolution_clock::now();
        result["processing_time"] = 
            std::chrono::duration<double, std::milli>(end_time - start_time).count();

        if (!first) output_file << ",\n";
        first = false;

        output_file << result.dump(4);
        output_file.flush();
    }

    output_file << "\n  ]\n}\n";
    output_file.close();

    std::cout << "✔ Processing complete. Output written to output.json\n";
    return 0;
}
