#ifndef CS488_MATERIAL_HPP
#define CS488_MATERIAL_HPP

#include "algebra.hpp"

class Material {
public:
  virtual ~Material();
  virtual void apply_gl() const = 0;
  virtual Colour get_diffuse() const = 0;
  virtual Colour get_specular() const = 0;
  virtual double get_shininess() const = 0;
  virtual double get_reflectiveness() const = 0;

protected:
  Material()
  {
  }
};

class PhongMaterial : public Material {
public:
  PhongMaterial(const Colour& kd, const Colour& ks, double shininess);
  virtual ~PhongMaterial();

  virtual void apply_gl() const;
  virtual Colour get_diffuse() const;
  virtual Colour get_specular() const;
  virtual double get_shininess() const;
  virtual double get_reflectiveness() const;

private:
  Colour m_kd;  // diffuse
  Colour m_ks;  // specular

  double m_shininess;
};

class ReflectivePhongMaterial : public PhongMaterial {
public:
  ReflectivePhongMaterial(const Colour& kd, const Colour& ks, double shininess, double reflectiveness);
  virtual ~ReflectivePhongMaterial();

  double get_reflectiveness() const;
private:
  double m_reflectiveness;
};


#endif
