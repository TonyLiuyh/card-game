//
// Created by apple on 2020/4/25.
//

#ifndef POKER_UICOMPONENT_H
#define POKER_UICOMPONENT_H

#include <memory>

class UIComponent {
public:
    enum ControlType {
        NONE,
        HORIZONTAL,
        VERTICAL
    };

    typedef std::shared_ptr<UIComponent> ptr;

    struct CompareX {
        bool operator() (const UIComponent::ptr& lhs, const UIComponent::ptr& rhs) const;
    };
    struct CompareY {
        bool operator() (const UIComponent::ptr& lhs, const UIComponent::ptr& rhs) const;
    };

    explicit UIComponent(int x, int y, int height, int width, bool selectable = false,
                         ControlType extra_control = NONE);

    virtual void Draw() = 0;

    int GetX() const;

    int GetY() const;

    bool Active() const;

    void SetActive(bool is_active);

    ControlType GetControlType() const;

    bool GetSelectable() const;

    void SetX(int new_x);

    void SetY(int new_y);

    void SetControlType(ControlType new_control);

    void SetSelectable(bool new_val);

    void Clear() const;

    virtual void Select();

    virtual void Unselect();

    virtual void Click();

    virtual bool Left();

    virtual bool Right();

    virtual bool Up();

    virtual bool Down();

    virtual void AltLeft();

    virtual void AltRight();

protected:
    int x;
    int y;
    int height;
    int width;
    bool selectable;
    ControlType extra_control;
    bool active;
};


#endif //POKER_UICOMPONENT_H
