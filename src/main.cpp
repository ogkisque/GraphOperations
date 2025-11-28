#include <iostream>
#include <string>
#include "graph.hpp"
#include "dotter.hpp"

int main()
{
    graph::Graph graph;
    graph.dump_txt();
    std::string graph1_name = "input_graph";
    graph.dump_dot(graph1_name);

    std::cout << "\n";
    graph.add_start_end();
    graph.dump_txt();
    std::string graph2_name = "start_end";
    graph.dump_dot(graph2_name);

    std::cout << "\n";
    graph.topological_renumber();
    graph.dump_txt();
    std::string graph3_name = "topological_sort";
    graph.dump_dot(graph3_name);

    std::cout << "\n";
    auto doms = graph.build_dominator_tree();
    for (auto& [idx, dom] : doms)
    {
        std::cout << idx << " ; " << dom << std::endl;
    }

    std::cout << "\n";
    auto post_doms = graph.build_postdominator_tree();
    for (auto& [idx, dom] : post_doms)
    {
        std::cout << idx << " ; " << dom << std::endl;
    }

    std::string graph4_name = "dom_tree";
    std::string graph5_name = "postdom_tree";
    graph::Graph::dump_tree_dot(graph4_name, doms);
    graph::Graph::dump_tree_dot(graph5_name, post_doms);

    return 0;
}