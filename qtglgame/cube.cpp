#include "cube.h"
#include "player.h"
#include <math.h>

Cube::Cube()
{
    m_name = "cube";

    // olbiczenie promienia obiektów
    this->m_radius = 0.5 * sqrt(3 * this->scale.x() * this->scale.x());

    // Ustawienie pozycji Cube'a
    this->position.setY(0);

    // Kolor Cube'a. Zwykły gradient.
    this->material.diffuse.setX(2.0f);
    this->material.diffuse.setY(1.3f);
    this->material.diffuse.setZ(3.1f);

    // Wielkość Cube'a.
    // Ustawienie w jednej linijce
    // zamiast osobno dla X, Y i Z.
    this->scale = QVector3D(1, 0.5f, 1);

    this->material.diffuse = QVector3D(1, 1, 1);
}

void Cube::init()
{
    m_mesh.generateCube(1.0f, 1.0f, 1.0f);
}

void Cube::render(GLWidget* glwidget)
{
    m_mesh.render(glwidget);
}

void Cube::update()
{

}
