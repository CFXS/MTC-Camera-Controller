// ---------------------------------------------------------------------
// CFXS MTC Camera Controller <https://github.com/CFXS/MTC-Camera-Controller>
// Copyright (C) 2021 | CFXS
// 
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>
// ---------------------------------------------------------------------
// [CFXS] //
#pragma once

#include <filesystem>

#define CFXS_ASSERT(x, y)                                                                                            \
    {                                                                                                                \
        if (!(x)) {                                                                                                  \
            printf("%s:%d  ASSERT FAILED: " #y "\n", std::filesystem::path(__FILE__).filename().string(), __LINE__); \
            CFXS_DEBUGBREAK();                                                                                       \
        }                                                                                                            \
    }
