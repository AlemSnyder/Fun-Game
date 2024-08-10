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
 * @file loading.hpp
 *
 * @brief Does nothing.
 *
 * @ingroup Util
 *
 */


// Everything that needs to be loaded from files

namespace util {

// load manifest to object handler
// This loads objects from qb files onto the gpu
void load_manifest();

} // namespace util
