#ifndef WALLS_H
#define WALLS_H

#include "cube.h"


class Walls : public Cube
{
public:
    Walls();

    void render(GLWidget* glwidget);
    void init();
    void update();
};

#endif // WALLS_H
