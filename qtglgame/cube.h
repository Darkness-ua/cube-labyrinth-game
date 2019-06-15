#include "gameobject.h"
#include "cmesh.h"
#pragma once

class Cube : public GameObject
{
public:
    Cube();

    void init();
    void render(GLWidget* glwidget);
    void update();

    CMesh m_mesh;
};
