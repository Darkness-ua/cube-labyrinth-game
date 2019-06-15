#include "gameobject.h"
#include "cmesh.h"

class Area : public GameObject
{
public:
    Area();

    void init();
    void render(GLWidget* glwidget);
    void update();

    CMesh m_mesh;
};
