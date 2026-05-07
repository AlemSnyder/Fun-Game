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
 * @file as_tests.hpp
 *
 * @author @AlemSnyder
 *
 * @brief Define AngelScript Tests
 *
 */

#pragma once

namespace as_test {

int test();

int as_loadtime_test();

int logging_test();

int as_threading();

int as_load_tests();

} // namespace as_test
