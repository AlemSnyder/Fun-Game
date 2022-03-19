#include <iostream>
#include <vector>
#include "terrain.hpp"

int test7(){
    Terrain ter = Terrain(1000,1000,20);
    //ter.test();
    Tile * start = ter.get_tile(2,2,5);
    Tile * end = ter.get_tile(8,8,5);


    //std::cout << ter.can_stand(3,3,6, 3, 1) << ter.get_tile(3,3,6)->is_solid() << "can stand\n";
    //std::cout << ter.can_stand(3,3,5, 3, 1) << ter.get_tile(3,3,5)->is_solid() << "can stand\n";
    //std::cout << ter.can_stand(3,3,4, 3, 1) << ter.get_tile(3,3,4)->is_solid() << "can stand\n";

    std::vector<Tile*> path = ter.get_path_Astar(start, end);

    for (auto it = path.begin(); it != path.end(); ++it){
        std::cout << (*it)->get_x() << " " << (*it)->get_y() << " " << (*it)->get_z() << "\n";
    }
    return 0;

    /*OnePath path = OnePath(14);
    std::cout << path.compatible(15);

    path = OnePath(14);
    std::cout << path.compatible(63);

    path = OnePath(10);
    std::cout << path.compatible(15);

    path = OnePath(14);
    std::cout << path.compatible(1);

    path = OnePath(14);
    std::cout << path.compatible(8);

    return 0;*/
}
