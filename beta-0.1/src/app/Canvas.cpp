//
// Created by apple on 2020/4/24.
//

#include "app/Canvas.h"
#include "curses.h"

bool Canvas::initialized = false;

Canvas* Canvas::active_canvas = nullptr;

void Canvas::Clear(int x, int y, int width, int height) {
    for (int i = 0; i < height; ++i) {
        mvaddstr(x + i - height / 2, y - width / 2, std::string(width, ' ').c_str());
    }
}

Canvas::Canvas(int width, int height)
    : component_grid(height, std::vector<std::vector<UIComponent::ptr>>(width))
    , id(InputHandler::Instance()->NewIdentifier())
    , current_component(nullptr), width(width), height(height), is_active(false) {
    if (!initialized) {
        initialized = true;
        start_color();
        assume_default_colors(COLOR_BLACK, -1);
        init_pair(RED, COLOR_RED, -1);

        // register keys at InputHandler
        InputHandler::Instance()->RegisterKey(id, CH_UP,
                [] { if (active_canvas != nullptr) active_canvas->Up(); });
        InputHandler::Instance()->RegisterKey(id, 'w',
                [] { if (active_canvas != nullptr) active_canvas->Up(); });
        InputHandler::Instance()->RegisterKey(id, CH_DOWN,
                [] { if (active_canvas != nullptr) active_canvas->Down(); });
        InputHandler::Instance()->RegisterKey(id, 's',
                [] { if (active_canvas != nullptr) active_canvas->Down(); });
        InputHandler::Instance()->RegisterKey(id, CH_LEFT,
                [] { if (active_canvas != nullptr) active_canvas->Left(); });
        InputHandler::Instance()->RegisterKey(id, 'a',
                [] { if (active_canvas != nullptr) active_canvas->Left(); });
        InputHandler::Instance()->RegisterKey(id, CH_RIGHT,
                [] { if (active_canvas != nullptr) active_canvas->Right(); });
        InputHandler::Instance()->RegisterKey(id, 'd',
                [] { if (active_canvas != nullptr) active_canvas->Right(); });
        InputHandler::Instance()->RegisterKey(id, RETURN,
                [] { if (active_canvas != nullptr) active_canvas->Enter(); });
        InputHandler::Instance()->RegisterKey(id, InputHandler::ALT_LEFT_ARROW,
                [] { if (active_canvas != nullptr) active_canvas->AltLeft(); });
        InputHandler::Instance()->RegisterKey(id, InputHandler::ALT_RIGHT_ARROW,
                [] { if (active_canvas != nullptr) active_canvas->AltRight(); });
    }
}

void Canvas::AddComponent(const UIComponent::ptr& component, int grid_x,
        int grid_y, int grid_width, int grid_height) {
    components.emplace(component, std::vector<int>{grid_x, grid_y, grid_width, grid_height});
    // put the component pointer in a rectangle area defined by its x, y, width, height
    for (int row = grid_x; row < grid_x + grid_height; ++row) {
        for (int col = grid_y; col < grid_y + grid_width; ++col) {
            component_grid[row][col].push_back(component);
        }
    }
}

void Canvas::RemoveComponent(const UIComponent::ptr &component) {
    // make sure this component exists
    auto iter = components.find(component);
    if (iter != components.end()) {
        // read position properties of the component
        int grid_x = iter->second[0], grid_y = iter->second[1],
            grid_width = iter->second[2], grid_height = iter->second[3];

        // go through the rectangle area and delete this component in each grid
        for (int row = grid_x; row < grid_x + grid_height; ++row) {
            for (int col = grid_y; col < grid_y + grid_width; ++col) {
                auto& grid = component_grid[row][col];
                for (auto iter2 = grid.begin(); iter2 != grid.end(); ++iter2) {
                    if (*iter2 == component) {
                        grid.erase(iter2);
                        break;
                    }
                }
            }
        }

        // finally delete this component in components collection
        components.erase(iter);
    }
}

void Canvas::ResetSelection() {
    if (current_component != nullptr) {
        current_component->Unselect();
        current_component->Draw();
        current_component.reset();
    }
}

void Canvas::Show() {
    // hide the currently active canvas
    if (active_canvas != nullptr) {
        active_canvas->Hide();
    }
    active_canvas = this;
    is_active = true;
    // draw each active component
    for (const auto & iter : components) {
        if (iter.first->Active()) {
            iter.first->Draw();
        }
    }
    refresh();
}

void Canvas::Hide() {
    clear();
    active_canvas = nullptr;
    is_active = false;
    if (current_component != nullptr) {
        current_component->Unselect();
        current_component.reset();
    }
}

