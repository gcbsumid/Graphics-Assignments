#include "mesh.hpp"
#include <iostream>

Mesh::Mesh(const std::vector<Point3D>& verts,
           const std::vector< std::vector<int> >& faces)
  : m_verts(verts),
    m_faces(faces)
{
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

IntersectObj* Mesh::intersect(Ray ray) {

  IntersectObj* obj = new IntersectObj();

  double t_closest = 1000000.0; // TODO: fix all these maxs
  int t_index = -1;

  for (unsigned int i = 0; i < m_faces.size(); ++i) {
    double t = 1000000.0;
    if (rayIntersectTriangle(ray, m_faces.at(i), t)) {
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

  return obj;
}

bool Mesh::rayIntersectTriangle(Ray ray, const Face& face, double& t) const {
  Point3D v0 = m_verts.at(face.at(0));
  Point3D v1 = m_verts.at(face.at(1));
  Point3D v2 = m_verts.at(face.at(2));

  Vector3D v0v1 = v1 - v0;
  Vector3D v0v2 = v2 - v0;
  Vector3D normal = v0v1.cross(v0v2);
  double normalDotRay = normal.dot(ray.mDirection); 
  if (normalDotRay == 0) { // ray parallel to the triangle
    return false;
  }

  double d = normal.dot(Vector3D(v0[0], v0[1], v0[2]));
  t = -(normal.dot(Vector3D(ray.mOrigin[0], ray.mOrigin[1], ray.mOrigin[2])) + d) / normalDotRay;

  // inside-out test 
  Point3D intersectionPoint = ray.mOrigin + (t * ray.mDirection);

  // inside out test edge 0;
  Vector3D e0 = intersectionPoint - v0;
  if (normal.dot(v0v1.cross(e0)) < 0) {
    return false;
  }

  // inside out test edge 1;
  Vector3D e1 = intersectionPoint - v1;
  Vector3D v1v2 = v2 - v1;
  if (normal.dot(v1v2.cross(e1)) < 0) {
    return false;
  }

  // inside out test edge 2;
  Vector3D e2 = intersectionPoint - v2;
  Vector3D v2v0 = v0 - v2;
  if (normal.dot(v2v0.cross(e2)) < 0) {
    return false;
  }

  return true;
}

bool Mesh::isInShadow(Ray ray) const {
  double t = 1000000.0;

  for (unsigned int i = 0; i < m_faces.size(); ++i) {
    if (rayIntersectTriangle(ray, m_faces.at(i), t)) {
      return true;
    }
  }
  return false;
}