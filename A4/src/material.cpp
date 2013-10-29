#include "material.hpp"

Material::~Material()
{
}

PhongMaterial::PhongMaterial(const Colour& kd, const Colour& ks, double shininess)
  : m_kd(kd), m_ks(ks), m_shininess(shininess)
{
}

PhongMaterial::~PhongMaterial()
{
}

void PhongMaterial::apply_gl() const
{
  // Perform OpenGL calls necessary to set up this material.
}

Colour PhongMaterial::get_diffuse() const {
  return m_kd;
}

Colour PhongMaterial::get_specular() const {
  return m_ks;
}

double PhongMaterial::get_shininess() const {
  return m_shininess;
}

double PhongMaterial::get_reflectiveness() const {
  return 0.0;
}

ReflectivePhongMaterial::ReflectivePhongMaterial(const Colour& kd, const Colour& ks, double shininess, double reflectiveness)
  : PhongMaterial(kd, ks, shininess), m_reflectiveness(reflectiveness)
{
}

ReflectivePhongMaterial::~ReflectivePhongMaterial()
{
}

double ReflectivePhongMaterial::get_reflectiveness() const {
  return m_reflectiveness;
}
