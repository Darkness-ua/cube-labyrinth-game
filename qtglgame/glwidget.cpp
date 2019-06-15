#include "glwidget.h"
#include "area.h"
#include "walls.h"
#include "texturemanager.h"
#include <QMouseEvent>
#include <QOpenGLShaderProgram>
#include <QCoreApplication>
#include <math.h>
#include <iostream>
#include <qstack.h>
#include <cstdlib>
#include <stack>
#include <vector>

using namespace std;

GLWidget::GLWidget(QWidget *parent)
    : QOpenGLWidget(parent),
      m_program(nullptr)
{
    setFocusPolicy(Qt::StrongFocus);
    setMouseTracking(true);

    QCursor c = cursor();
    c.setShape(Qt::CursorShape::BlankCursor);
    setCursor(c);
}

GLWidget::~GLWidget()
{
    //for(auto it = m_meshes.begin() ; it != m_meshes.end(); it++)
      //  delete it.value();

    cleanup();
}

QSize GLWidget::sizeHint() const
{
    return QSize(1000, 800);
}

void GLWidget::cleanup()
{
    if (m_program == nullptr)
        return;
    makeCurrent();

    delete m_program;
    m_program = nullptr;
    doneCurrent();
}

void GLWidget::initializeGL()
{
    srand(time(NULL));
    initializeOpenGLFunctions();
    glClearColor(0.29f, 0.32f, 0.29f, 1);
    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);

    for(int i = 0;i < 255;++i)
        m_keyState[i] = false;

    CMesh::loadAllMeshes();

    m_program = new QOpenGLShaderProgram;
    m_program->addShaderFromSourceFile(QOpenGLShader::Vertex, "resources/shader.vs");
    m_program->addShaderFromSourceFile(QOpenGLShader::Fragment, "resources/shader.fs");
    m_program->bindAttributeLocation("vertex", 0);
    m_program->bindAttributeLocation("normal", 1);
    m_program->link();

    m_program->bind();
    m_projMatrixLoc = m_program->uniformLocation("projMatrix");
    m_viewMatrixLoc = m_program->uniformLocation("viewMatrix");
    m_modelMatrixLoc = m_program->uniformLocation("modelMatrix");
    m_hasTextureLoc = m_program->uniformLocation("hasTexture");
    m_cameraPositionLoc = m_program->uniformLocation("cameraPosition");

    m_materialLoc.ambient = m_program->uniformLocation("material.ambient");
    m_materialLoc.diffuse = m_program->uniformLocation("material.diffuse");
    m_materialLoc.specular = m_program->uniformLocation("material.specular");
    m_materialLoc.shininess = m_program->uniformLocation("material.shininess");

    for(int i = 0; i < MAX_LIGHT; ++i)
    {
        m_lightLoc[i].position     = m_program->uniformLocation(QString().asprintf("light[%d].position", i));
        m_lightLoc[i].ambient      = m_program->uniformLocation(QString().asprintf("light[%d].ambient", i));
        m_lightLoc[i].diffuse      = m_program->uniformLocation(QString().asprintf("light[%d].diffuse", i));
        m_lightLoc[i].specular     = m_program->uniformLocation(QString().asprintf("light[%d].specular", i));
        m_lightLoc[i].isActive     = m_program->uniformLocation(QString().asprintf("light[%d].isActive", i));
        m_lightLoc[i].attenuation  = m_program->uniformLocation(QString().asprintf("light[%d].attenuation", i));
    }

    m_program->release();

    // HUD shaders
    m_program_hud = new QOpenGLShaderProgram;
    m_program_hud->addShaderFromSourceFile(QOpenGLShader::Vertex, "resources/shader_hud.vs");
    m_program_hud->addShaderFromSourceFile(QOpenGLShader::Fragment, "resources/shader_hud.fs");
    m_program_hud->bindAttributeLocation("vertex", 0);
    m_program_hud->bindAttributeLocation("normal", 1);
    m_program_hud->bindAttributeLocation("uvCoord", 2);
    m_program_hud->link();

    m_program_hud->bind();
    m_resolutionLoc_hud = m_program_hud->uniformLocation("resolution");
    m_color_hud = m_program_hud->uniformLocation("color");
    m_hasTextureLoc_hud = m_program_hud->uniformLocation("hasTexture");
    m_rectangleLoc_hud.xPos = m_program_hud->uniformLocation("rect.xPos");
    m_rectangleLoc_hud.yPos = m_program_hud->uniformLocation("rect.yPos");
    m_rectangleLoc_hud.width = m_program_hud->uniformLocation("rect.width");
    m_rectangleLoc_hud.height = m_program_hud->uniformLocation("rect.height");

    m_program_hud->release();

    TextureManager::init();

    lastUpdateTime = 0;
    timer.start();
    FPS = 60;

        // Dodanie obiektu do sceny.
        Area* area = new Area();
        area->m_texture = TextureManager::getTexture("background"); //  накладываем текстуру травы на стены
        addObject(area);

        player = new Player();
        addObject(player);

        //  динамически создаём планшу
        labyrinthWalls = new GameObject**[23];
        int temp_pos_x = 0, temp_pos_z = 0;

        for(int height = 0;height < 23;++height){
            temp_pos_z = 0;
            labyrinthWalls[height] = new GameObject*[45];

            for(int width = 0;width < 45;++width){
                labyrinthWalls[height][width] = new Walls();
                labyrinthWalls[height][width]->H = height;
                labyrinthWalls[height][width]->W = width;

                // устанавливаем бортики планши и даём им посещённый статус
                if(height == 0 || height == 22 || width == 0 || width == 44){
//                    labyrinthWalls[height][width]->material_color = QVector3D(1.0f, 1.0f, 1.0f);
                    labyrinthWalls[height][width]->checkWall = true;
                }

                labyrinthWalls[height][width]->position.setX(temp_pos_x);
                labyrinthWalls[height][width]->position.setZ(temp_pos_z);

                labyrinthWalls[height][width]->m_texture = TextureManager::getTexture("wall"); //  накладываем текстуру травы на стены
                addObject(labyrinthWalls[height][width]);   //  добавляем стены на планшу
                temp_pos_z++;
                }
                temp_pos_x--;
            }
                // устанавливаем сетку с дырками в лабиринте
                for(int height = 0;height < 23;++height)
                    for(int width = 0;width < 45;++width)
                        //здесь мы проверяем делать ли дырку в лаберинте
                        if((height % 2 != 0  && width % 2 != 0) && //если ячейка нечетная по x и y,
                         (height < 23-1 && width < 45-1)){   //и при этом находится в пределах стен лабиринта
                            labyrinthWalls[height][width]->stateWall = true;    // делаем дырку
                            labyrinthWalls[height][width - 1]->checkWall = true;    //  даём предыдущей стене посещённый статус
                        }
                        else
                            labyrinthWalls[height][width]->stateWall = false;   //  в противном случае это стена

                labyrinthWalls[1][1]->stateWall = true;
                labyrinthWalls[1][1]->checkWall = true;

                int unvisitedCountCells = 2, H = 1, W = 1;
                vector<GameObject*> visitedCells;
                visitedCells.push_back(labyrinthWalls[H][W]);

                while(!visitedCells.empty()){
                    unvisitedCountCells = labyrinthWalls[H][W]->GetNeighbours(labyrinthWalls, H, W);

                    if(unvisitedCountCells)
                    {
                        labyrinthWalls[H][W]->MakeStep(labyrinthWalls, H, W, 1 + rand() % unvisitedCountCells);
                        visitedCells.push_back(labyrinthWalls[H][W]);
                    }
                    else
                    {
                        visitedCells.pop_back();
                        if(!visitedCells.empty())
                        {
                            H = visitedCells[visitedCells.size() - 1]->H;
                            W = visitedCells[visitedCells.size() - 1]->W;
                        }
                    }
                }
                int exit = 1 + rand() % 21;

                while(!labyrinthWalls[exit][44]->stateWall)
                {
                exit = 1 + rand() % 21;
                    if(labyrinthWalls[exit][43]->stateWall)
                        labyrinthWalls[exit][44]->stateWall = true;
                }
        }

