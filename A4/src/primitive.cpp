#include "primitive.hpp"
#include "polyroots.hpp"
#include <cmath>

static const double EPSILON = 1e-8;
static const double BIG_NUMBER = 1e8;

Primitive::Primitive()
{
}

Primitive::~Primitive()
{
}

AABB Primitive::getBoundingBox() {
  return mBoundingBox;
}

double Primitive::getDistance(const Point3D& p0, const Point3D& p1) {
  Vector3D ray = p0 - p1;
  return ray.length();
}


Sphere::Sphere()
{
  mBoundingBox.mMin = Point3D(-1.0, -1.0, -1.0);
  mBoundingBox.mMax = Point3D(1.0, 1.0, 1.0);
}

Sphere::~Sphere()
{
}

IntersectObj* Sphere::intersect(Ray ray) {
  // TODO
  Point3D center(0.0, 0.0, 0.0);
  double A = ray.mDirection.dot(ray.mDirection);
  Vector3D centerToEye = ray.mOrigin - center;
  double B = 2 * centerToEye.dot(ray.mDirection);
  double C = centerToEye.dot(centerToEye) - 1.0;
  double roots[2];

  // Calate for real roots
  size_t numOfRealRoots = quadraticRoots(A, B, C, roots);
  
  if (numOfRealRoots == 0) {
    // std::cout << "NO REAL ROOTS!" << std::endl;
    return NULL;
  }

  // std::cout << "REAL ROOTS!" << std::endl;
  IntersectObj* obj = new IntersectObj();
  double t = BIG_NUMBER;
  for (unsigned int i = 0; i < numOfRealRoots; ++i) {
    if (roots[i] < 0.0) {
      continue;
    }
    if (roots[i] < t) {
      t = roots[i];
    }
  }

  // Get the point of intersection
  obj->mPoint = ray.mOrigin + (t * ray.mDirection);
  obj->mNormal = obj->mPoint - center;
  obj->mDistance = getDistance(obj->mPoint, ray.mOrigin);
  // obj->mNormal.normalize();

  return obj;
}

bool Sphere::isInShadow(Ray ray) const {
  Point3D center(0.0, 0.0, 0.0);
  double A = ray.mDirection.dot(ray.mDirection);
  Vector3D centerToEye = ray.mOrigin - Point3D(0.0, 0.0, 0.0);
  // centerToEye.normalize();
  double B = 2 * centerToEye.dot(ray.mDirection);
  double C = centerToEye.dot(centerToEye) - 1.0;
  double roots[2];

  // Calate for real roots
  size_t numOfRealRoots = quadraticRoots(A, B, C, roots);
  
  if (numOfRealRoots == 0) {
    // There are no intersections with this object
    return false;
  } 

  double dist = ray.mDirection.length();
  for (unsigned int i = 0; i < numOfRealRoots; ++i) {
    if (roots[i] < EPSILON) {
      continue;
    }
    if (roots[i] < (dist)) {
      return true;
    }
  }

  return false;
}

Cube::Cube()
{
  mBoundingBox.mMin = Point3D(-1, -1, -1);
  mBoundingBox.mMax = Point3D(1, 1, 1);
}

Cube::~Cube()
{
}

