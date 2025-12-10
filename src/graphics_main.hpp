// -*- lsst-c++ -*-
/*
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software
 *
 * Foundation, version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 */

/**
 * @file graphics_main.hpp
 *
 * @brief Defines GUI startup functions
 *
 * @ingroup --
 */

#include <argh.h>

#include <variant>

/**
 * @brief Start Graphics window
 *
 * @param const argh::parser& cmdl command line arguments
 */
int graphics_main(const argh::parser& cmdl);

/**
 * @brief Start Graphics window
 *
 * @warning NOT IMPLEMENTED
 */
int graphics_main();

namespace intro_scene {

enum return_to {
    EXIT,
    INTRO_SCENE,
    NEW_GAME,
    LOAD_GAME,
};

struct Exit {
    int status = 0;
};

struct IntroPage {};

struct NewGame {
    std::string biome;
    int seed;
    // map location;
    // starting something
    // difficulty etc
    // int difficulty;
};

struct LoadGame {
    std::filesystem::path game_file_path;
};

using result = std::variant<Exit, IntroPage, NewGame, LoadGame>;

} // namespace intro_scene

intro_scene::result intro_window();

intro_scene::result graphics_main(intro_scene::NewGame);

intro_scene::result graphics_main(intro_scene::LoadGame);