void GLWidget::setLights() {
    for(int i = 0; i < MAX_LIGHT; ++i) {
        m_program->setUniformValue(m_lightLoc[i].position,    m_lights[i].position   );
        m_program->setUniformValue(m_lightLoc[i].ambient,     m_lights[i].ambient    );
        m_program->setUniformValue(m_lightLoc[i].diffuse,     m_lights[i].diffuse    );
        m_program->setUniformValue(m_lightLoc[i].specular,    m_lights[i].specular   );
        m_program->setUniformValue(m_lightLoc[i].isActive,    m_lights[i].isActive   );
        m_program->setUniformValue(m_lightLoc[i].attenuation, m_lights[i].attenuation);
    }
}

void GLWidget::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    QStack<QMatrix4x4> worldMatrixStack;

    m_program->bind();

    m_program->setUniformValue(m_cameraPositionLoc, QVector3D(-11, 20, 22));  // combinowac

    // Light near camera
     m_lights[0].position    = player->position;
     m_lights[0].ambient     = player->material.diffuse;//QVector3D(0.1f, 0.1f, 0.1f);
     m_lights[0].diffuse     = QVector3D(0.9f, 0.9f, 0.9f);
     m_lights[0].specular    = QVector3D(0.1f, 0.1f, 0.1f);
     m_lights[0].isActive    = 1;
     m_lights[0].attenuation = 1.5f;

    setLights();

    m_world.setToIdentity();
    m_camera.setToIdentity();

    m_camera.lookAt(QVector3D(-11, 20, 22), QVector3D(-11, 8, 22), QVector3D(1, 0, 0)); // combinowac

    for(unsigned int i = 0 ; i < m_gameObjects.size() ; i++) {
        GameObject* obj = m_gameObjects[i];

        m_program->setUniformValue(m_materialLoc.ambient, obj->material.ambient);
        m_program->setUniformValue(m_materialLoc.diffuse, obj->material.diffuse);
        m_program->setUniformValue(m_materialLoc.specular, obj->material.specular);
        m_program->setUniformValue(m_materialLoc.shininess, obj->material.shininess);

        if(obj->m_texture != nullptr)
        {
            m_program->setUniformValue(m_hasTextureLoc, 1);
            obj->m_texture->bind();
        }
        else
        {
            m_program->setUniformValue(m_hasTextureLoc, 0);
        }

        worldMatrixStack.push(m_world);
            m_world.translate(obj->position);
            m_world.rotate(obj->rotation.x(), 1, 0, 0);
            m_world.rotate(obj->rotation.y(), 0, 1, 0);
            m_world.rotate(obj->rotation.z(), 0, 0, 1);
            m_world.scale(obj->scale);
            setTransforms();
            obj->render(this);
        m_world = worldMatrixStack.pop();
    }

    m_program->release();

    float timerTime = timer.elapsed() * 0.001f;
    float deltaTime = timerTime - lastUpdateTime;
    if(deltaTime >= (1.0f / FPS)) {
        updateGL();
    lastUpdateTime = timerTime;
    }
    paintHUD();

    update();
}