IntersectObj* Cube::intersect(Ray ray) {
  double t_min = 0.0;
  double t_max = BIG_NUMBER;

  Vector3D delta = Point3D(0.0, 0.0, 0.0) - ray.mOrigin;

  // Compute the intersections with 2 planes that delimit the x Axis
  Vector3D xAxis = Vector3D(1.0, 0, 0);
  Vector3D yAxis = Vector3D(0, 1.0, 0);
  Vector3D zAxis = Vector3D(0, 0, 1.0);

  Vector3D curAxis;
  double e_x = xAxis.dot(delta);
  double f_x = ray.mDirection.dot(xAxis);
  double e_y = yAxis.dot(delta);
  double f_y = ray.mDirection.dot(yAxis);
  double e_z = zAxis.dot(delta);
  double f_z = ray.mDirection.dot(zAxis);
  
  double t1_x = (e_x + mBoundingBox.mMin[0])/f_x; // intersection with left plane
  double t2_x = (e_x + mBoundingBox.mMax[0])/f_x; // intersection with right plane
  double t1_y = (e_y + mBoundingBox.mMin[1])/f_y; // intersection with top plane
  double t2_y = (e_y + mBoundingBox.mMax[1])/f_y; // intersection with bottom plane
  double t1_z = (e_z + mBoundingBox.mMin[2])/f_z; // intersection with front plane
  double t2_z = (e_z + mBoundingBox.mMax[2])/f_z; // intersection with back plane

  IntersectObj* obj = new IntersectObj();
  // X axis
  curAxis = -xAxis;
  if (t1_x > t2_x) {
    curAxis = xAxis;
    double temp = t1_x;
    t1_x = t2_x;
    t2_x = temp;
  }

  if (t2_x < t_max) {
    t_max = t2_x;
  }

  if (t1_x > t_min) { 
    t_min = t1_x;
    Point3D intersectionPoint = ray.mOrigin + (t_min * ray.mDirection);
    obj->mDistance = getDistance(obj->mPoint, ray.mOrigin);
    obj->mPoint = intersectionPoint;
    obj->mNormal = curAxis;
  }

  // Y axis
  curAxis = -yAxis;
  if (t1_y > t2_y) {
    curAxis = yAxis;
    double temp = t1_y;
    t1_y = t2_y;
    t2_y = temp;
  }

  if (t2_y < t_max) {
    t_max = t2_y;
  }

  if (t1_y > t_min) { 
    t_min = t1_y;
    Point3D intersectionPoint = ray.mOrigin + (t_min * ray.mDirection);
    obj->mDistance = getDistance(obj->mPoint, ray.mOrigin);
    obj->mPoint = intersectionPoint;
    obj->mNormal = curAxis;
  }

  // Z Axis
  curAxis = -zAxis;
  if (t1_z > t2_z) {
    curAxis = zAxis;
    double temp = t1_z;
    t1_z = t2_z;
    t2_z = temp;
  }

  if (t2_z < t_max) {
    t_max = t2_z;
  }

  if (t1_z > t_min) { 
    t_min = t1_z;
    Point3D intersectionPoint = ray.mOrigin + (t_min * ray.mDirection);
    obj->mDistance = getDistance(obj->mPoint, ray.mOrigin);
    obj->mPoint = intersectionPoint;
    obj->mNormal = curAxis;
  }

  if (t_max < t_min) {
    delete obj;
    return NULL;
  }

  return obj;
}

bool Cube::isInShadow(Ray ray) const {
  double t_min = 0.0;
  double t_max = BIG_NUMBER;

  Vector3D delta = Point3D(0.0, 0.0, 0.0) - ray.mOrigin;

  // Compute the intersections with 2 planes that delimit the x Axis
  Vector3D xAxis = Vector3D(1.0, 0, 0);
  Vector3D yAxis = Vector3D(0, 1.0, 0);
  Vector3D zAxis = Vector3D(0, 0, 1.0);
  double e_x = xAxis.dot(delta);
  double f_x = ray.mDirection.dot(xAxis);
  double e_y = yAxis.dot(delta);
  double f_y = ray.mDirection.dot(yAxis);
  double e_z = zAxis.dot(delta);
  double f_z = ray.mDirection.dot(zAxis);
  
  double t1_x = (e_x + mBoundingBox.mMin[0])/f_x; // intersection with left plane
  double t2_x = (e_x + mBoundingBox.mMax[0])/f_x; // intersection with right plane
  double t1_y = (e_y + mBoundingBox.mMin[1])/f_y; // intersection with top plane
  double t2_y = (e_y + mBoundingBox.mMax[1])/f_y; // intersection with bottom plane
  double t1_z = (e_z + mBoundingBox.mMin[2])/f_z; // intersection with front plane
  double t2_z = (e_z + mBoundingBox.mMax[2])/f_z; // intersection with back plane

  // X axis
  if (t1_x > t2_x) {
    double temp = t1_x;
    t1_x = t2_x;
    t2_x = temp;
  }

  if (t2_x < t_max) {
    t_max = t2_x;
  }

  if (t1_x > t_min) { 
    t_min = t1_x;
  }

  // Y axis
  if (t1_y > t2_y) {
    double temp = t1_y;
    t1_y = t2_y;
    t2_y = temp;
  }

  if (t2_y < t_max) {
    t_max = t2_y;
  }

  if (t1_y > t_min) { 
    t_min = t1_y;
  }

  // Z Axis
  if (t1_z > t2_z) {
    double temp = t1_z;
    t1_z = t2_z;
    t2_z = temp;
  }

  if (t2_z < t_max) {
    t_max = t2_z;
  }

  if (t1_z > t_min) { 
    t_min = t1_z;
  }


  // IntersectObj* obj = new IntersectObj();
  // Point3D intersectionPoint = ray.mOrigin + (t_min * ray.mDirection);
  // double distance = getDistance(intersectionPoint, ray.mOrigin);
  if (t_min < EPSILON) {
    std::cout << "t_min: " << t_min << std::endl;
    return false;
  }

  if (t_max < t_min) {
    return false;
  }

  return true;

}

