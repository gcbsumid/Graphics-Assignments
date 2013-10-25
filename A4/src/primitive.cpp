#include "primitive.hpp"
#include "polyroots.hpp"
#include <cmath>

const double SMALL_NUM = 0.00000001;

Primitive::~Primitive()
{
}

Sphere::~Sphere()
{
}

IntersectObj* Sphere::intersect(Ray ray) {
  // TODO
  return NULL;
}

bool Sphere::isInShadow(Ray ray) const {
  // TODO
  return false;
}

Cube::~Cube()
{
}

IntersectObj* Cube::intersect(Ray ray) {
  // TODO
  return NULL;  
}

bool Cube::isInShadow(Ray ray) const {
  // TODO
  return false;
}

NonhierSphere::~NonhierSphere()
{
}

IntersectObj* NonhierSphere::intersect(Ray ray) {

  double A = ray.mDirection.dot(ray.mDirection);
  Vector3D centerToEye = ray.mOrigin - mPos;
  centerToEye.normalize();
  double B = 2 * ray.mDirection.dot(centerToEye);
  double C = centerToEye.dot(centerToEye) - (mRadius * mRadius);
  double roots[2];

  // Calate for real roots
  size_t numOfRealRoots = quadraticRoots(A, B, C, roots);
  
  if (numOfRealRoots == 0) {
    // There are no intersections with this object
    return NULL;
  }

  IntersectObj* obj = new IntersectObj();
  for (unsigned int i = 0; i < numOfRealRoots; ++i) {
    if (roots[i] < 0) {
      continue;
    }
    if (roots[i] < obj->mDistance) {
      obj->mDistance = roots[i];
    }
  }

  // Get the point of intersection
  obj->mPoint = ray.mOrigin + (obj->mDistance * ray.mDirection);
  obj->mNormal = obj->mPoint - mPos;
  obj->mNormal.normalize();

  return obj;
}

bool NonhierSphere::isInShadow(Ray ray) const {
  Vector3D normalizedRay = ray.mDirection;
  normalizedRay.normalize();
  double A = normalizedRay.dot(normalizedRay);
  Vector3D centerToEye = ray.mOrigin - mPos;
  centerToEye.normalize();
  double B = 2 * normalizedRay.dot(centerToEye);
  double C = centerToEye.dot(centerToEye) - (mRadius * mRadius);
  double roots[2];

  // Calate for real roots
  size_t numOfRealRoots = quadraticRoots(A, B, C, roots);
  
  if (numOfRealRoots == 0) {
    // There are no intersections with this object
    return false;
  } 

  double dist = ray.mDirection.length();
  for (unsigned int i = 0; i < numOfRealRoots; ++i) {
    if (roots[i] < 0) {
      continue;
    }
    if (roots[i] < dist) {
      return true;
    }
  }

  return false;
}


NonhierBox::~NonhierBox()
{
}

IntersectObj* NonhierBox::intersect(Ray ray) {
  Plane planes[6]; 
  Point3D corner1[6];
  Point3D corner2[6];
  planes[0].mPoint = mPos + Vector3D(mSize/2.0, 0.0, 0.0);
  corner1[0] = planes[0].mPoint + Vector3D(0.0, mSize/2.0, -mSize/2.0);
  corner2[0] = planes[0].mPoint + Vector3D(0.0, -mSize/2.0, mSize/2.0);

  planes[1].mPoint = mPos + Vector3D(-mSize/2.0, 0.0, 0.0);
  corner1[1] = planes[1].mPoint + Vector3D(0.0, mSize/2.0, -mSize/2.0);
  corner2[1] = planes[1].mPoint + Vector3D(0.0, -mSize/2.0, mSize/2.0);

  planes[2].mPoint = mPos + Vector3D(0.0, mSize/2.0, 0.0);
  corner1[2] = planes[2].mPoint + Vector3D(mSize/2.0, 0.0, -mSize/2.0);
  corner2[2] = planes[2].mPoint + Vector3D(-mSize/2.0, 0.0, mSize/2.0);

  planes[3].mPoint = mPos + Vector3D(0.0, -mSize/2.0, 0.0);
  corner1[3] = planes[3].mPoint + Vector3D(mSize/2.0, 0.0, -mSize/2.0);
  corner2[3] = planes[3].mPoint + Vector3D(-mSize/2.0, 0.0, mSize/2.0);

  planes[4].mPoint = mPos + Vector3D(0.0, 0.0, mSize/2.0);
  corner1[4] = planes[4].mPoint + Vector3D(mSize/2.0, -mSize/2.0, 0.0);
  corner2[4] = planes[4].mPoint + Vector3D(-mSize/2.0, mSize/2.0, 0.0);

  planes[5].mPoint = mPos + Vector3D(0.0, 0.0, -mSize/2.0);
  corner1[5] = planes[5].mPoint + Vector3D(mSize/2.0, -mSize/2.0, 0.0);
  corner2[5] = planes[5].mPoint + Vector3D(-mSize/2.0, mSize/2.0, 0.0);

  for (int i = 0; i < 6; ++i) {
    planes[i].mNormal = planes[i].mPoint - mPos;
    planes[i].mNormal.normalize();
  }

  IntersectObj* obj = new IntersectObj();
  for (int i = 0; i < 6; ++i) {
    double denom = planes[i].mNormal.dot(ray.mDirection);

    if (denom < SMALL_NUM) {
      continue;
    }

    Vector3D rayOrigToPlanePt = planes[i].mPoint - ray.mOrigin;
    rayOrigToPlanePt.normalize();

    double t = rayOrigToPlanePt.dot(planes[i].mNormal) / denom;
    if (t < 0.0) {
      continue;
    }

    Point3D intersectionPoint = ray.mOrigin + (t * ray.mDirection);

    if (PointOnBoxFace(intersectionPoint, corner1[i], corner2[i])) {
      if (t < obj->mDistance) {
        obj->mDistance = t;
        obj->mPoint = intersectionPoint;
        obj->mNormal = planes[i].mNormal;
      }
    }
  }

  if (obj->mDistance >= 100000.0) {
    delete obj;
    return NULL;
  }

  return obj;
}

