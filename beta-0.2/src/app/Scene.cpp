//
// Created by apple on 2020/4/30.
//

#include "app/Scene.h"

Scene::Scene(int canvas_width, int canvas_height) : canvas(canvas_width, canvas_height), active(false) {}

void Scene::Open() {
    if (!active) {
        canvas.Show();
        active = true;
    }
}

void Scene::Close() {
    if (active) {
        canvas.ResetSelection();
        canvas.Hide();
        active = false;
    }
}