NonhierSphere::NonhierSphere(const Point3D& pos, double radius)
  : mPos(pos), mRadius(radius)
{
  mBoundingBox.mMin = Point3D(mPos[0] - mRadius, mPos[1] - mRadius, mPos[2] - mRadius);
  mBoundingBox.mMax = Point3D(mPos[0] + mRadius, mPos[1] + mRadius, mPos[2] + mRadius);
}

NonhierSphere::~NonhierSphere()
{
}

IntersectObj* NonhierSphere::intersect(Ray ray) {

  double A = ray.mDirection.dot(ray.mDirection);
  // std::cout << "Ray Origin: " << ray.mOrigin << std::endl;
  Vector3D centerToEye = ray.mOrigin - mPos;
  // std::cout << "Center To Eye: " << centerToEye << std::endl;
  // centerToEye.normalize();
  double B = 2 * centerToEye.dot(ray.mDirection);
  double C = centerToEye.dot(centerToEye) - (mRadius * mRadius);
  double roots[2];

  // std::cout << "A: " << A << " B: " << B << " C: " << C << std::endl;

  // Calate for real roots
  size_t numOfRealRoots = quadraticRoots(A, B, C, roots);
  
  if (numOfRealRoots == 0) {
    // std::cout << "NO REAL ROOTS." << std::endl;
    // There are no intersections with this object
    return NULL;
  }

  // std::cout << "Distance: ";
  IntersectObj* obj = new IntersectObj();
  double t = BIG_NUMBER;
  for (unsigned int i = 0; i < numOfRealRoots; ++i) {
    if (roots[i] < 0.0) {
      continue;
    }
    if (roots[i] < t) {
      t = roots[i];
    }
  }
  // std::cout << std::endl;

  // Get the point of intersection
  obj->mPoint = ray.mOrigin + (t * ray.mDirection);
  obj->mNormal = obj->mPoint - mPos;
  obj->mDistance = getDistance(obj->mPoint, ray.mOrigin);
  // obj->mNormal.normalize();

  return obj;
}

bool NonhierSphere::isInShadow(Ray ray) const {
  double A = ray.mDirection.dot(ray.mDirection);
  Vector3D centerToEye = ray.mOrigin - mPos;
  // centerToEye.normalize();
  double B = 2 * centerToEye.dot(ray.mDirection);
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
    if (roots[i] < EPSILON) {
      continue;
    }
    if (roots[i] < dist) {
      return true;
    }
  }

  return false;
}

NonhierBox::NonhierBox(const Point3D& pos, double size)
  : mPos(pos), mSize(size)
{
  mBoundingBox.mMin = Point3D(mPos[0] - mSize, mPos[1] - mSize, mPos[2] - mSize);
  mBoundingBox.mMax = Point3D(mPos[0] + mSize, mPos[1] + mSize, mPos[2] + mSize);
}

NonhierBox::~NonhierBox()
{
}

