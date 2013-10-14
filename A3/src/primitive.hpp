#ifndef CS488_PRIMITIVE_HPP
#define CS488_PRIMITIVE_HPP

#include "algebra.hpp"

class Primitive {
public:
  virtual ~Primitive();
  virtual void walk_gl(bool picking) = 0; //const = 0;
};

class Sphere : public Primitive {
public:
  Sphere();
  virtual ~Sphere();
  virtual void walk_gl(bool picking); // const;

  double tempAngle;
};

#endif
