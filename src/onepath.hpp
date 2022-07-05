#ifndef __ONEPATH_HPP__
#define __ONEPATH_HPP__

#include <stdint.h>

enum class DirectionFlags : int8_t {
    NONE = 0,
    HORIZONTAL1 = 1 << 0,
    HORIZONTAL2 = 1 << 1,
    VERTICAL = 1 << 2,
    OPEN = 1 << 3,
    UP_AND_OVER = 1 << 4,
    UP_AND_DIAGONAL = 1 << 5,
};
inline DirectionFlags operator|(DirectionFlags lhs, DirectionFlags rhs) {
    return static_cast<DirectionFlags>(static_cast<int8_t>(lhs) |
                                       static_cast<int8_t>(rhs));
}
inline DirectionFlags operator&(DirectionFlags lhs, DirectionFlags rhs) {
    return static_cast<DirectionFlags>(static_cast<int8_t>(lhs) &
                                       static_cast<int8_t>(rhs));
}
inline DirectionFlags operator~(DirectionFlags val){
    return static_cast<DirectionFlags>(~static_cast<int8_t>(val));
}
//DirectionFlags operator!=(DirectionFlags lhs, DirectionFlags rhs){
//    return (static_cast<int8_t>(lhs)) == (static_cast<int8_t>(rhs));
//}

class OnePath {  // path type from adjacent nodes

    /*
    open
    0
    0
    up, diagonal
    up, and forward
    diagonal
    up
    forward
    */
private:
    DirectionFlags type;

public:
    // there should be not OnePath pointers
    OnePath() : type(DirectionFlags::NONE) {}
    OnePath(int8_t type_) : type(static_cast<DirectionFlags>(type_)) {}
    OnePath(DirectionFlags type_) : type(type_) {}

    inline bool is_open() const {
        return ((DirectionFlags::OPEN & type) == DirectionFlags::OPEN);
    }
    inline bool is_up_diagonal() const {
        return (
            ((DirectionFlags::HORIZONTAL2 | DirectionFlags::VERTICAL) & type) ==
            (DirectionFlags::HORIZONTAL2 | DirectionFlags::VERTICAL));
    }
    inline bool is_up_over() const {
        return (((DirectionFlags::HORIZONTAL1 | DirectionFlags::VERTICAL) &
                type) ==
                    (DirectionFlags::HORIZONTAL1 | DirectionFlags::VERTICAL));
    }
    inline bool is_diagonal() const {
        return (((DirectionFlags::HORIZONTAL2 | DirectionFlags::VERTICAL) &
                type) == DirectionFlags::HORIZONTAL2);
    }
    inline bool is_up() const { return ((DirectionFlags::VERTICAL & type) == DirectionFlags::VERTICAL); }
    inline bool is_level() const { return ((DirectionFlags::VERTICAL & type) == DirectionFlags::NONE); }

    inline void close() { type = type & (DirectionFlags::HORIZONTAL1 | DirectionFlags::HORIZONTAL2 | DirectionFlags::VERTICAL); }
    inline void open() { type = type | DirectionFlags::OPEN; }

    inline int8_t get_type() const { return static_cast<int8_t>(type); }

    inline bool compatible(DirectionFlags test) const {
        DirectionFlags upOver = is_up_over() ? DirectionFlags::UP_AND_OVER : DirectionFlags::NONE;
        DirectionFlags upDiagonal = is_up_diagonal() ? DirectionFlags::UP_AND_DIAGONAL : DirectionFlags::NONE;
        return !bool((type | upOver | upDiagonal) & ~test);// != DirectionFlags::NONE;
    };

    inline bool compatible(int8_t test) const {
        return compatible(static_cast<DirectionFlags>(test));
    }

    // if  1 and 0 then false
    //  1 horizontal
    //  2 diagonal
    //  4 up/down
    //  8 open
    //  16 up and over
    //  32 up and diagonal
    //

    bool operator==(const OnePath other) const {
        return other.type == this->type;
    }
};
#endif // __ONEPATH_HPP__
