#pragma once

#include <iostream>
#include <vector>
#include <unordered_map>
#include <string>
#include <sstream>
#include <set>

#include "dotter.hpp"

namespace graph
{

class Graph
{
    using idx_t = int;
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

    void dump_dot()
    {
        dotter::Dotter dotter;
        for (auto& [idx, idxs] : adjacency_lists_)
        {
            dotter.AddNode("Node " + std::to_string(idx), idx);

            for (auto& child : idxs)
            {
                dotter.AddNode("Node " + std::to_string(child), child);
                dotter.AddLink(idx, child);
            }
        }
        dotter.PrintDotText();
        dotter.Render();
    }

private:
    std::unordered_map<idx_t, std::vector<idx_t>> adjacency_lists_;

}; // class Graph

}