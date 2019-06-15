#include "walls.h"
#include "glwidget.h"

Walls::Walls()
{
    m_name = "wall";

    // Ustawienie pozycji
    this->position.setX(10);
    this->position.setZ(-21);

    this->material.diffuse = QVector3D(1, 1, 1);
}

void Walls::render(GLWidget *glwidget)
{
    if(!stateWall)
        Cube::render(glwidget);
}

void Walls::init()
{
    Cube::init();
}

void Walls::update()
{

}