bool NonhierBox::isInShadow(Ray ray) const {
  // TODO
  Plane planes[6]; 
  Point3D corner1[6];
  Point3D corner2[6];
  planes[0].mPoint = mPos + Vector3D(mSize/2.0, 0.0, 0.0);
  corner1[0] = planes[0].mPoint + Vector3D(0.0, mSize/2.0, -mSize/2.0);
  corner2[0] = planes[0].mPoint + Vector3D(0.0, -mSize/2.0, mSize/2.0);

  planes[1].mPoint = mPos + Vector3D(-mSize/2.0, 0.0, 0.0);
  corner1[1] = planes[1].mPoint + Vector3D(0.0, mSize/2.0, -mSize/2.0);
  corner2[1] = planes[1].mPoint + Vector3D(0.0, -mSize/2.0, mSize/2.0);

  planes[2].mPoint = mPos + Vector3D(0.0, mSize/2.0, 0.0);
  corner1[2] = planes[2].mPoint + Vector3D(mSize/2.0, 0.0, -mSize/2.0);
  corner2[2] = planes[2].mPoint + Vector3D(-mSize/2.0, 0.0, mSize/2.0);

  planes[3].mPoint = mPos + Vector3D(0.0, -mSize/2.0, 0.0);
  corner1[3] = planes[3].mPoint + Vector3D(mSize/2.0, 0.0, -mSize/2.0);
  corner2[3] = planes[3].mPoint + Vector3D(-mSize/2.0, 0.0, mSize/2.0);

  planes[4].mPoint = mPos + Vector3D(0.0, 0.0, mSize/2.0);
  corner1[4] = planes[4].mPoint + Vector3D(mSize/2.0, -mSize/2.0, 0.0);
  corner2[4] = planes[4].mPoint + Vector3D(-mSize/2.0, mSize/2.0, 0.0);

  planes[5].mPoint = mPos + Vector3D(0.0, 0.0, -mSize/2.0);
  corner1[5] = planes[5].mPoint + Vector3D(mSize/2.0, -mSize/2.0, 0.0);
  corner2[5] = planes[5].mPoint + Vector3D(-mSize/2.0, mSize/2.0, 0.0);

  for (int i = 0; i < 6; ++i) {
    planes[i].mNormal = planes[i].mPoint - mPos;
    planes[i].mNormal.normalize();
  }

  double dist = ray.mDirection.length();
  Vector3D normalizedRay = ray.mDirection;
  normalizedRay.normalize();
  for (int i = 0; i < 6; ++i) {
    double denom = planes[i].mNormal.dot(normalizedRay);

    if (denom < SMALL_NUM) {
      continue;
    }

    Vector3D rayOrigToPlanePt = planes[i].mPoint - ray.mOrigin;
    rayOrigToPlanePt.normalize();

    double t = rayOrigToPlanePt.dot(planes[i].mNormal) / denom;
    if (t < 0.0) {
      continue;
    }

    Point3D intersectionPoint = ray.mOrigin + (t * normalizedRay);

    if (PointOnBoxFace(intersectionPoint, corner1[i], corner2[i])) {
      if (t < dist) {
        return true;
      }
    }
  }
  return false;
}

bool NonhierBox::PointOnBoxFace(Point3D pt, Point3D corner1, Point3D corner2) {
  double min_x = std::min(corner1[0], corner2[0]);
  double max_x = std::max(corner1[0], corner2[0]);
  double min_y = std::min(corner1[1], corner2[1]);
  double max_y = std::max(corner1[1], corner2[1]);
  double min_z = std::min(corner1[2], corner2[2]);
  double max_z = std::max(corner1[2], corner2[2]);
  if(pt[0] >= min_x && pt[0] <= max_x && 
    pt[1] >= min_y && pt[1] <= max_y &&
    pt[2] >= min_z && pt[2] <= max_z) {
    return true;
  }

  return false;
}