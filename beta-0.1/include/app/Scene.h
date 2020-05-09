//
// Created by apple on 2020/4/30.
//

#ifndef POKER_SCENE_H
#define POKER_SCENE_H

#include <memory>
#include "app/Canvas.h"

class Scene {
public:
    explicit Scene(int canvas_width = 1, int canvas_height = 1);

    void Open();

    void Close();

protected:
    Canvas canvas;
};

#endif //POKER_SCENE_H
