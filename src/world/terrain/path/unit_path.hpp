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
 * @file unit_path.hpp
 *
 * @author @AlemSnyder
 *
 * @brief Defines UnitPath, and DirectionalFlags
 *
 */

#pragma once
#include "types.hpp"

#include <cstdint>

namespace terrain {

/**
 * @brief enum class defining possible directions between two tiles
 *
 */
enum class DirectionFlags : path_t {
    NONE = 0,
    OPEN = 1 << 0,
    HORIZONTAL1 = 1 << 1,
    HORIZONTAL2 = 1 << 2,
    VERTICAL = 1 << 3,
    UP_AND_OVER = 1 << 4,
    UP_AND_DIAGONAL = 1 << 5,
};

inline DirectionFlags
operator|(DirectionFlags lhs, DirectionFlags rhs) {
    return static_cast<DirectionFlags>(
        static_cast<path_t>(lhs) | static_cast<path_t>(rhs)
    );
}

inline DirectionFlags
operator&(DirectionFlags lhs, DirectionFlags rhs) {
    return static_cast<DirectionFlags>(
        static_cast<path_t>(lhs) & static_cast<path_t>(rhs)
    );
}

inline DirectionFlags
operator~(DirectionFlags val) {
    return static_cast<DirectionFlags>(~static_cast<path_t>(val));
}

/**
 * @brief Holds path types between two adjacent tiles
 *
 * @details This class also holds information on weather a path type exists
 * between any two nodes in a NodeGroup
 *
 */
class UnitPath {
 private:
    // the type of path
    DirectionFlags type;

 public:
    /**
     * @brief Construct a new Unit Path object (default initializer)
     *
     */
    UnitPath() : type(DirectionFlags::NONE) {}

    /**
     * @brief Construct a new Unit Path object
     *
     * @param type_ path type between the tiles
     */
    UnitPath(path_t type_) : type(static_cast<DirectionFlags>(type_)) {}

    /**
     * @brief Construct a new Unit Path object
     *
     * @param type_ path type between the tiles
     */
    UnitPath(DirectionFlags type_) : type(type_) {}

    // is the path open
    inline bool
    is_open() const {
        return ((DirectionFlags::OPEN & type) == DirectionFlags::OPEN);
    }

    // is the path vertical, and diagonal
    inline bool
    is_up_diagonal() const {
        return ((DirectionFlags::UP_AND_DIAGONAL & type))
               == DirectionFlags::UP_AND_DIAGONAL;
    }

    // is the path vertical, but not diagonal
    inline bool
    is_up_over() const {
        return ((DirectionFlags::UP_AND_OVER & type)) == DirectionFlags::UP_AND_OVER;
    }

    // is the path diagonal, and horizontal
    inline bool
    is_diagonal() const {
        return ((DirectionFlags::HORIZONTAL2 & type)) == DirectionFlags::HORIZONTAL2;
    }

    // is the path vertical
    inline bool
    is_up() const {
        return ((DirectionFlags::VERTICAL & type) == DirectionFlags::VERTICAL);
    }

    // is the path horizontal
    inline bool
    is_level() const {
        return ((DirectionFlags::VERTICAL & type) == DirectionFlags::NONE);
    }

    // set the openness to closed
    inline void
    close() {
        type = type & (~DirectionFlags::OPEN);
    }

    // set the openness to open
    inline void
    open() {
        type = type | DirectionFlags::OPEN;
    }

    // return the path represented as a path_t
    inline path_t
    get_type() const {
        return static_cast<path_t>(type);
    }

    // is this path compatible with the given test
    // same as is path::type a subset of test
    inline bool
    compatible(DirectionFlags test) const {
        return !bool(type & ~test);
    };

    // is this path compatible with the given test
    // same as is path::type a subset of test
    inline bool
    compatible(path_t test) const {
        return compatible(static_cast<DirectionFlags>(test));
    }

    // is this path compatible with the given test
    // same as is path::type a subset of test
    inline bool
    compatible(UnitPath test) const {
        return compatible(test.type);
    }

    bool
    operator==(const UnitPath other) const {
        return other.type == this->type;
    }

    bool
    operator==(const path_t other) const {
        return static_cast<DirectionFlags>(other) == this->type;
    }

    bool
    operator==(const DirectionFlags other) const {
        return other == this->type;
    }

    UnitPath
    operator&(const UnitPath other) const {
        return type & other.type;
    }

    UnitPath
    operator|(const UnitPath other) const {
        return type | other.type;
    }
};

} // namespace terrain
