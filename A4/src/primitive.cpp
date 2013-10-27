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
  Point3D center(0.0, 0.0, 0.0);
  double A = ray.mDirection.dot(ray.mDirection);
  Vector3D centerToEye = ray.mOrigin - center;
  double B = 2 * centerToEye.dot(ray.mDirection);
  double C = centerToEye.dot(centerToEye);
  double roots[2];

  // Calate for real roots
  size_t numOfRealRoots = quadraticRoots(A, B, C, roots);
  
  if (numOfRealRoots == 0) {
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
  obj->mNormal = obj->mPoint - center;
  obj->mNormal.normalize();

  return obj;
}

bool Sphere::isInShadow(Ray ray) const {
  Point3D center(0.0, 0.0, 0.0);
  double A = ray.mDirection.dot(ray.mDirection);
  Vector3D centerToEye = ray.mOrigin - Point3D(0.0, 0.0, 0.0);
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
    if (roots[i] < 0) {
      continue;
    }
    if (roots[i] < dist) {
      return true;
    }
  }

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
  for (unsigned int i = 0; i < numOfRealRoots; ++i) {
    // std::cout << roots[i] << ", ";
    if (roots[i] < 0) {
      continue;
    }
    if (roots[i] < obj->mDistance) {
      obj->mDistance = roots[i];
    }
  }
  // std::cout << std::endl;

  // Get the point of intersection
  obj->mPoint = ray.mOrigin + (obj->mDistance * ray.mDirection);
  obj->mNormal = obj->mPoint - mPos;
  obj->mNormal.normalize();

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
  double t_min = 0.0;
  double t_max = 100000.0;

  Vector3D delta = mPos - ray.mOrigin;

  Point3D AABB_min = Point3D(mPos[0] - mSize/2.0, mPos[1] - mSize/2.0, mPos[2] - mSize/2.0);
  Point3D AABB_max = Point3D(mPos[0] + mSize/2.0, mPos[1] + mSize/2.0, mPos[2] + mSize/2.0);

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
  
  double t1_x = (e_x + AABB_min[0])/f_x; // intersection with left plane
  double t2_x = (e_x + AABB_max[0])/f_x; // intersection with right plane
  double t1_y = (e_y + AABB_min[1])/f_y; // intersection with top plane
  double t2_y = (e_y + AABB_max[1])/f_y; // intersection with bottom plane
  double t1_z = (e_z + AABB_min[2])/f_z; // intersection with front plane
  double t2_z = (e_z + AABB_max[2])/f_z; // intersection with back plane

  IntersectObj* obj = new IntersectObj();
  Vector3D normalizedRay = ray.mDirection;
  normalizedRay.normalize();
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
    Point3D intersectionPoint = ray.mOrigin + (t_min * normalizedRay);
    obj->mDistance = t_min;
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
    Point3D intersectionPoint = ray.mOrigin + (t_min * normalizedRay);
    obj->mDistance = t_min;
    obj->mPoint = intersectionPoint;
    obj->mNormal = curAxis;
  }

  // Z Axis
  curAxis = zAxis;
  if (t1_z > t2_z) {
    curAxis = -zAxis;
    double temp = t1_z;
    t1_z = t2_z;
    t2_z = temp;
  }

  if (t2_z < t_max) {
    t_max = t2_z;
  }

  if (t1_z > t_min) { 
    t_min = t1_z;
    Point3D intersectionPoint = ray.mOrigin + (t_min * normalizedRay);
    obj->mDistance = t_min;
    obj->mPoint = intersectionPoint;
    obj->mNormal = curAxis;
  }

  if (t_max < t_min) {
    delete obj;
    return NULL;
  }

  return obj;




  // Plane planes[6]; 
  // Point3D corner1[6];
  // Point3D corner2[6];

  // planes[0].mPoint = mPos + Vector3D(mSize/2.0, 0.0, 0.0);
  // corner1[0] = planes[0].mPoint + Vector3D(0.0, mSize/2.0, -mSize/2.0);
  // corner2[0] = planes[0].mPoint + Vector3D(0.0, -mSize/2.0, mSize/2.0);

  // planes[1].mPoint = mPos + Vector3D(-mSize/2.0, 0.0, 0.0);
  // corner1[1] = planes[1].mPoint + Vector3D(0.0, mSize/2.0, -mSize/2.0);
  // corner2[1] = planes[1].mPoint + Vector3D(0.0, -mSize/2.0, mSize/2.0);

  // planes[2].mPoint = mPos + Vector3D(0.0, mSize/2.0, 0.0);
  // corner1[2] = planes[2].mPoint + Vector3D(mSize/2.0, 0.0, -mSize/2.0);
  // corner2[2] = planes[2].mPoint + Vector3D(-mSize/2.0, 0.0, mSize/2.0);

  // planes[3].mPoint = mPos + Vector3D(0.0, -mSize/2.0, 0.0);
  // corner1[3] = planes[3].mPoint + Vector3D(mSize/2.0, 0.0, -mSize/2.0);
  // corner2[3] = planes[3].mPoint + Vector3D(-mSize/2.0, 0.0, mSize/2.0);

  // planes[4].mPoint = mPos + Vector3D(0.0, 0.0, mSize/2.0);
  // corner1[4] = planes[4].mPoint + Vector3D(mSize/2.0, -mSize/2.0, 0.0);
  // corner2[4] = planes[4].mPoint + Vector3D(-mSize/2.0, mSize/2.0, 0.0);

  // planes[5].mPoint = mPos + Vector3D(0.0, 0.0, -mSize/2.0);
  // corner1[5] = planes[5].mPoint + Vector3D(mSize/2.0, -mSize/2.0, 0.0);
  // corner2[5] = planes[5].mPoint + Vector3D(-mSize/2.0, mSize/2.0, 0.0);

  // for (int i = 0; i < 6; ++i) {
  //   planes[i].mNormal = planes[i].mPoint - mPos;
  //   planes[i].mNormal.normalize();
  // }

  // double dist = ray.mDirection.length();
  // Vector3D normalizedRay = ray.mDirection;
  // normalizedRay.normalize();
  // IntersectObj* obj = new IntersectObj();
  // for (int i = 0; i < 6; ++i) {
  //   double denom = planes[i].mNormal.dot(normalizedRay);

  //   if (denom <= SMALL_NUM) {
  //     continue;
  //   }

  //   Vector3D rayOrigToPlanePt = planes[i].mPoint - ray.mOrigin;
  //   // rayOrigToPlanePt.normalize();

  //   double t = rayOrigToPlanePt.dot(planes[i].mNormal) / denom;
  //   if (t < 0.0) {
  //     continue;
  //   }
  //   Point3D intersectionPoint = ray.mOrigin + (t * normalizedRay);

  //   // switch (i) {
  //   //   case 0:
  //   //     std::cout << i << " Right" << std::endl;
  //   //   break;
  //   //   case 1: 
  //   //     std::cout << i << " Left" << std::endl;

  //   //   break;
  //   //   case 2: 
  //   //     std::cout << i << " Top" << std::endl;

  //   //   break;
  //   //   case 3:
  //   //     std::cout << i << " Bottom" << std::endl;

  //   //   break;
  //   //   case 4:
  //   //     std::cout << i << " Front" << std::endl;

  //   //   break;
  //   //   case 5:
  //   //     std::cout << i << " Back" << std::endl;

  //   //   break;
  //   // }
  //   // std::cout << i << " t: " << t << std::endl;
  //   if (PointOnBoxFace(intersectionPoint, corner1[i], corner2[i])) {
  //     // std::cout << "I'm on the box face!" << std::endl;
  //     if (t < obj->mDistance) {

  //       obj->mDistance = t;
  //       obj->mPoint = intersectionPoint;
  //       obj->mNormal = planes[i].mNormal;
  //     }
  //   }
  // }

  // if (obj->mDistance >= 100000.0) {
  //   delete obj;
  //   return NULL;
  // }

  // return obj;
}

