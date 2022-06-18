#ifdef BUILDING_EXAMPLE_DLL
#define DLL __declspec(dllexport)
#else
#define DLL __declspec(dllimport)
#endif

#include <list>
//#include "../src/world.hpp"

extern "C" DLL std::list<int> __cdecl GetVoxelTerrain() {
    // World world = World("../SavedTerrain/pathfinder_input.qb");
    // return world.terrain_main.ExportVoxelsAsList();
    return {1, 2, 3, 4};
}



/*class EXAMPLE_DLL ExportClass
{
public:
        ExportClass() {};
        virtual ~ExportClass() {};
        void func(void);

        //std::list<int> GetVoxelTerrain();
};
*/