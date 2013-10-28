#ifndef CS488_MESH_HPP
#define CS488_MESH_HPP

#include <vector>
#include <iosfwd>
#include "primitive.hpp"
#include "algebra.hpp"

// A polygonal mesh.
class Mesh : public Primitive {
public:
  Mesh(const std::vector<Point3D>& verts,
       const std::vector< std::vector<int> >& faces);

  typedef std::vector<int> Face;

  virtual IntersectObj* intersect(Ray ray);
  virtual bool isInShadow(Ray ray) const;

  bool checkAgainstBoundingBox(Ray ray);

private:

  bool rayIntersectFace(Ray ray, const Face& face, double& t) const;
  bool rayIntersectTriangle(Ray ray, const Point3D& v0, const Point3D& v1, const Point3D& v2, double& t) const;

  std::vector<Point3D> m_verts;
  std::vector<Face> m_faces;

  friend std::ostream& operator<<(std::ostream& out, const Mesh& mesh);
};

#endif
