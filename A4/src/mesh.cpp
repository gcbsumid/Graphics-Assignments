#include "mesh.hpp"
#include <iostream>

static const double EPSILON = 1e-8;

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
    } else if (vert[0] > mBoundingBox.mMax[0]) {
      mBoundingBox.mMax[0] = vert[0];
    }

    if (vert[1] < mBoundingBox.mMin[1]) {
      mBoundingBox.mMin[1] = vert[1];
    } else if (vert[1] > mBoundingBox.mMax[1]) {
      mBoundingBox.mMax[1] = vert[1];
    }

    if (vert[2] < mBoundingBox.mMin[2]) {
      mBoundingBox.mMin[2] = vert[2];
    } else if (vert[2] > mBoundingBox.mMax[2]) {
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


bool Mesh::checkAgainstBoundingBox(Ray ray) {

  double t_min = -100000.0;
  double t_max = 100000.0;

  Point3D position = Point3D(
                      (mBoundingBox.mMin[0] + mBoundingBox.mMax[0])/2.0,
                      (mBoundingBox.mMin[1] + mBoundingBox.mMax[1])/2.0,
                      (mBoundingBox.mMin[2] + mBoundingBox.mMax[2])/2.0
                      );

  // std::cout << "position: " << position << std::endl;
  Vector3D delta = position - ray.mOrigin;
  // std::cout << "delta: " << delta << std::endl;

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

  // std::cout << "t_max: " << t_max << " t_min: " << t_min << std::endl;
  if (t_max < t_min) {
    return false;
  }



  return true;
}

IntersectObj* Mesh::intersect(Ray ray) {
  // if (!checkAgainstBoundingBox(ray)) {
  //   return NULL;
  // }

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