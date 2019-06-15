#include "gameobject.h"
#include "cmesh.h"
#include "cube.h"
#include <vector>
#pragma once

class Player : public GameObject
{
public:
    Player();
    // przechowujemy tutaj kierunek snaka
    void init();
    void render(GLWidget* glwidget);
    void update();

    int pos_x = -1;
    int pos_y = 0;
    int pos_z = 1;

    unsigned int counter = 0;

    bool checkPosition(GameObject*** labyrinthWalls, const char step);
    void setPosition(const char step);

    CMesh m_mesh;
};