bool NonhierBox::isInShadow(Ray ray) const {
  double t_min = 0.0;
  double t_max = 100000.0;


  Vector3D delta = mPos - ray.mOrigin;

  Point3D AABB_min = Point3D(mPos[0] - mSize/2.0, mPos[1] - mSize/2.0, mPos[2] - mSize/2.0);
  Point3D AABB_max = Point3D(mPos[0] + mSize/2.0, mPos[1] + mSize/2.0, mPos[2] + mSize/2.0);

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
  
  double t1_x = (e_x + AABB_min[0])/f_x; // intersection with left plane
  double t2_x = (e_x + AABB_max[0])/f_x; // intersection with right plane
  double t1_y = (e_y + AABB_min[1])/f_y; // intersection with top plane
  double t2_y = (e_y + AABB_max[1])/f_y; // intersection with bottom plane
  double t1_z = (e_z + AABB_min[2])/f_z; // intersection with front plane
  double t2_z = (e_z + AABB_max[2])/f_z; // intersection with back plane

  Vector3D normalizedRay = ray.mDirection;
  normalizedRay.normalize();
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

  if (t_max < t_min) {
    // std::cout << "Shdow function: False." << std::endl;
    return false;
  }
  // std::cout << "Shadow function: true." << std::endl;

  return true;


  // // TODO
  // Plane planes[6]; 
  // Point3D corner1[6];
  // Point3D corner2[6];
  // planes[0].mPoint = mPos + Vector3D(mSize/2.0, 0.0, 0.0);
  // corner1[0] = planes[0].mPoint + Vector3D(0.0, mSize/2.0, -mSize/2.0);
  // corner2[0] = planes[0].mPoint + Vector3D(0.0, -mSize/2.0, mSize/2.0);

  // planes[1].mPoint = mPos + Vector3D(-mSize/2.0, 0.0, 0.0);
  // corner1[1] = planes[1].mPoint + Vector3D(0.0, mSize/2.0, -mSize/2.0);
  // corner2[1] = planes[1].mPoint + Vector3D(0.0, -mSize/2.0, mSize/2.0);

  // planes[2].mPoint = mPos + Vector3D(0.0, mSize/2.0, 0.0);
  // corner1[2] = planes[2].mPoint + Vector3D(mSize/2.0, 0.0, -mSize/2.0);
  // corner2[2] = planes[2].mPoint + Vector3D(-mSize/2.0, 0.0, mSize/2.0);

  // planes[3].mPoint = mPos + Vector3D(0.0, -mSize/2.0, 0.0);
  // corner1[3] = planes[3].mPoint + Vector3D(mSize/2.0, 0.0, -mSize/2.0);
  // corner2[3] = planes[3].mPoint + Vector3D(-mSize/2.0, 0.0, mSize/2.0);

  // planes[4].mPoint = mPos + Vector3D(0.0, 0.0, mSize/2.0);
  // corner1[4] = planes[4].mPoint + Vector3D(mSize/2.0, -mSize/2.0, 0.0);
  // corner2[4] = planes[4].mPoint + Vector3D(-mSize/2.0, mSize/2.0, 0.0);

  // planes[5].mPoint = mPos + Vector3D(0.0, 0.0, -mSize/2.0);
  // corner1[5] = planes[5].mPoint + Vector3D(mSize/2.0, -mSize/2.0, 0.0);
  // corner2[5] = planes[5].mPoint + Vector3D(-mSize/2.0, mSize/2.0, 0.0);

  // for (int i = 0; i < 6; ++i) {
  //   planes[i].mNormal = planes[i].mPoint - mPos;
  //   planes[i].mNormal.normalize();
  // }

  // double dist = ray.mDirection.length();
  // Vector3D normalizedRay = ray.mDirection;
  // // normalizedRay.normalize();
  // for (int i = 0; i < 6; ++i) {
  //   double denom = planes[i].mNormal.dot(normalizedRay);

  //   if (denom <= 0) {
  //     continue;
  //   }

  //   Vector3D rayOrigToPlanePt = planes[i].mPoint - ray.mOrigin;
  //   // rayOrigToPlanePt.normalize();

  //   double t = rayOrigToPlanePt.dot(planes[i].mNormal) / denom;
  //   if (t < 0.0) {
  //     continue;
  //   }

  //   Point3D intersectionPoint = ray.mOrigin + (t * normalizedRay);

  //   if (PointOnBoxFace(intersectionPoint, corner1[i], corner2[i])) {
  //     if (t < dist) {
  //       return true;
  //     }
  //   }
  // }
  // return false;
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