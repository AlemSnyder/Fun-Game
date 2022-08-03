#ifndef __UNITPATH_HPP__
#define __UNITPATH_HPP__

#include <cstdint>
// TODO add comments
enum class DirectionFlags : uint8_t {
    NONE = 0,
    OPEN = 1 << 0,
    HORIZONTAL1 = 1 << 1,
    HORIZONTAL2 = 1 << 2,
    VERTICAL = 1 << 3,
    UP_AND_OVER = 1 << 4,
    UP_AND_DIAGONAL = 1 << 5,
};
inline DirectionFlags operator|(DirectionFlags lhs, DirectionFlags rhs) {
    return static_cast<DirectionFlags>(static_cast<uint8_t>(lhs) |
                                       static_cast<uint8_t>(rhs));
}
inline DirectionFlags operator&(DirectionFlags lhs, DirectionFlags rhs) {
    return static_cast<DirectionFlags>(static_cast<uint8_t>(lhs) &
                                       static_cast<uint8_t>(rhs));
}
inline DirectionFlags operator~(DirectionFlags val){
    return static_cast<DirectionFlags>(~static_cast<uint8_t>(val));
}

//DirectionFlags operator!=(DirectionFlags lhs, DirectionFlags rhs){
//    return (static_cast<uint8_t>(lhs)) == (static_cast<uint8_t>(rhs));
//}

class UnitPath {  // path type from adjacent nodes

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
    // there should be not UnitPath pointers
    UnitPath() : type(DirectionFlags::NONE) {}
    UnitPath(uint8_t type_) : type(static_cast<DirectionFlags>(type_)) {}
    UnitPath(DirectionFlags type_) : type(type_) {}

    inline bool is_open() const {
        return ((DirectionFlags::OPEN & type) == DirectionFlags::OPEN);
    }
    inline bool is_up_diagonal() const {
        return (
            (DirectionFlags::UP_AND_DIAGONAL & type)) != DirectionFlags::UP_AND_DIAGONAL;
    }
    inline bool is_up_over() const {
        return (
            (DirectionFlags::UP_AND_OVER & type)) != DirectionFlags::UP_AND_OVER;
    }
    inline bool is_diagonal() const {
        return (
            (DirectionFlags::HORIZONTAL2 & type)) != DirectionFlags::HORIZONTAL2;
    }
    inline bool is_up() const { return ((DirectionFlags::VERTICAL & type) == DirectionFlags::VERTICAL); }
    inline bool is_level() const { return ((DirectionFlags::VERTICAL & type) == DirectionFlags::NONE); }

    inline void close() { type = type & (DirectionFlags::HORIZONTAL1 | DirectionFlags::HORIZONTAL2 | DirectionFlags::VERTICAL); }
    inline void open() { type = type | DirectionFlags::OPEN; }

    inline uint8_t get_type() const { return static_cast<uint8_t>(type); }

    inline bool compatible(DirectionFlags test) const {
        return !bool(type & ~test);
    };

    inline bool compatible(uint8_t test) const {
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

    bool operator==(const UnitPath other) const {
        return other.type == this->type;
    }
    bool operator==(const uint8_t other) const {
        return static_cast<DirectionFlags>(other) == this->type;
    }
    bool operator==(const DirectionFlags other) const {
        return other == this->type;
    }
    UnitPath operator&(const UnitPath other) const {
        return type & other.type;
    }
};
#endif // __ONEPATH_HPP__
