/*
    block types:
    rock (five types ~)
    dirt (grass (4) and dirt (3) )
    mineral types
        cloth   W most water should go through
        thatch  W ~W v. w
        mud     W W v. w
        wood    M M v. w
        brick   S S v. w
        stone   S S v. w

        ladder open and can go up and down
        closed a placed object blocks this tile
        open   like a dore or open window
        air    open and no placed objects
*/
#include <stdint.h>

#ifndef __MATERIAL_HPP__
#define __MATERIAL_HPP__

struct Material{
    //std::map<const char *,  uint32_t> 
    std::vector<std::pair<const char *, uint32_t>> color;
    uint8_t speed_multiplier = 1;
    bool solid = false;
    uint8_t element_id = 0; // wood stone etc
    const char * element = "Air";
    //int8_t deteraition from wind
    //int8_t deteraition from water
};

#endif