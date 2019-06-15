HEADERS       = glwidget.h \
                window.h \
                mainwindow.h \
    cmesh.h \
    gameobject.h \
    cube.h \
    area.h \
    walls.h \
    player.h \
    texturemanager.h
SOURCES       = glwidget.cpp \
                main.cpp \
                window.cpp \
                mainwindow.cpp \
    cmesh.cpp \
    cube.cpp \
    area.cpp \
    walls.cpp \
    player.cpp \
    texturemanager.cpp

QT           += widgets

# install
target.path = ./qtglgame
INSTALLS += target

DESTDIR = builds
OBJECTS_DIR = builds/objects

DISTFILES += \
    builds/resources/shader.fs \
    builds/resources/shader.vs \
    builds/resources/shader_hud.fs \
    builds/resources/shader_hud.vs
