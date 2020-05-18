//
// Created by apple on 2020/4/24.
//

#ifndef POKER_CANVAS_H
#define POKER_CANVAS_H

#include <map>
#include <unordered_set>
#include <vector>
#include "app/UIComponent.h"
#include "app/InputHandler.h"

// represent a canvas where UI components are drawn
// it uses a grid-like layout to control how components are selected
class Canvas {
public:
    static const int RED;
    static const int GREEN;
    static const int BLUE;
    static const int YELLOW;
    static const int GREY;

    // clear a rectangle of the given size centered at (x, y)
    static void Clear(int x, int y, int width, int height);

    // create a canvas with grid of the given certain size
    Canvas(int width, int height);

    // let this canvas become the active canvas
    // and draw all active components on the canvas
    void Show();

    // clear the screen and inactivate this canvas
    void Hide();

    // add a component to the canvas
    void AddComponent(const UIComponent::ptr& component, int grid_x, int grid_y,
            int grid_width = 1, int grid_height = 1);

    void AddComponent(const UIComponent::ptr& component);

    // remove a component from the canvas
    void RemoveComponent(const UIComponent::ptr& component);

    void ResetSelection();

private:
    void Left();

    void Right();

    void Up();

    void Down();

    void AltLeft();

    void AltRight();

    void Enter();

private:
    static Canvas* active_canvas;

    static bool initialized;

    // x by y grid, one grid represented by a vector, indicating the order of components
    std::vector<std::vector<std::vector<UIComponent::ptr>>> component_grid;

    // a collection of components and vectors of their grid_x, grid_y, grid_width, grid_height
    std::unordered_map<UIComponent::ptr, std::vector<int>> components;

    std::unordered_set<UIComponent::ptr> components_not_in_grid;

    UIComponent::ptr current_component;

    InputHandler::Identifier id;

    int width;

    int height;

    bool is_active;
};

#endif //POKER_CANVAS_H
