#ifndef SCENE_HPP
#define SCENE_HPP

#include <list>
#include "algebra.hpp"
#include "primitive.hpp"
#include "material.hpp"

struct IntersectObj;

class SceneNode {
public:
  SceneNode(const std::string& name);
  virtual ~SceneNode();

  virtual IntersectObj* intersect(Ray ray);
  virtual bool isInShadow(Ray ray, SceneNode* node) const;

  const Matrix4x4& get_transform() const { return mTrans; }
  const Matrix4x4& get_inverse() const { return mInvTrans; }
  
  void set_transform(const Matrix4x4& m)
  {
    mTrans = m;
    mInvTrans = m.invert();
  }

  void set_transform(const Matrix4x4& m, const Matrix4x4& i)
  {
    mTrans = m;
    mInvTrans = i;
  }

  void add_child(SceneNode* child)
  {
    mChildren.push_back(child);
    // recalculate_bounding_box();
  }

  void remove_child(SceneNode* child)
  {
    mChildren.remove(child);
    // recalculate_bounding_box();
  }

  // Callbacks to be implemented.
  // These will be called from Lua.
  virtual void rotate(char axis, double angle);
  virtual void scale(const Vector3D& amount);
  virtual void translate(const Vector3D& amount);

  // Returns true if and only if this node is a JointNode
  virtual bool is_joint() const;
  
  std::string get_name() const {
    return mName;
  }

protected:
  // Useful for picking
  int mId;
  std::string mName;

  // Transformations
  Matrix4x4 mTranslate;
  Matrix4x4 mScale;
  Matrix4x4 mRotate;
  Matrix4x4 mInvTranslate;
  Matrix4x4 mInvScale;
  Matrix4x4 mInvRotate;

  Matrix4x4 mTrans;
  Matrix4x4 mInvTrans;

  // Hierarchy
  typedef std::list<SceneNode*> ChildList;
  ChildList mChildren;
};

class JointNode : public SceneNode {
public:
  JointNode(const std::string& name);
  virtual ~JointNode();

  virtual bool is_joint() const;

  void set_joint_x(double min, double init, double max);
  void set_joint_y(double min, double init, double max);

  struct JointRange {
    double min, init, max;
  };

  
protected:

  JointRange m_joint_x, m_joint_y;
};

class GeometryNode : public SceneNode {
public:
  GeometryNode(const std::string& name,
               Primitive* primitive);
  virtual ~GeometryNode();

  virtual IntersectObj* intersect(Ray ray);
  virtual bool isInShadow(Ray ray, SceneNode* node) const;
  virtual void scale(const Vector3D& amount);

  const Material* get_material() const;
  Material* get_material();

  void set_material(Material* material)
  {
    mMaterial = material;
  }

protected:

  Material* mMaterial;
  Primitive* mPrimitive;
  // Matrix4x4 mScale;
  // Matrix4x4 mInvScale;
}; 

#endif
