//
// Created by apple on 2020/4/30.
//

#include "app/Scene.h"

Scene::Scene(int canvas_width, int canvas_height) : canvas(canvas_width, canvas_height) {}

void Scene::Open() {
    canvas.Show();
}

void Scene::Close() {
    canvas.ResetSelection();
    canvas.Hide();
}