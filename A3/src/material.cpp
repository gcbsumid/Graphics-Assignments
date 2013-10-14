#include "material.hpp"
#include <GL/gl.h>
#include <GL/glu.h>

Material::~Material()
{
}

PhongMaterial::PhongMaterial(const Colour& kd, const Colour& ks, double shininess)
  : m_kd(kd), m_ks(ks), m_shininess(shininess)
{
    std::cout <<"Material created!" << std::endl;
}

PhongMaterial::~PhongMaterial()
{
}

void PhongMaterial::apply_gl() const
{
    // Perform OpenGL calls necessary to set up this material.
    // std::cout << "I'm in the material." << std::endl;
    // glShadeModel(GL_SMOOTH);

    GLfloat mat_diffuse[] = { (float)m_kd.R(), (float)m_kd.G(), (float)m_kd.B() };
    GLfloat mat_specular[] = { (float)m_ks.R(), (float)m_ks.G(), (float)m_ks.B() }; 
    GLfloat mat_shininess[] = { (float)m_shininess };  

    // std::cout << "Diffuse: (" << mat_diffuse[0] << ", " << mat_diffuse[1] << ", " << mat_diffuse[2] << ")" << std::endl;
    // std::cout << "Specular: (" << mat_specular[0] << ", " << mat_specular[1] << ", " << mat_specular[2] << ")" << std::endl;
    // std::cout << "Shininess: (" << mat_shininess[0] << ")" << std::endl;


    glEnable(GL_COLOR_MATERIAL);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glColorMaterial(GL_FRONT, GL_DIFFUSE);
    glColor3f(m_kd.R(), m_kd.G(), m_kd.B());
}