void Canvas::Up() {
    if (current_component == nullptr) {
        for (int i = height - 1; i >= 0; --i) {
            for (int j = width - 1; j >= 0; --j) {
                for (auto & k : component_grid[i][j]) {
                    if (k->Active() && k->GetSelectable()) {
                        current_component = k;
                        current_component->Select();
                        current_component->Draw();
                        return;
                    }
                }
            }
        }
    } else {
        if (current_component->GetControlType() == UIComponent::HORIZONTAL) {
            if (current_component->Up()) {
                current_component->Draw();
                return;
            }
        }
        current_component->Unselect();
        current_component->Draw();

        int grid_x = components[current_component][0], grid_y = components[current_component][1],
            grid_width = components[current_component][2];

        for (int row = grid_x - 1; row >= 0; --row) {
            for (int col = grid_y; col < grid_y + grid_width; ++col) {
                for (auto & k : component_grid[row][col]) {
                    if (k->Active() && k->GetSelectable()) {
                        current_component = k;
                        current_component->Select();
                        current_component->Draw();
                        return;
                    }
                }
            }
        }
    }
    current_component = nullptr;
}

void Canvas::Down() {
    if (current_component == nullptr) {
        for (int i = 0; i < height; ++i) {
            for (int j = 0; j < width; ++j) {
                for (auto & k : component_grid[i][j]) {
                    if (k->Active() && k->GetSelectable()) {
                        current_component = k;
                        current_component->Select();
                        current_component->Draw();
                        return;
                    }
                }
            }
        }
    } else {
        if (current_component->GetControlType() == UIComponent::HORIZONTAL) {
            if (current_component->Down()) {
                current_component->Draw();
                return;
            }
        }
        current_component->Unselect();
        current_component->Draw();

        int grid_x = components[current_component][0],
                grid_y = components[current_component][1],
                grid_height = components[current_component][3],
                grid_width = components[current_component][2];

        for (int row = grid_x + grid_height; row < height; ++row) {
            for (int col = grid_y; col < grid_y + grid_width; ++col) {
                for (auto & k : component_grid[row][col]) {
                    if (k->Active() && k->GetSelectable()) {
                        current_component = k;
                        current_component->Select();
                        current_component->Draw();
                        return;
                    }
                }
            }
        }
    }

    current_component = nullptr;
}

void Canvas::Left() {
    if (current_component == nullptr) {
        for (int j = width - 1; j >= 0; --j) {
            for (int i = height - 1; i >= 0; --i) {
                for (auto & k : component_grid[i][j]) {
                    if (k->Active() && k->GetSelectable()) {
                        current_component = k;
                        current_component->Select();
                        current_component->Draw();
                        return;
                    }
                }
            }
        }

    } else {
        if (current_component->GetControlType() == UIComponent::VERTICAL) {
            if (current_component->Left()) {
                current_component->Draw();
                return;
            }
        }
        current_component->Unselect();
        current_component->Draw();

        int grid_x = components[current_component][0],
                grid_y = components[current_component][1],
                grid_height = components[current_component][3],
                grid_width = components[current_component][2];

        for (int col = grid_y - 1; col >= 0; --col) {
            for (int row = grid_x; row < grid_x + grid_height; ++row) {
                for (auto & k : component_grid[row][col]) {
                    if (k->Active() && k->GetSelectable()) {
                        current_component = k;
                        current_component->Select();
                        current_component->Draw();
                        return;
                    }
                }
            }
        }
    }

    current_component = nullptr;
}

void Canvas::Right() {
    if (current_component == nullptr) {
        for (int j = 0; j < width; ++j) {
            for (int i = 0; i < height; ++i) {
                for (auto & k : component_grid[i][j]) {
                    if (k->Active() && k->GetSelectable()) {
                        current_component = k;
                        current_component->Select();
                        current_component->Draw();
                        return;
                    }
                }
            }
        }

    } else {
        if (current_component->GetControlType() == UIComponent::VERTICAL) {
            if (current_component->Right()) {
                current_component->Draw();
                return;
            }
        }
        current_component->Unselect();
        current_component->Draw();

        int grid_x = components[current_component][0],
                grid_y = components[current_component][1],
                grid_height = components[current_component][3],
                grid_width = components[current_component][2];

        for (int col = grid_y + grid_width; col < width; ++col) {
            for (int row = grid_x + grid_height - 1; row >= grid_x; --row) {
                for (auto & k : component_grid[row][col]) {
                    if (k->Active() && k->GetSelectable()) {
                        current_component = k;
                        current_component->Select();
                        current_component->Draw();
                        return;
                    }
                }
            }
        }
    }

    current_component = nullptr;
}

void Canvas::AltLeft() {
    if (current_component != nullptr) {
        current_component->AltLeft();
        if (is_active && current_component != nullptr) {
            current_component->Draw();
        }
    }
}

void Canvas::AltRight() {
    if (current_component != nullptr) {
        current_component->AltRight();
        if (is_active && current_component != nullptr) {
            current_component->Draw();
        }
    }
}

void Canvas::Enter() {
    if (current_component != nullptr) {
        current_component->Click();
        if (is_active && current_component != nullptr) {
            current_component->Draw();
        }
    }
}