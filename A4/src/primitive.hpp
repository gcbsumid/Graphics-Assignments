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

struct AABB {
  AABB() : mMin(1e8, 1e8, 1e8), mMax(-1e8, -1e8, -1e8){}

  Point3D mMin;
  Point3D mMax;


  AABB& operator =(const AABB& other)
  {
    mMin = other.mMin;
    mMax = other.mMax;
    return *this;
  }
};

class Primitive {
public:
  Primitive();
  virtual ~Primitive();

  virtual IntersectObj* intersect(Ray ray) = 0;
  virtual bool isInShadow(Ray ray) const = 0;
  virtual bool isNonHeirarchical() { return false;}

  AABB getBoundingBox();

  static double getDistance(const Point3D& p0, const Point3D& p1);

protected:
  AABB mBoundingBox;
};

class Sphere : public Primitive {
public:
  Sphere();
  virtual ~Sphere();

  virtual IntersectObj* intersect(Ray ray);
  virtual bool isInShadow(Ray ray) const;
};

class Cube : public Primitive {
public:
  Cube();
  virtual ~Cube();

  virtual IntersectObj* intersect(Ray ray);
  virtual bool isInShadow(Ray ray) const;
};

class NonhierSphere : public Primitive {
public:
  NonhierSphere(const Point3D& pos, double radius);
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
  NonhierBox(const Point3D& pos, double size);
  
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
