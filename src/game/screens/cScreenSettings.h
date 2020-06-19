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
#pragma once

#include "interface/cInterfaceWindowXML.h"

class cScreenSettings : public cInterfaceWindowXML
{
public:
    void set_ids() override;

    cScreenSettings();
    void init(bool back) override;
    void process() override { };

    void update_settings();

    void update_girl_sliders();
    void update_gang_sliders();
private:
    int back_id = -1;
    int ok_id = -1;
    int revert_id = -1;

    // Folders
    int characters_id = -1;
    int saves_id = -1;
    int defaultimages_id = -1;
    int items_id = -1;
    int backupsaves_id = -1;
    int preferdefault_id = -1;

    // Catacombs
    int uniquecatacombs_id = -1;
    int uniquecatacombslabel_id = -1;
    int girlgetsgirls_id = -1;
    int girlgetsitems_id = -1;
    int girlgetsbeast_id = -1;
    int girlsgirlspercslider_id = -1;
    int girlsitemspercslider_id = -1;
    int ganggetsgirls_id = -1;
    int ganggetsitems_id = -1;
    int ganggetsbeast_id = -1;
    int gangsgirlspercslider_id = -1;
    int gangsitemspercslider_id = -1;
};