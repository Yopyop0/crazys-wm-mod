/*
* Copyright 2009, 2010, The Pink Petal Development Team.
* The Pink Petal Devloment Team are defined as the game's coders
* who meet on http://pinkpetal.org
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

#ifndef WM_MANAGER_H
#define WM_MANAGER_H

#include "fwd.h"
#include "data.h"
#include <vector>

namespace tinyxml2 {
    class XMLElement;
}

class IBuilding;

class cMovieManager {
public:
    cMovieManager();
    void init();
    void load_xml(const tinyxml2::XMLElement& element);
    void save_xml(tinyxml2::XMLElement& element) const;
    int step(IBuilding& studio);

    // creating movies
    void add_scene(MovieScene scene);
    bool delete_scene(const MovieScene* scene);
    Movie& create_movie(const std::vector<const MovieScene*>& scene, std::string name);
    const std::vector<MovieScene>& get_scenes() const;

    std::string auto_create_name(const std::vector<const MovieScene*>& scene);

    // running movies
    const std::vector<Movie>& get_movies() const;
    void set_ticket_price(int index, int price);
    void hype_movie(int index, float hype, int cost);
    int hype_audience(int index, float hype);

    const std::vector<sTargetGroup>& get_audience() const;

    // info queries
    struct RatingResult {
        float Satisfaction;
        float Score;

        float QualityFactor;
        float PriceFactor;
        float HypeFactor;
        float NoTurnOff;
    };
    static RatingResult rate_movie_for_audience(const sTargetGroup& audience, const Movie& movie);
    int estimate_revenue(const Movie& movie);
    int auto_detect_ticket_price(const Movie& movie);
private:
    std::vector<sTargetGroup> m_Audiences;
    std::vector<sTargetGroupSpec> m_TargetGroupSpecs;
    std::vector<Movie> m_Movies;
    std::vector<MovieScene> m_Scenes;       // scenes that haven't been used in a movie yet

    void update_audience();

    // default naming list
    std::array<std::vector<std::string>, (int)SceneCategory::NUM_TYPES> m_DefaultNames;
};


#endif //WM_MANAGER_H