#include "player.h"
#include "cube.h"
#include "glwidget.h"
#include <math.h>
#include <vector>
#include <iostream>

using namespace std;

Player::Player()
{
    m_name = "player";

    // olbiczenie promienia obiektów
    this->m_radius = 0.5 * sqrt(3 * this->scale.x() * this->scale.x());

    // Ustawienie pozycji głowy Player'a
    this->position.setX(pos_x);
    this->position.setY(pos_y);
    this->position.setZ(pos_z);

    // Kolor Player'a. Zwykły gradient.
    this->material.diffuse.setX(2.0f);
    this->material.diffuse.setY(1.3f);
    this->material.diffuse.setZ(3.1f);

    // Wielkość Player'a.
    // Ustawienie w jednej linijce
    // zamiast osobno dla X, Y i Z.
    this->scale = QVector3D(0.8f, 0.5f, 0.8f);

    this->material.diffuse = QVector3D(1.0f, 0.0f, 0.0f);
}

void Player::init()
{
    m_mesh.generateCube(1.0f, 1.0f, 1.0f);
}

void Player::render(GLWidget* glwidget)
{
    m_mesh.render(glwidget);
}

void Player::update()
{
    // przesuwanie trzeba zrobic tutaj
    this->position.setX(pos_x);
    this->position.setY(pos_y);
    this->position.setZ(pos_z);

    float temp = this->counter / 100.0f;
    this->material.diffuse = QVector3D(sin(temp), cos(temp), sin(1));
}

bool Player::checkPosition(GameObject ***labyrinthWalls, const char step)
{
    int temp_pos_x = this->pos_x * -1, temp_pos_z = this->pos_z;

    switch(step)
    {
        case 'W':
            if(temp_pos_x - 1 > 0 && labyrinthWalls[temp_pos_x - 1][temp_pos_z]->checkWall && labyrinthWalls[temp_pos_x - 1][temp_pos_z]->stateWall)
                return true;
            break;
        case 'S':
//            qDebug() << labyrinthWalls[temp_pos_x + 1][temp_pos_z]->checkWall << endl;
//            qDebug() << labyrinthWalls[temp_pos_x + 1][temp_pos_z]->stateWall << endl;
            if(temp_pos_x + 1 < 22 && labyrinthWalls[temp_pos_x + 1][temp_pos_z]->checkWall && labyrinthWalls[temp_pos_x + 1][temp_pos_z]->stateWall)
                return true;
            break;
        case 'A':
            if(temp_pos_z - 1 > 0 && labyrinthWalls[temp_pos_x][temp_pos_z - 1]->checkWall && labyrinthWalls[temp_pos_x][temp_pos_z - 1]->stateWall)
                return true;
            break;
        case 'D':
            if(temp_pos_z + 1 < 44 + 1 && labyrinthWalls[temp_pos_x][temp_pos_z + 1]->checkWall && labyrinthWalls[temp_pos_x][temp_pos_z + 1]->stateWall)
                return true;
            break;
    }
    return false;
}

void Player::setPosition(const char step)
{
    switch(step)
    {
        case 'W':
            this->pos_x++;
            break;
        case 'S':
            this->pos_x--;
            break;
        case 'A':
            this->pos_z--;
            break;
        case 'D':
            this->pos_z++;
            break;
    }
}