void GLWidget::setTransforms(void)
{
    m_program->setUniformValue(m_projMatrixLoc, m_proj);
    m_program->setUniformValue(m_viewMatrixLoc, m_camera);
    m_program->setUniformValue(m_modelMatrixLoc, m_world);
}

void GLWidget::addObject(GameObject *obj)
{
    obj->init();
    m_gameObjects.push_back(obj);
}

void GLWidget::setRectangle(float xPos, float yPos, float width, float height, QVector3D color, QOpenGLTexture *texture) {
    m_program_hud->setUniformValue(m_rectangleLoc_hud.xPos, xPos);
    m_program_hud->setUniformValue(m_rectangleLoc_hud.yPos, yPos);
    m_program_hud->setUniformValue(m_rectangleLoc_hud.width, width);
    m_program_hud->setUniformValue(m_rectangleLoc_hud.height, height);
    m_program_hud->setUniformValue(m_color_hud, color);

    if(texture != nullptr) {
        m_program_hud->setUniformValue(m_hasTextureLoc_hud, 1);
        texture->bind();
    } else {
        m_program_hud->setUniformValue(m_hasTextureLoc_hud, 0);
    }
}

//void GLWidget::addObject(Walls *obj)
//{
//    checkCollision(obj);
//    obj->init();
//    m_gameObjects.push_back(obj);
//}

//void GLWidget::checkCollision(Walls *obj)
//{
//    if(obj->position.x() == player->pos_x && obj->position.z() == player->pos_z)
//        obj->checkWall = true;
//}

void GLWidget::resizeGL(int w, int h)
{
    m_proj.setToIdentity();
    m_proj.perspective(60.0f, GLfloat(w) / h, 0.01f, 100.0f);

    m_resolution = QVector2D(w, h);
}

void GLWidget::mousePressEvent(QMouseEvent *event)
{
    m_lastPos = event->pos();
}

