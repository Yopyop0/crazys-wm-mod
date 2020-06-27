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
#include <iostream>
#include "interface/cFont.h"
#include <SDL_ttf.h>
#include "CLog.h"
#include "interface/CGraphics.h"
#include "sConfig.h"
#include <vector>
#include <numeric>
#include "interface/cColor.h"

cFont::cFont(CGraphics* gfx) : m_GFX(gfx)
{
}

void cFont::SetColor(unsigned char r, unsigned char g, unsigned char b)
{
    m_TextColor.r = r;
    m_TextColor.g = g;
    m_TextColor.b = b;
}

/*
 * old version for reference (and in case I balls it up
 */
bool cFont::LoadFont(const std::string& font, int size)
{
    cConfig cfg;
    m_Font = nullptr;
    g_LogFile.debug("interface", "loading font: '", font, "' at size ", size);

    m_Font.reset(TTF_OpenFont(font.c_str(), size));
    if (!m_Font)
    {
        g_LogFile.error("interface", "Error in LoadFont for font file: ", font);
        g_LogFile.error("interface", TTF_GetError());
        return false;
    }
    return true;
}

std::string cFont::UpdateLineEndings(std::string text)
{
#ifndef LINUX
    // for Windows, double "\n \n" newline characters were showing up as one newline and a boxy (bad) character...
    // so, here's a cheap-ass workaround to add a "\r" carriage return in front of each "\n" for Windows
    int pos = text.find("\n", 0);
    while (pos != std::string::npos)
    {
        text.insert(pos, "\r");
        pos = text.find("\n", pos + 2);
    }
#endif
    return std::move(text);
}

int cFont::GetFontHeight()
{
    return TTF_FontHeight(m_Font.get());
}

int cFont::GetFontLineSkip() const
{
    return TTF_FontLineSkip(m_Font.get());
}

void cFont::SetFontBold(bool Bold)
{
    TTF_SetFontStyle(m_Font.get(), Bold ? TTF_STYLE_BOLD : TTF_STYLE_NORMAL);
}

void cFont::GetSize(const std::string& text, int& width, int& height) const
{
    TTF_SizeText(m_Font.get(), text.c_str(), &width, &height);
}

cSurface cFont::RenderText(std::string text) const
{
    cConfig cfg;
    return m_GFX->GetImageCache().CreateTextSurface(m_Font.get(), std::move(text),
            sColor(m_TextColor.r, m_TextColor.g, m_TextColor.b), cfg.fonts.antialias());

}

cSurface cFont::RenderMultilineText(std::string text, int width) const
{
    text = UpdateLineEndings(std::move(text));

    // first separate into lines according to width
    std::vector<std::string> lines;
    std::string temp(text);    // current line of text
    temp += " ";    // makes sure that all the text will be displayed
    int n = 0;    // current index into the string
    int q = 0;    // the next \n int the string
    int p = 0;    // holds the previous index into the string
    int charwidth, charheight;
    width = width - 10;  // pad the sides a bit, it was otherwise slightly overflowing

    // -- Get until either ' ' or '\n'
    while (n != -1)
    {
        std::string strSub;
        n = temp.find(' ', p + 1);        // -- Find the next " "
        q = temp.find('\n', p + 1);        // -- Find the next "\n"
        if (q < n && q != -1)
        {
            strSub = temp.substr(0, q);
            GetSize(strSub, charwidth, charheight);
            if (charwidth >= width)
            {
                strSub = temp.substr(0, p);
                lines.push_back(strSub);    // -- Puts strSub into the lines vector
                temp = temp.substr(p + 1, std::string::npos);
                p = 0;
            }
            else
            {
                strSub = temp.substr(0, q);
                lines.push_back(strSub);
                temp = temp.substr(q + 1, std::string::npos);
                p = 0;
            }
        }
        else
        {
            strSub = temp.substr(0, n);
            GetSize(strSub, charwidth, charheight);
            if (charwidth >= width || n == -1)
            {
                strSub = temp.substr(0, p);
                lines.push_back(strSub);    // -- Puts strSub into the lines vector
                if (n != -1) temp = temp.substr(p + 1, std::string::npos);
                p = 0;
            }
            else
                p = n;
        }
    }

    int lineskip = GetFontLineSkip();
    int height = lines.size()*lineskip;

    // create a surface to render all the text too
    auto message = m_GFX->CreateSurface(width, height, sColor(0xff, 0, 0), true);

    cConfig cfg;
    for (unsigned int i = 0; i < lines.size(); i++)
    {
        if(!lines[i].empty()) {
            auto line = m_GFX->GetImageCache().CreateTextSurface(
                    m_Font.get(), lines[i], sColor(m_TextColor.r, m_TextColor.g, m_TextColor.b),
                    cfg.fonts.antialias());
            if(!line)
                continue;
            SDL_SetSurfaceBlendMode(line.RawSurface()->surface(), SDL_BLENDMODE_NONE);
            SDL_Rect dst = {0, static_cast<Sint16>(i * lineskip),
                            static_cast<Uint16>(line.GetWidth()), static_cast<Uint16>(line.GetHeight())};
            message = message.BlitOther(line, nullptr, &dst);
        }
    }
    return std::move(message);
}

