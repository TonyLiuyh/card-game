//
// Created by apple on 2020/5/12.
//

#include "app/TextLabel.h"
#include "app/Canvas.h"
#include <boost/algorithm/string.hpp>
#include <utility>
#include <curses.h>

TextLabel::ptr TextLabel::Create(const std::string &text, int x, int y, int height, int width,
        HorizontalAlignment ha, VerticalAlignment va) {
    return TextLabel::ptr(new TextLabel(text, x, y, height, width, ha, va));
}

TextLabel::TextLabel(std::string text, int x, int y, int height, int width,
        HorizontalAlignment ha, VerticalAlignment va)
    : UIComponent(x, y, height, width)
    , text(std::move(text))
    , ha(ha)
    , va(va) {
}

void TextLabel::Draw() {
    Clear();
    if (text.empty()) {
        return;
    }
    //Canvas::Clear(x, y, width, height);
    std::vector<std::string> lines;
    boost::split(lines, text, boost::is_any_of("\n"));
    int start_x = x - height / 2;
    int number_lines = 0;
    std::vector<std::list<std::pair<std::string, int>>> color_lines;
    for (auto& line : lines) {
        if (line.empty()) {
            number_lines += 1;
            color_lines.emplace_back();
        } else {
            std::list<std::pair<std::string, int>> colors = ResolveColorString(line);
            int length = 0;
            for (const auto& item : colors) {
                length += item.first.length();
            }
            number_lines += (length - 1) / width + 1;
            color_lines.push_back(colors);
        }
    }

    if (number_lines < height) {
        if (va == VERTICAL_MID) {
            start_x += (height - number_lines) / 2;
        } else if (va == BOT) {
            start_x += height - number_lines;
        }
    }

    int line_index = 0;
    int start_y = y - width / 2;
    for (int i = 0; i < number_lines && i < height; ++i) {
        int length = 0;
        for (const auto& item : color_lines[line_index]) {
            length += item.first.length();
        }
        if (length > width) {
            move(start_x + i, start_y);
            length = 0;
            auto iter = color_lines[line_index].begin();
            while (length + iter->first.length() <= width) {
                length += iter->first.length();
                if (iter->second > 0) {
                    attron(COLOR_PAIR(iter->second));
                    addstr(iter->first.c_str());
                    attroff(COLOR_PAIR(iter->second));
                } else {
                    addstr(iter->first.c_str());
                }
                iter = color_lines[line_index].erase(iter);
            }
            if (length < width) {
                if (iter->second > 0) {
                    attron(COLOR_PAIR(iter->second));
                    addstr(iter->first.substr(0, width - length).c_str());
                    attroff(COLOR_PAIR(iter->second));
                } else {
                    addstr(iter->first.substr(0, width - length).c_str());
                }
                iter->first = iter->first.substr(width - length);
            }
        } else {
            if (ha == HORIZONTAL_MID) {
                start_y += (width - length) / 2;
            } else if (ha == RIGHT) {
                start_y += width - length;
            }
            move(start_x + i, start_y);
            for (const auto& item : color_lines[line_index]) {
                if (item.second > 0) {
                    attron(COLOR_PAIR(item.second));
                    addstr(item.first.c_str());
                    attroff(COLOR_PAIR(item.second));
                } else {
                    addstr(item.first.c_str());
                }
            }
            ++line_index;
        }
    }
}

void TextLabel::SetText(const std::string &new_text) {
    text = new_text;
}

void TextLabel::SetText(const std::vector<std::string> &texts) {
    if (texts.empty()) {
        text = "";
    }
    text = texts[0];
    for (int i = 1; i < texts.size(); ++i) {
        text += "\n" + texts[i];
    }
}

void TextLabel::SetHorizontalAlignment(HorizontalAlignment new_ha) {
    ha = new_ha;
}

void TextLabel::SetVerticalAlignment(VerticalAlignment new_va) {
    va = new_va;
}

std::list<std::pair<std::string, int>> TextLabel::ResolveColorString(const std::string& str) {
    if (str.empty()) {
        return std::list<std::pair<std::string, int>>();
    }
    std::vector<std::string> tokens;
    std::list<std::pair<std::string, int>> result;
    boost::split(tokens, str, boost::is_any_of("\b"));
    if (!tokens[0].empty()) {
        result.emplace_back(tokens[0], 0);
    }
    for (int i = 1; i < tokens.size(); ++i) {
        if (tokens[i].size() > 1) {
            if (tokens[i][0] == 'R') {
                result.emplace_back(tokens[i].substr(1), Canvas::RED);
            } else if (tokens[i][0] == 'G') {
                result.emplace_back(tokens[i].substr(1), Canvas::GREEN);
            } else if (tokens[i][0] == 'L') {
                result.emplace_back(tokens[i].substr(1), Canvas::BLUE);
            } else if (tokens[i][0] == 'Y') {
                result.emplace_back(tokens[i].substr(1), Canvas::YELLOW);
            } else if (tokens[i][0] == 'E') {
                result.emplace_back(tokens[i].substr(1), Canvas::GREY);
            } else {
                result.emplace_back(tokens[i].substr(1), 0);
            }
        }
    }
    return result;
}