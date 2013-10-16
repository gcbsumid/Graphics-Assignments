#ifndef CS488_PRIMITIVE_HPP
#define CS488_PRIMITIVE_HPP

#include "algebra.hpp"
#include <GL/gl.h>

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

  static void initDisplayList();
  static bool mHasInitializedDisplayList;

  static GLuint mSphereDL;
  double tempAngle;
};

#endif
