#ifndef CS488_PRIMITIVE_HPP
#define CS488_PRIMITIVE_HPP

#include "algebra.hpp"

class SceneNode;

struct IntersectObj {
  IntersectObj() : mNode(NULL), mDistance(100000.0) {}
  SceneNode* mNode;
  Point3D mPoint;
  Vector3D mNormal;
  double mDistance;
};

class Primitive {
public:
  virtual ~Primitive();

  virtual IntersectObj* intersect(Ray ray) = 0;
  virtual bool isInShadow(Ray ray) const = 0;
  virtual bool isNonHeirarchical() { return false;}
};

class Sphere : public Primitive {
public:
  virtual ~Sphere();

  virtual IntersectObj* intersect(Ray ray);
  virtual bool isInShadow(Ray ray) const;
};

class Cube : public Primitive {
public:
  virtual ~Cube();

  virtual IntersectObj* intersect(Ray ray);
  virtual bool isInShadow(Ray ray) const;
};

class NonhierSphere : public Primitive {
public:
  NonhierSphere(const Point3D& pos, double radius)
    : mPos(pos), mRadius(radius)
  {
  }
  virtual ~NonhierSphere();
  virtual IntersectObj* intersect(Ray ray);
  virtual bool isInShadow(Ray ray) const;
  virtual bool isNonHeirarchical() { return true;}

private:
  Point3D mPos;
  double mRadius;
};

class NonhierBox : public Primitive {
public:
  NonhierBox(const Point3D& pos, double size)
    : mPos(pos), mSize(size)
  {
  }
  
  virtual ~NonhierBox();
  virtual IntersectObj* intersect(Ray ray);
  virtual bool isInShadow(Ray ray) const;
  virtual bool isNonHeirarchical() { return true;}

private:
  static bool PointOnBoxFace(Point3D pt, Point3D corner1, Point3D corner2);

  Point3D mPos;
  double mSize;
};

#endif
