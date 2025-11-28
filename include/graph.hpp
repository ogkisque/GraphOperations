#pragma once

#include <iostream>
#include <vector>
#include <unordered_map>
#include <string>
#include <sstream>
#include <unordered_set>
#include <limits>
#include <queue>
#include <cassert>
#include <stack>
#include <algorithm>
#include <functional>

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
        std::unordered_set<idx_t> no_parent(all_nodes_);
        std::unordered_set<idx_t> no_child(all_nodes_);

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

    void topological_renumber()
    {
        std::unordered_map<idx_t, int> color;
        color.reserve(all_nodes_.size());
        for (auto n : all_nodes_)
            color[n] = 0;

        std::vector<idx_t> stack;
        stack.reserve(all_nodes_.size());
        bool has_cycle = false;

        std::function<void(idx_t)> dfs = [&](idx_t u)
        {
            color[u] = 1;
            auto it = adjacency_lists_.find(u);
            if (it != adjacency_lists_.end())
            {
                for (auto child : it->second)
                {
                    int child_color = color[child];
                    if (child_color == 0)
                    {
                        dfs(child);
                        if (has_cycle)
                            return;
                    }
                    else if (child_color == 1) // значит back-edge
                    {
                        has_cycle = true;
                        return;
                    }
                }
            }
            color[u] = 2;
            stack.push_back(u);
        };

        for (auto n : all_nodes_)
        {
            if (color[n] == 0)
            {
                dfs(n);
                if (has_cycle)
                    throw std::runtime_error("Graph contains a cycle");
            }
        }
        std::reverse(stack.begin(), stack.end());

        std::unordered_map<idx_t, idx_t> new_id;
        new_id.reserve(stack.size());
        for (idx_t i = 0; i < stack.size(); ++i)
            new_id[stack[i]] = i;

        std::unordered_map<idx_t, std::vector<idx_t>> new_adj;
        new_adj.reserve(stack.size());
        for (auto old_u : stack)
        {
            auto new_u = new_id[old_u];
            auto it = adjacency_lists_.find(old_u);
            if (it == adjacency_lists_.end())
            {
                new_adj[new_u] = {};
            }
            else
            {
                std::vector<idx_t> new_neighbors;
                new_neighbors.reserve(it->second.size());
                for (auto old_v : it->second)
                    new_neighbors.push_back(new_id[old_v]);
                new_adj[new_u] = std::move(new_neighbors);
            }
        }

        adjacency_lists_ = std::move(new_adj);
        update_all_nodes();
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

    std::unordered_map<idx_t, idx_t> build_dominator_tree() const
    {
        return build_dominator_tree_helper(adjacency_lists_, 0, false);
    }

    std::unordered_map<idx_t, idx_t> build_postdominator_tree() const
    {
        std::unordered_map<idx_t, std::vector<idx_t>> rev;
        rev.reserve(adjacency_lists_.size() * 2);

        for (const auto &kv : adjacency_lists_)
        {
            idx_t u = kv.first;
            if (rev.find(u) == rev.end())
                rev[u] = {};
            for (idx_t v : kv.second) {
                rev[v].push_back(u);
            }
        }

        idx_t exit = -1;
        for (auto& [idx, idxs] : adjacency_lists_)
            if (idxs.size() == 0) exit = idx;

        return build_dominator_tree_helper(rev, exit, true);
    }

    static void
    dump_tree_dot(std::string& graph_name, std::unordered_map<idx_t, idx_t> tree)
    {
        dotter::Dotter dotter;

        for (auto& [idx1, idx2] : tree)
        {
            dotter.AddNode("Node " + std::to_string(idx1), idx1);
        }

        for (auto& [idx1, idx2] : tree)
            dotter.AddLink(idx1, idx2);

        std::string dot_name = graph_name + ".dot";
        std::string png_name = graph_name + ".png";
        dotter.PrintDotText(dot_name);
        dotter.Render(dot_name, png_name);
    }

private:
    void update_all_nodes()
    {
        all_nodes_.clear();
        for (auto& [idx, idxs] : adjacency_lists_)
        {
            all_nodes_.insert(idx);
            for (auto& child : idxs)
            {
                all_nodes_.insert(child);
            }
        }
    }

    std::unordered_map<idx_t, idx_t>
    build_dominator_tree_helper(const std::unordered_map<idx_t, std::vector<idx_t>> &adj,
                                idx_t entry, bool order_desc) const
    {
        std::unordered_map<idx_t, std::vector<idx_t>> preds;
        std::vector<idx_t> all_nodes(all_nodes_.begin(), all_nodes_.end());
        all_nodes.reserve(adj.size());
        for (auto &[idx, idxs] : adj)
        {
            for (auto child : idxs)
                preds[child].push_back(idx);
        }

        if (order_desc)
            std::sort(all_nodes.begin(), all_nodes.end(), std::greater<idx_t>());
        else
            std::sort(all_nodes.begin(), all_nodes.end());

        const idx_t INVALID = std::numeric_limits<idx_t>::max();
        std::unordered_map<idx_t, idx_t> doms;
        doms.reserve(all_nodes.size());
        doms[entry] = entry;

        for (size_t i = 0; i < all_nodes.size(); i++)
        {
            idx_t n = all_nodes[i];
            if (n == entry) continue;
            
            idx_t new_dom = INVALID;
            auto itp = preds.find(n);
            assert(itp != preds.end());

            for (auto pred : itp->second)
            {
                if (doms.find(pred) == doms.end()) continue;
                if (new_dom == INVALID)
                    new_dom = pred;
                else
                    new_dom = intersect(pred, new_dom, doms, all_nodes);
            }

            assert(new_dom != INVALID);
            doms[n] = new_dom;
        }

        return doms;
    }

    idx_t intersect(idx_t a, idx_t b,
                    std::unordered_map<idx_t, idx_t>& doms,
                    const std::vector<idx_t>& pos) const
    {
        while (a != b)
        {
            while (pos[a] < pos[b])
                b = doms[b];

            while (pos[b] < pos[a])
                a = doms[a];
        }
        return a;
    };

    std::unordered_map<idx_t, std::vector<idx_t>> adjacency_lists_;
    std::unordered_set<idx_t> all_nodes_;

}; // class Graph

}