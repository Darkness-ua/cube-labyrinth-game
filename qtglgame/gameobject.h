#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <QVector3D>
#include "texturemanager.h"

class GLWidget;

class GameObject {
public:
    GameObject()
    {

    }

    QVector3D energy = QVector3D(0.0f, 0.0f, 0.0f);
    QVector3D position = QVector3D(0.0f, 0.0f, 0.0f);
    QVector3D rotation = QVector3D(0.0f, 0.0f, 0.0f);
    QVector3D scale = QVector3D(1.0f, 1.0f, 1.0f);

    struct Material {
        QVector3D ambient  = QVector3D(1.0f, 1.0f, 1.0f);
        QVector3D diffuse  = QVector3D(1.0f, 1.0f, 1.0f);
        QVector3D specular = QVector3D(1.0f, 1.0f, 1.0f);
        float shininess = 1.0f;
    };
    Material material;

    float m_radius = 1.0f;
    std::string m_name;

    virtual void init() = 0;
    virtual void render(GLWidget* glwidget) = 0;
    virtual void update() = 0;

    bool isAlive = true;

    QOpenGLTexture* m_texture = nullptr;

    int H = 1, W = 1;
    bool stateWall = false, checkWall = false;

    int GetNeighbours(GameObject*** labyrinthWalls, int height, int width)
    {
        int countOfNeighbours = 0;

        if(height - 2 > 0)
            if(!labyrinthWalls[height - 2][width]->checkWall && labyrinthWalls[height - 2][width]->stateWall)
                countOfNeighbours++;

        if(width - 2 > 0)
            if(!labyrinthWalls[height][width - 2]->checkWall && labyrinthWalls[height][width - 2]->stateWall)
                countOfNeighbours++;

        if(width + 2 < 44)
            if(!labyrinthWalls[height][width + 2]->checkWall && labyrinthWalls[height][width + 2]->stateWall)
                countOfNeighbours++;

        if(height + 2 < 22)
            if(!labyrinthWalls[height + 2][width]->checkWall && labyrinthWalls[height + 2][width]->stateWall)
                countOfNeighbours++;

        return countOfNeighbours;
    }

    int MakeStep(GameObject*** labyrinthWalls, int &height, int &width, int step)
    {
        int countOfNeighbours = 0;

        if(height - 2 > 0)
            if(!labyrinthWalls[height - 2][width]->checkWall && labyrinthWalls[height - 2][width]->stateWall)
            {
                countOfNeighbours++;
                if(step == countOfNeighbours)
                {
                    height -= 2;
                    labyrinthWalls[height + 1][width]->stateWall = true;    //  убираем стену между предыдущей и текущей точкой
                    labyrinthWalls[height + 1][width]->checkWall = true; //  устанавливаем посещённое состояние убранной стене
                    labyrinthWalls[height][width]->checkWall = true; //  устанавливаем посещённое состояние текущей клетке
                }
            }

        if(width - 2 > 0)
            if(!labyrinthWalls[height][width - 2]->checkWall && labyrinthWalls[height][width - 2]->stateWall)
            {
                countOfNeighbours++;
                if(step == countOfNeighbours)
                {
                    width -= 2;
                    labyrinthWalls[height][width + 1]->stateWall = true;    //  убираем стену между предыдущей и текущей точкой
                    labyrinthWalls[height][width + 1]->checkWall = true; //  устанавливаем посещённое состояние убранной стене
                    labyrinthWalls[height][width]->checkWall = true; //  устанавливаем посещённое состояние текущей клетке
                }
            }

        if(width + 2 < 44)
            if(!labyrinthWalls[height][width + 2]->checkWall && labyrinthWalls[height][width + 2]->stateWall)
            {
                countOfNeighbours++;
                if(step == countOfNeighbours)
                {
                    width += 2;
                    labyrinthWalls[height][width - 1]->stateWall = true;    //  убираем стену между предыдущей и текущей точкой
                    labyrinthWalls[height][width - 1]->checkWall = true; //  устанавливаем посещённое состояние убранной стене
                    labyrinthWalls[height][width]->checkWall = true; //  устанавливаем посещённое состояние текущей клетке
                }
            }

        if(height + 2 < 22)
            if(!labyrinthWalls[height + 2][width]->checkWall && labyrinthWalls[height + 2][width]->stateWall)
            {
                countOfNeighbours++;
                if(step == countOfNeighbours)
                {
                    height += 2;
                    labyrinthWalls[height - 1][width]->stateWall = true;    //  убираем стену между предыдущей и текущей точкой
                    labyrinthWalls[height - 1][width]->checkWall = true; //  устанавливаем посещённое состояние убранной стене
                    labyrinthWalls[height][width]->checkWall = true; //  устанавливаем посещённое состояние текущей клетке
                }
            }

        return countOfNeighbours;
    }

    virtual ~GameObject() = default;
};

#endif // GAMEOBJECT_H
