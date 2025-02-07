// -*- lsst-c++ -*-
/*
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 */

/**
 * @file node.hpp
 *
 * @author @AlemSnyder
 *
 * @brief Defines Node class
 *
 * @ingroup Terrain
 *
 */

#pragma once

#include "types.hpp"
#include "node_group.hpp"

#include <unordered_set>

namespace terrain {

/**
 * @brief A node used to find a path with the A* or breadth first algorithm
 *
 * @details A node is used to find a path between two points. Nodes allows for
 * sorting searchable tiles in a way to select which T should be traveled
 * through
 *
 * @tparam T Underlying object, defines where the nodes exists in space
 */
template <class T>
class Node {            // Used to find paths.
    T* tile_;           // defines position in space
    Node* parent_node_; // previous step in fastest way to get to this nodes

    float g_cost_;  // Time from start to this node
    float h_cost_;  // Minimum time from this node to end
    float f_cost_;  // Minimum time from start to end through this node
    bool explored_; // Is there a path from start to this node?

 public:
    /**
     * @brief Construct a new Node object
     *
     * @param tile position of node in space
     * @param hc theoretical minimum time needed to reach goal
     */
    Node(T* tile, float hc) :
        tile_(tile), parent_node_(nullptr), g_cost_(0), h_cost_(hc),
        f_cost_(g_cost_ + h_cost_), explored_(false){};
    /**
     * @brief Construct a new Node object (default initializer)
     * @deprecated should not be used
     */
    Node() : Node(nullptr, 0){};

    /**
     * @brief Explore this node
     *
     * @param parent where this node was explored from
     * @param gc time required to reach this node from start
     */
    void explore(Node<T>* parent, float gc);
    /**
     * @brief Explore as if this is the starting position.
     *
     */
    void explore();

    /**
     * @brief Get the underlying position object
     *
     * @return T* underlying position structure
     */
    inline T*
    get_tile() {
        return tile_;
    }

    /**
     * @brief Get the underlying position object
     *
     * @return T* underlying position structure
     */
    inline const T*
    get_tile() const {
        return tile_;
    }

    /**
     * @brief Get the parent
     *
     * @return Node<T>* previous step in fastest way to get to this nodes
     */
    inline Node<T>*
    get_parent() {
        return parent_node_;
    }

    /**
     * @brief Get the time required to reach this node from start
     *
     * @return float time from start to this node
     */
    inline float
    get_time_cost() const {
        return g_cost_;
    }

    /**
     * @brief Get the predicted continue cots
     *
     * @return float minimum time from this node to end
     */
    inline float
    get_predicted_continue_cots() const {
        return h_cost_;
    }

    /**
     * @brief Get the predicted total cots
     *
     * @return float minimum time from start to end through this node
     */
    inline float
    get_total_predicted_cost() const {
        return f_cost_;
    }

    /**
     * @brief Has this node been explored
     *
     * @return true This node has been explored
     * @return false This node has not been explored
     */
    inline bool
    is_explored() const {
        return explored_;
    }
};

} // namespace terrain