cSurface cFont::RenderTable(const std::string& text, int max_width) const {
    // split into cells
    struct sCellData {
        int tabs;
        std::string content;
        cSurface render;
        bool is_last;
    };

    int tab = 0;
    int num_tabs = 0;
    std::vector<sCellData> cells;
    std::string next_cell;
    cConfig cfg;

    auto add_cell = [&, this](std::string text, bool last) {
        num_tabs = std::max(tab+1, num_tabs);
        if(text.empty()) {
            cells.push_back(sCellData{tab, std::move(text), cSurface(), last});
            return;
        }

        auto render = m_GFX->GetImageCache().CreateTextSurface(
                m_Font.get(), text, sColor(m_TextColor.r, m_TextColor.g, m_TextColor.b),
                cfg.fonts.antialias());
        SDL_SetSurfaceBlendMode(render.RawSurface()->surface(), SDL_BLENDMODE_NONE);

        cells.push_back(sCellData{tab, std::move(text), std::move(render), last});
    };

    // split text into cells
    for(auto& c : text) {
        if(c == '\t') {
            add_cell(std::move(next_cell), false);
            next_cell = "";
            ++tab;
        } else if(c == '\n') {
            add_cell(std::move(next_cell), true);
            next_cell = "";
            tab = 0;
        } else {
            next_cell.push_back(c);
        }
    }
    if(!next_cell.empty())
        add_cell(std::move(next_cell), true);

    // now determine tab positions
    std::vector<int> tab_widths(num_tabs, 0);
    for(auto& cell : cells) {
        if(!cell.render || cell.is_last)
            continue;
        tab_widths[cell.tabs] = std::max(tab_widths[cell.tabs], cell.render.GetWidth());
    }

    // determine the offsets
    std::vector<int> tab_start(num_tabs, 0);
    for(auto i = 0; i < num_tabs - 1; ++i) {
        tab_start[i+1] = tab_start[i] + tab_widths[i];
    }

    // at this point, we have the chance to split overlong lines
    int num_lines = 0;
    for(int i = 0; i < cells.size(); ++i) {
        if(!cells[i].render) continue;
        int x_pos = tab_start[cells[i].tabs] + cells[i].render.GetWidth();
        if(x_pos > max_width && cells[i].content.size() > 1) {
            // split
            int split_point = cells[i].content.size() / 2;
            while(split_point > 0 && cells[i].content[split_point] != ' ')
                --split_point;
            std::string first = cells[i].content.substr(0, split_point);
            std::string last = cells[i].content.substr(split_point);

            auto render = m_GFX->GetImageCache().CreateTextSurface(
                    m_Font.get(), first, sColor(m_TextColor.r, m_TextColor.g, m_TextColor.b),
                    cfg.fonts.antialias());
            SDL_SetSurfaceBlendMode(render.RawSurface()->surface(), SDL_BLENDMODE_NONE);

            cells[i] = sCellData{tab, std::move(first), std::move(render), true};

            render = m_GFX->GetImageCache().CreateTextSurface(
                    m_Font.get(), last, sColor(m_TextColor.r, m_TextColor.g, m_TextColor.b),
                    cfg.fonts.antialias());
            SDL_SetSurfaceBlendMode(render.RawSurface()->surface(), SDL_BLENDMODE_NONE);
            cells.insert(cells.begin() + i + 1, sCellData{0, std::move(last), std::move(render), true} );
        }

        if(cells[i].is_last) ++num_lines;
    }

    int lineskip = GetFontLineSkip();
    int height = (num_lines+1)*lineskip;

    // create a surface to render all the text too
    auto message = m_GFX->CreateSurface(max_width, height, sColor(0xff, 0, 0), true);

    int line = 0;
    for(int i = 0; i < cells.size(); ++i) {
        const auto& cell = cells[i];
        if(!cell.render) {
            if(cell.is_last)
                line += 1;
            continue;
        }

        int x = tab_start.at(cells[i].tabs);
        SDL_Rect dst = {static_cast<Sint16>(x),
                        static_cast<Sint16>(line * lineskip),
                        static_cast<Uint16>(cell.render.GetWidth()),
                        static_cast<Uint16>(cell.render.GetHeight())};
        message = message.BlitOther(cell.render, nullptr, &dst);

        if(cell.is_last)
            line += 1;
    }
    return message;
}


void FontDeleter::operator()(TTF_Font * font)
{
    if(font)
        TTF_CloseFont(font);
}