void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton) {
        //setXRotation(m_camXRot + 0.5f * dy);
        //setYRotation(m_camYRot + 0.5f * dx);
    } else if (event->buttons() & Qt::RightButton) {
        //setXRotation(m_camXRot + 0.5f * dy);
        //setZRotation(m_camZRot + 0.5f * dx);
    }
    m_lastPos = event->pos();
}

void GLWidget::keyPressEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Escape)
        exit(0);
    else if (e->key() == Qt::Key_Space)
    {
//        Bullet* bullet = new Bullet();
//        bullet->position = m_player.position + m_player.direction * 0.7f;
//        bullet->position.setY(0);
//        bullet->scale = QVector3D(0.5f, 0.5f, 0.5f);
//        bullet->m_radius = 0.5f;
//        bullet->energy = 3 * m_player.direction;
//        bullet->energy.setY(0);
//        addObject(bullet);
    }
    else
        QWidget::keyPressEvent(e);

    if(e->key() >= 0 && e->key() <= 255)
        m_keyState[e->key()] = true;
}

void GLWidget::keyReleaseEvent(QKeyEvent *e)
{
    if(e->key() >= 0 && e->key() <= 255)
    {
        m_keyState[e->key()] = false;
        m_pressKeyState[e->key()] = false;
    }
}

void GLWidget::updateGL()
{
    QCursor::setPos(mapToGlobal(QPoint(width()/2, height()/2)));

        if(m_keyState[Qt::Key_W] && !m_pressKeyState[Qt::Key_W] && player->checkPosition(labyrinthWalls, 'W'))
        {
            m_pressKeyState[Qt::Key_W] = true;
            player->setPosition('W');
        }

        if(m_keyState[Qt::Key_S] && !m_pressKeyState[Qt::Key_S] && player->checkPosition(labyrinthWalls, 'S'))
        {
            m_pressKeyState[Qt::Key_S] = true;
            player->setPosition('S');
        }

        if(m_keyState[Qt::Key_A] && !m_pressKeyState[Qt::Key_A] && player->checkPosition(labyrinthWalls, 'A'))
        {
            m_pressKeyState[Qt::Key_A] = true;
            player->setPosition('A');
        }

        if(m_keyState[Qt::Key_D] && !m_pressKeyState[Qt::Key_D] && player->checkPosition(labyrinthWalls, 'D'))
        {
            m_pressKeyState[Qt::Key_D] = true;
            player->setPosition('D');
        }

    player->update();

    for(unsigned int i = 0 ; i < m_gameObjects.size(); )
    {
        GameObject* obj = m_gameObjects[i];

        if(obj->isAlive == false)
        {
            m_gameObjects.erase(m_gameObjects.begin() + i);
            delete obj;
        }
        else
            i++;
    }
    player->counter++;
}

void GLWidget::paintHUD() {
    CMesh *rectMesh = CMesh::m_meshes["rect"];

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    m_program_hud->bind();
    m_program_hud->setUniformValue(m_resolutionLoc_hud, m_resolution);

    if(player->position.z() != 44 && player->counter <= 5400)
    {
        unsigned int playerCounterMax = 5400;
        float posX = 20;
        float posY = 8;
        float width = 250;
        float height = 10;
        float frame = 5;

        float bar = player->counter / float(playerCounterMax);
        bar = bar > 1 ? 1 : bar;

        setRectangle(posX, posY, width*bar, height, QVector3D(0, 1, 0), nullptr);
        rectMesh->render(this);

        setRectangle(posX+width*bar, posY, width*(1-bar), height, QVector3D(0, 0, 0), nullptr);
        rectMesh->render(this);

        setRectangle(posX-frame, posY-frame, width+frame*2, height+frame*2, QVector3D(0, 0.5f, 0), nullptr);
        rectMesh->render(this);
    }

    if(player->position.z() == 44)
    {
        setRectangle(0, 0, m_resolution.x(), m_resolution.y(), QVector3D(1, 1, 1), TextureManager::getTexture("win"));
        rectMesh->render(this);
    }

    if(player->counter >= 5400 && player->position.z() != 44)
    {
        setRectangle(0, 0, m_resolution.x(), m_resolution.y(), QVector3D(1, 1, 1), TextureManager::getTexture("lose"));
        rectMesh->render(this);
    }

    glDisable(GL_BLEND);
    m_program_hud->release();
}
