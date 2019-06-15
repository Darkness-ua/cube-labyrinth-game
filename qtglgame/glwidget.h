#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QMatrix4x4>
#include <QKeyEvent>
#include <QMap>
#include "cmesh.h"
#include <player.h>
#include <QElapsedTimer>
#include <vector>
#include "walls.h"

QT_FORWARD_DECLARE_CLASS(QOpenGLShaderProgram)

class GLWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    GLWidget(QWidget *parent = nullptr);
    ~GLWidget();

    QSize sizeHint() const override;

    friend CMesh;

public slots:
    void cleanup();

signals:

protected:
    void initializeGL() override;
    void paintGL() override;
    void updateGL();
    void paintHUD();
    void resizeGL(int width, int height) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

    void setTransforms(void);

    void addObject(GameObject* obj);
//    void addObject(Walls *obj);
//    void checkCollision(Walls *obj);

private:

    struct LightLocStruct {
        int position;
        int ambient;
        int diffuse;
        int specular;
        int isActive;
        int attenuation;
    };

    struct RectangleLocStruct {
        int xPos;
        int yPos;
        int width;
        int height;
    };

    struct MaterialLocStruct {
        int ambient;
        int diffuse;
        int specular;
        int shininess;
    };

    struct Light {
        QVector3D position;
        QVector3D ambient;
        QVector3D diffuse;
        QVector3D specular;
        int isActive = 0;
        float attenuation;
    };

    static const int MAX_LIGHT = 5;
    Light m_lights[MAX_LIGHT];
    void setLights();

    QPoint m_lastPos;
    QOpenGLShaderProgram *m_program;
    int m_projMatrixLoc;
    int m_viewMatrixLoc;
    int m_modelMatrixLoc;
    int m_hasTextureLoc;
    LightLocStruct m_lightLoc[MAX_LIGHT];
    int m_cameraPositionLoc;
    MaterialLocStruct m_materialLoc;

    QOpenGLShaderProgram *m_program_hud;
    int m_resolutionLoc_hud;
    int m_color_hud;
    int m_hasTextureLoc_hud;

    RectangleLocStruct m_rectangleLoc_hud;
    void setRectangle(float xPos, float yPos, float width, float height, QVector3D color, QOpenGLTexture *texture);
    QVector2D m_resolution;

    QMatrix4x4 m_proj;
    QMatrix4x4 m_camera;
    QMatrix4x4 m_world;

    //QMap<QString, CMesh*> m_meshes;

    bool m_keyState[256];

    float m_camDistance = 1.5f;

    QElapsedTimer timer;
    float lastUpdateTime;
    float FPS;

    bool m_pressKeyState[256];

    Player* player = nullptr;
    GameObject*** labyrinthWalls = nullptr;

    std::vector<GameObject*> m_gameObjects;
};

#endif

