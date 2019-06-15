#include "texturemanager.h"
#include <QImage>
#include <assert.h>

std::map<std::string, QOpenGLTexture*> TextureManager::m_textures;

TextureManager::TextureManager()
{

}

void TextureManager::init()
{
    m_textures["wall"] = new QOpenGLTexture(QImage("resources/wall.jpg"));
    m_textures["background"] = new QOpenGLTexture(QImage("resources/background.jpg"));
    m_textures["win"] = new QOpenGLTexture(QImage("resources/win.png"));
    m_textures["lose"] = new QOpenGLTexture(QImage("resources/lose.png"));
}

QOpenGLTexture* TextureManager::getTexture(std::string name)
{
    return m_textures[name];
}
