#pragma once

#include <iostream>
#include <vector>
#include <unordered_map>
#include <string>
#include <sstream>
#include <set>
#include <limits>

#include "dotter.hpp"

namespace graph
{

class Graph
{
    using idx_t = int;
    const idx_t START_IDX = std::numeric_limits<idx_t>::min();
    const idx_t END_IDX = std::numeric_limits<idx_t>::max();
public:
    Graph()
    {
        std::string line;
        while (std::getline(std::cin, line))
        {
            if (line.empty())
                continue;

            std::vector<idx_t> idxs;
            std::istringstream iss(line);
            idx_t tmp;
            bool fl = true;
            idx_t idx;

            while (iss >> tmp)
            {
                if (fl)
                {
                    fl = false;
                    idx = tmp;
                }
                else
                {
                    adjacency_lists_[idx].push_back(tmp);
                }
            }
        }

        for (auto& [idx, idxs] : adjacency_lists_)
        {
            all_nodes_.insert(idx);
            for (auto& child : idxs)
            {
                all_nodes_.insert(child);
            }
        }
    }

    void add_start_end()
    {
        std::set<idx_t> no_parent(all_nodes_);
        std::set<idx_t> no_child(all_nodes_);

        for (auto& [idx, idxs] : adjacency_lists_)
        {
            no_child.erase(idx);
            for (auto& child : idxs)
            {
                no_parent.erase(child);
            }
        }

        for (auto& idx : no_parent)
        {
            adjacency_lists_[START_IDX].push_back(idx);
        }

        for (auto& idx : no_child)
        {
            adjacency_lists_[idx].push_back(END_IDX);
        }

        all_nodes_.insert(START_IDX);
        all_nodes_.insert(END_IDX);
    }

    void dump_txt()
    {
        for (auto& [idx, idxs] : adjacency_lists_)
        {
            std::cout << idx << ": ";
            for (auto& child : idxs)
            {
                std::cout << child << " ";
            }
            std::cout << std::endl;
        }
    }

    void dump_dot(std::string& graph_name)
    {
        dotter::Dotter dotter;

        for (auto& idx : all_nodes_)
        {
            if (idx == START_IDX)
                dotter.AddNode("Start node", idx);
            else if (idx == END_IDX)
                dotter.AddNode("End node", idx);
            else
                dotter.AddNode("Node " + std::to_string(idx), idx);
        }

        for (auto& [idx, idxs] : adjacency_lists_)
            for (auto& child : idxs)
                dotter.AddLink(idx, child);

        std::string dot_name = graph_name + ".dot";
        std::string png_name = graph_name + ".png";
        dotter.PrintDotText(dot_name);
        dotter.Render(dot_name, png_name);
    }

private:
    std::unordered_map<idx_t, std::vector<idx_t>> adjacency_lists_;
    std::set<idx_t> all_nodes_;

}; // class Graph

}