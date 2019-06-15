#include "area.h"
#include <math.h>

Area::Area()
{
    m_name = "area";

    // olbiczenie promienia obiektów
    this->m_radius = 0.5 * sqrt(3 * this->scale.x() * this->scale.x());

    // Ustawienie pozycji Cube'a
    this->position.setX(-11);
    this->position.setY(-1);
    this->position.setZ(22);

    // Kolor Cube'a. Zwykły gradient.
    this->material.diffuse.setX(2.0f);
    this->material.diffuse.setY(1.3f);
    this->material.diffuse.setZ(3.1f);

    // Wielkość Cube'a.
    // Ustawienie w jednej linijce
    // zamiast osobno dla X, Y i Z.
    this->scale = QVector3D(25, 0.5f, 48);

    this->material.diffuse = QVector3D(0.1f, 0.1f, 0.1f);
}

void Area::init()
{
    m_mesh.generateCube(1.0f, 1.0f, 1.0f);
}

void Area::render(GLWidget* glwidget)
{
    m_mesh.render(glwidget);
}

void Area::update()
{

}
