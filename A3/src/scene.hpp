#ifndef SCENE_HPP
#define SCENE_HPP

#include <list>
#include "algebra.hpp"
#include "primitive.hpp"
#include "material.hpp"

class SceneNode {
public:
  SceneNode(const std::string& name);
  virtual ~SceneNode();

  virtual void walk_gl(bool picking = false) const;

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
  }

  void remove_child(SceneNode* child)
  {
    mChildren.remove(child);
  }

  // Callbacks to be implemented.
  // These will be called from Lua.
  void rotate(char axis, double angle);
  void scale(const Vector3D& amount);
  void translate(const Vector3D& amount);

  // Returns true if and only if this node is a JointNode
  virtual bool is_joint() const;
  
protected:
  
  // Useful for picking
  int mId;
  std::string mName;

  // Transformations
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

  virtual void walk_gl(bool bicking = false) const;

  virtual bool is_joint() const;

  void set_joint_x(double min, double init, double max);
  void set_joint_y(double min, double init, double max);

  struct JointRange {
    double min, init, max;
  };

  
protected:

  JointRange mJointX, mJointY;
};

class GeometryNode : public SceneNode {
public:
  GeometryNode(const std::string& name,
               Primitive* primitive);
  virtual ~GeometryNode();

  virtual void walk_gl(bool picking = false) const;

  const Material* get_material() const;
  Material* get_material();

  void set_material(Material* material)
  {
    mMaterial = material;
  }

protected:
  Material* mMaterial;
  Primitive* mPrimitive;
};

#endif
