#include "mesh.hpp"
#include <iostream>

static const double EPSILON = 1e-6;
static const double BIG_NUMBER = 1e8;

Mesh::Mesh(const std::vector<Point3D>& verts,
           const std::vector< std::vector<int> >& faces)
  : m_verts(verts),
    m_faces(faces)
{
  mBoundingBox.mMin = m_verts[0];
  mBoundingBox.mMax = m_verts[0];
  for (auto& vert : m_verts) {
    if (vert[0] < mBoundingBox.mMin[0]) {
      mBoundingBox.mMin[0] = vert[0];
    }

    if (vert[0] > mBoundingBox.mMax[0]) {
      mBoundingBox.mMax[0] = vert[0];
    }

    if (vert[1] < mBoundingBox.mMin[1]) {
      mBoundingBox.mMin[1] = vert[1];
    }

    if (vert[1] > mBoundingBox.mMax[1]) {
      mBoundingBox.mMax[1] = vert[1];
    }

    if (vert[2] < mBoundingBox.mMin[2]) {
      mBoundingBox.mMin[2] = vert[2];
    }

    if (vert[2] > mBoundingBox.mMax[2]) {
      mBoundingBox.mMax[2] = vert[2];
    }
  }

  // std::cout << "mBoundingBox: \n" << mBoundingBox.mMin << std::endl << mBoundingBox.mMax << std::endl;
}

std::ostream& operator<<(std::ostream& out, const Mesh& mesh)
{
  std::cerr << "mesh({";
  for (std::vector<Point3D>::const_iterator I = mesh.m_verts.begin(); I != mesh.m_verts.end(); ++I) {
    if (I != mesh.m_verts.begin()) std::cerr << ",\n      ";
    std::cerr << *I;
  }
  std::cerr << "},\n\n     {";
  
  for (std::vector<Mesh::Face>::const_iterator I = mesh.m_faces.begin(); I != mesh.m_faces.end(); ++I) {
    if (I != mesh.m_faces.begin()) std::cerr << ",\n      ";
    std::cerr << "[";
    for (Mesh::Face::const_iterator J = I->begin(); J != I->end(); ++J) {
      if (J != I->begin()) std::cerr << ", ";
      std::cerr << *J;
    }
    std::cerr << "]";
  }
  std::cerr << "});" << std::endl;
  return out;
}


bool Mesh::checkAgainstBoundingBox(Ray ray) const {

  double tmin = (mBoundingBox.mMin[0] - ray.mOrigin[0]) / ray.mDirection[0];
  double tmax = (mBoundingBox.mMax[0] - ray.mOrigin[0]) / ray.mDirection[0];

  if (tmin > tmax) {
    double temp = tmin;
    tmin = tmax;
    tmax = temp;
  }

  double tymin = (mBoundingBox.mMin[1] - ray.mOrigin[1]) / ray.mDirection[1];
  double tymax = (mBoundingBox.mMax[1] - ray.mOrigin[1]) / ray.mDirection[1];

  if (tymin > tymax) {
    double temp = tymin;
    tymin = tymax;
    tymax = temp;
  }

  if ((tmin > tymax) || (tymin > tmax)) {
    return false;
  }

  if (tymin > tmin) {
    tmin = tymin;
  }

  if (tymax < tmax) {
    tmax = tymax;
  }

  double tzmin = (mBoundingBox.mMin[2] - ray.mOrigin[2]) / ray.mDirection[2];
  double tzmax = (mBoundingBox.mMax[2] - ray.mOrigin[2]) / ray.mDirection[2];

  if (tzmin > tzmax) {
    double temp = tzmin;
    tzmin = tzmax;
    tzmax = temp;
  }

  if ((tmin > tzmax) || (tzmin > tmax)) {
    return false;
  }

  if (tzmin > tmin) {
    tmin = tzmin;
  }

  if (tzmax < tmax) {
    tmax = tzmax;
  }

  return true;
}

IntersectObj* Mesh::intersect(Ray ray) {
  if (!checkAgainstBoundingBox(ray)) {
    return NULL;
  }
  std::cout << "I'm here!" << std::endl;

  IntersectObj* obj = new IntersectObj();

  double t_closest = 1000000.0; // TODO: fix all these maxs
  int t_index = -1;

  for (unsigned int i = 0; i < m_faces.size(); ++i) {
    double t = 1000000.0;
    if (rayIntersectFace(ray, m_faces.at(i), t)) {
      if (t < t_closest) {
        t_closest = t;
      t_index = i;
      }
    }
  }

  if (t_index == -1) {
    delete obj;
    return NULL;
  }

  obj->mPoint = ray.mOrigin + (t_closest * ray.mDirection);
  Face face = m_faces.at(t_index);
  Point3D v0 = m_verts.at(face.at(0));
  Point3D v1 = m_verts.at(face.at(1));
  Point3D v2 = m_verts.at(face.at(2));

  Vector3D v0v1 = v1 - v0;
  Vector3D v0v2 = v2 - v0;
  obj->mNormal = v0v1.cross(v0v2);
  obj->mNormal.normalize();
  obj->mDistance = getDistance(obj->mPoint, ray.mOrigin);

  if (obj->mDistance > BIG_NUMBER) {
    delete obj;
    return NULL;
  }

  return obj;
}

bool Mesh::rayIntersectFace(Ray ray, const Face& face, double& t) const {
  t = 100000000.0;
  double t_temp;
  bool ret = false;
  for (unsigned int i = 2; i < face.size(); i++) {
    if(rayIntersectTriangle(
        ray, 
        m_verts.at(face.at(0)),
        m_verts.at(face.at(i-1)),
        m_verts.at(face.at(i)), 
        t_temp)) {
      if (t_temp < t) {
        t = t_temp;
        ret = true;
      }
    }
  }

  return ret;
}

bool Mesh::rayIntersectTriangle(
    Ray ray, 
    const Point3D& v0, 
    const Point3D& v1, 
    const Point3D& v2,
    double& t
  ) const {

  Vector3D v0v1 = v1 - v0;
  Vector3D v0v2 = v2 - v0;
  // Vector3D normal = v0v1.cross(v0v2);

  Vector3D pvec = ray.mDirection.cross(v0v2);
  double det = v0v1.dot(pvec);

  if (det > -EPSILON && det < EPSILON) {
    return false;
  }

  double invDet = 1/det;

  Vector3D pointToOrigin = ray.mOrigin - v0;
  double u = pointToOrigin.dot(pvec) * invDet;

  if (u < 0.0 || u > 1.0) {
    return false;
  }

   Vector3D qvec = pointToOrigin.cross(v0v1);
  double v = ray.mDirection.dot(qvec) * invDet;

  if (v < 0 || u + v > 1) {
    return false;
  }

  t = v0v2.dot(qvec) * invDet;

  return  true;
}

bool Mesh::isInShadow(Ray ray) const {
  if (!checkAgainstBoundingBox(ray)) {
    return false;
  }

  double t = 1000000.0;

  for (unsigned int i = 0; i < m_faces.size(); ++i) {
    if (rayIntersectFace(ray, m_faces.at(i), t)) {
      if (t < EPSILON) {
        continue;
      }
      return true;
    }
  }
  return false;
}