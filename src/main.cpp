#include <iostream>
#include <string>
#include "graph.hpp"
#include "dotter.hpp"

int main()
{
    graph::Graph graph;
    graph.dump_txt();
    std::string graph1_name = "graph1";
    graph.dump_dot(graph1_name);

    std::cout << "\n";
    graph.add_start_end();
    graph.dump_txt();
    std::string graph2_name = "graph2";
    graph.dump_dot(graph2_name);

    return 0;
}