IntersectObj* NonhierBox::intersect(Ray ray) {
  double t_min = 0.0;
  double t_max = BIG_NUMBER;

  Vector3D delta = mPos - ray.mOrigin;

  // Compute the intersections with 2 planes that delimit the x Axis
  Vector3D xAxis = Vector3D(1.0, 0, 0);
  Vector3D yAxis = Vector3D(0, 1.0, 0);
  Vector3D zAxis = Vector3D(0, 0, 1.0);

  Vector3D curAxis;
  double e_x = xAxis.dot(delta);
  double f_x = ray.mDirection.dot(xAxis);
  double e_y = yAxis.dot(delta);
  double f_y = ray.mDirection.dot(yAxis);
  double e_z = zAxis.dot(delta);
  double f_z = ray.mDirection.dot(zAxis);
  
  double t1_x = (e_x + mBoundingBox.mMin[0])/f_x; // intersection with left plane
  double t2_x = (e_x + mBoundingBox.mMax[0])/f_x; // intersection with right plane
  double t1_y = (e_y + mBoundingBox.mMin[1])/f_y; // intersection with top plane
  double t2_y = (e_y + mBoundingBox.mMax[1])/f_y; // intersection with bottom plane
  double t1_z = (e_z + mBoundingBox.mMin[2])/f_z; // intersection with front plane
  double t2_z = (e_z + mBoundingBox.mMax[2])/f_z; // intersection with back plane

  IntersectObj* obj = new IntersectObj();
  // X axis
  curAxis = -xAxis;
  if (t1_x > t2_x) {
    curAxis = xAxis;
    double temp = t1_x;
    t1_x = t2_x;
    t2_x = temp;
  }

  if (t2_x < t_max) {
    t_max = t2_x;
  }

  if (t1_x > t_min) { 
    t_min = t1_x;
    Point3D intersectionPoint = ray.mOrigin + (t_min * ray.mDirection);
    obj->mPoint = intersectionPoint;
    obj->mDistance = getDistance(obj->mPoint, ray.mOrigin);
    obj->mNormal = curAxis;
  }

  // Y axis
  curAxis = -yAxis;
  if (t1_y > t2_y) {
    curAxis = yAxis;
    double temp = t1_y;
    t1_y = t2_y;
    t2_y = temp;
  }

  if (t2_y < t_max) {
    t_max = t2_y;
  }

  if (t1_y > t_min) { 
    t_min = t1_y;
    Point3D intersectionPoint = ray.mOrigin + (t_min * ray.mDirection);
    obj->mPoint = intersectionPoint;
    obj->mDistance = getDistance(obj->mPoint, ray.mOrigin);
    obj->mNormal = curAxis;
  }

  // Z Axis
  curAxis = -zAxis;
  if (t1_z > t2_z) {
    curAxis = zAxis;
    double temp = t1_z;
    t1_z = t2_z;
    t2_z = temp;
  }

  if (t2_z < t_max) {
    t_max = t2_z;
  }

  if (t1_z > t_min) { 
    t_min = t1_z;
    Point3D intersectionPoint = ray.mOrigin + (t_min * ray.mDirection);
    obj->mPoint = intersectionPoint;
    obj->mDistance = getDistance(obj->mPoint, ray.mOrigin);
    obj->mNormal = curAxis;
  }

  if (t_max < t_min) {
    delete obj;
    return NULL;
  }

  return obj;
}

bool NonhierBox::isInShadow(Ray ray) const {
  double t_min = 0.0;
  double t_max = BIG_NUMBER;


  Vector3D delta = mPos - ray.mOrigin;

  // Compute the intersections with 2 planes that delimit the x Axis
  Vector3D xAxis = Vector3D(1.0, 0, 0);
  Vector3D yAxis = Vector3D(0, 1.0, 0);
  Vector3D zAxis = Vector3D(0, 0, 1.0);
  double e_x = xAxis.dot(delta);
  double f_x = ray.mDirection.dot(xAxis);
  double e_y = yAxis.dot(delta);
  double f_y = ray.mDirection.dot(yAxis);
  double e_z = zAxis.dot(delta);
  double f_z = ray.mDirection.dot(zAxis);
  
  double t1_x = (e_x + mBoundingBox.mMin[0])/f_x; // intersection with left plane
  double t2_x = (e_x + mBoundingBox.mMax[0])/f_x; // intersection with right plane
  double t1_y = (e_y + mBoundingBox.mMin[1])/f_y; // intersection with top plane
  double t2_y = (e_y + mBoundingBox.mMax[1])/f_y; // intersection with bottom plane
  double t1_z = (e_z + mBoundingBox.mMin[2])/f_z; // intersection with front plane
  double t2_z = (e_z + mBoundingBox.mMax[2])/f_z; // intersection with back plane

  // X axis
  if (t1_x > t2_x) {
    double temp = t1_x;
    t1_x = t2_x;
    t2_x = temp;
  }

  if (t2_x < t_max) {
    t_max = t2_x;
  }

  if (t1_x > t_min) { 
    t_min = t1_x;
  }

  // Y axis
  if (t1_y > t2_y) {
    double temp = t1_y;
    t1_y = t2_y;
    t2_y = temp;
  }

  if (t2_y < t_max) {
    t_max = t2_y;
  }

  if (t1_y > t_min) { 
    t_min = t1_y;
  }

  // Z Axis
  if (t1_z > t2_z) {
    double temp = t1_z;
    t1_z = t2_z;
    t2_z = temp;
  }

  if (t2_z < t_max) {
    t_max = t2_z;
  }

  if (t1_z > t_min) { 
    t_min = t1_z;
  }

  if (t_min < EPSILON) {
    std::cout << "t_min: " << t_min << std::endl;
    return false;
  }

  // if (t_max < t_min && t_min < EPSILON) {
  if (t_max < t_min) {
    return false;
  }

  return true;
}

bool NonhierBox::PointOnBoxFace(Point3D pt, Point3D corner1, Point3D corner2) {
  // std::cout << "Point: " << pt << std::endl;
  // std::cout << "Corner1: " << corner1 << std::endl;
  // std::cout << "Corner2: " << corner2 << std::endl;
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
  // std::cout << "I'm returning false. " << std::endl;
  return false;
}