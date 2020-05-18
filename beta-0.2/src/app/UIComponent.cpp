//
// Created by apple on 2020/4/25.
//

#include "app/UIComponent.h"
#include "app/Canvas.h"

bool UIComponent::CompareX::operator()(const UIComponent::ptr& lhs, const UIComponent::ptr& rhs) const {
    if (lhs == nullptr) {
        return true;
    } else if (rhs == nullptr) {
        return false;
    } else if (lhs->x == rhs->x) {
        if (lhs->y == rhs->y) {
            return lhs < rhs;
        } else {
            return lhs->y < rhs->y;
        }
    } else {
        return lhs->x < rhs->x;
    }
}

bool UIComponent::CompareY::operator()(const UIComponent::ptr& lhs, const UIComponent::ptr& rhs) const {
    if (lhs == nullptr) {
        return true;
    } else if (rhs == nullptr) {
        return false;
    } else if (lhs->y == rhs->y) {
        if (lhs->x == rhs->x) {
            return lhs < rhs;
        } else {
            return lhs->x < rhs->x;
        }
    } else {
        return lhs->y < rhs->y;
    }
}

UIComponent::UIComponent(int x, int y, int height, int width, bool selectable, ControlType extra_control)
        : x(x), y(y), height(height), width(width), selectable(selectable), extra_control(extra_control)
        , active(true) {}

int UIComponent::GetX() const {
    return x;
}

int UIComponent::GetY() const {
    return y;
}

bool UIComponent::Active() const {
    return active;
}

void UIComponent::SetActive(bool is_active) {
    if (active == is_active) {
        return;
    }
    active = is_active;
}

UIComponent::ControlType UIComponent::GetControlType() const {
    return extra_control;
}

bool UIComponent::GetSelectable() const {
    return selectable;
}

void UIComponent::SetX(int new_x) {
    x = new_x;
}

void UIComponent::SetY(int new_y) {
    y = new_y;
}

void UIComponent::SetControlType(ControlType new_control) {
    extra_control = new_control;
}

void UIComponent::SetSelectable(bool new_val) {
    selectable = new_val;
}

void UIComponent::Clear() const {
    Canvas::Clear(x, y, width, height);
}

void UIComponent::Select() {}

void UIComponent::Unselect() {}

void UIComponent::Click() {}

bool UIComponent::Left() { return false; }

bool UIComponent::Right() { return false; }

bool UIComponent::Up() { return false; }

bool UIComponent::Down() { return false; }

void UIComponent::AltLeft() {}

void UIComponent::AltRight() {}
