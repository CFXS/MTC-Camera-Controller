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

#include <Application.hpp>

namespace TCC {

    class TCC_Application : public Application {
    public:
        TCC_Application(int argc, char** argv);
        virtual ~TCC_Application() = default;

        virtual void OnCreate() override;
        virtual void OnDestroy() override;
    };

} // namespace TCC