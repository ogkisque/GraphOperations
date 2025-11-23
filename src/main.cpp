#include <iostream>
#include "graph.hpp"
#include "dotter.hpp"

int main()
{
    graph::Graph graph;
    graph.dump_txt();

    graph.dump_dot();

    return 0;
}