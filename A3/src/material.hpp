#ifndef CS488_MATERIAL_HPP
#define CS488_MATERIAL_HPP

#include "algebra.hpp"

class Material {
public:
  virtual ~Material();
  virtual void apply_gl(bool isSelected) const = 0;

protected:
  Material()
  {
  }
};

class PhongMaterial : public Material {
public:
  PhongMaterial(const Colour& kd, const Colour& ks, double shininess);
  virtual ~PhongMaterial();

  virtual void apply_gl(bool isSelected) const;

private:
  Colour m_kd;
  Colour m_ks;

  double m_shininess;

  static const Colour m_selected;
};


#endif
