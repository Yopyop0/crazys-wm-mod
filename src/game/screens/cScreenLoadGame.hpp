/*
* Copyright 2009, 2010, The Pink Petal Development Team.
* The Pink Petal Devloment Team are defined as the game's coders
* who meet on http://pinkpetal.org     // old site: http://pinkpetal .co.cc
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef CRAZYS_WM_MOD_CSCREENLOADGAME_HPP
#define CRAZYS_WM_MOD_CSCREENLOADGAME_HPP

#include "interface/cInterfaceWindowXML.h"
#include "utils/DirPath.h"

class cScreenLoadGame : public cInterfaceWindowXML
{
public:
    cScreenLoadGame(const std::string& save_folder);
    void init(bool back) override;
    void process() override {};
    void set_ids() override;
private:
    // UI IDs
    int id_saveslist;
    void load_game();

    void OnKeyPress(SDL_Keysym keysym) override;

    DirPath m_SavesPath;
};


#endif //CRAZYS_WM_MOD_CSCREENLOADGAME_HPP
