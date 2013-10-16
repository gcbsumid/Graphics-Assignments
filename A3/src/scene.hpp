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
  const Matrix4x4& get_translation() const { return mTranslation; }
  const Matrix4x4& get_translation_inverse() const { return mTranslationInverse; }
  const Matrix4x4& get_rotation() const { return mRotation; }
  const Matrix4x4& get_rotation_inverse() const { return mRotationInverse; }
  
  void set_transform(const Matrix4x4& m)
  {
    mTrans = m;
    mInvTrans = m.invert();
  }

  void recalculate_transform() {
    mTrans = mTranslation * mScale * mRotation;
    mInvTrans = mTrans.invert();        
  }

  void set_rotation(const Matrix4x4 rotate) {
    mRotation = rotate;
    mRotationInverse = rotate.invert();

    recalculate_transform();
  }

  void set_translation(const Matrix4x4 trans) {
    mTranslation = trans;
    mTranslationInverse = trans.invert();

    recalculate_transform();
  }

  void reset_rotation() {
    mRotation = Matrix4x4();
    mRotationInverse = Matrix4x4();
  }

  void reset_translation() {
    mTranslation = Matrix4x4();
    mTranslationInverse = Matrix4x4();    
  }

  void set_transform(const Matrix4x4& m, const Matrix4x4& i)
  {
    mTrans = m;
    mInvTrans = i;
  }

  void add_child(SceneNode* child)
  {
    mChildren.push_back(child);
    child->setParent(this);
  }

  void remove_child(SceneNode* child)
  {
    mChildren.remove(child);
  }

  virtual bool select(int id);

  // Callbacks to be implemented.
  // These will be called from Lua.
  virtual void rotate(char axis, double angle);
  virtual void scale(const Vector3D& amount);
  virtual void translate(const Vector3D& amount);
  virtual void selection_rotate(char axis, double angle);
  virtual void reset_joints();
  virtual void unselect_all();

  void setParent(SceneNode* node) {
    mParent = node;
  }

  // Returns true if and only if this node is a JointNode
  virtual bool is_joint() const;
  void apply_select_to_children(bool isSelect);
  
protected:


  static int nextID;
  // Useful for picking
  int mId;
  std::string mName;

  // Transformations
  Matrix4x4 mTrans;
  Matrix4x4 mInvTrans;

  Matrix4x4 mRotation;
  Matrix4x4 mTranslation;
  Matrix4x4 mScale;
  Matrix4x4 mRotationInverse;
  Matrix4x4 mTranslationInverse;

  // Hierarchy
  SceneNode* mParent;

  bool mSelected;
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

  virtual void selection_rotate(char axis, double angle);
  virtual void reset_joints();

  struct JointRange {
    double min, init, max;
  };

  
protected:
  double mAngleX; 
  double mAngleY;
  JointRange mJointX, mJointY;
};

class GeometryNode : public SceneNode {
public:
  GeometryNode(const std::string& name,
               Primitive* primitive);
  virtual ~GeometryNode();

  virtual void walk_gl(bool picking = false) const;
  virtual void scale(const Vector3D& amount);
  virtual bool select(int id);

  const Material* get_material() const;
  Material* get_material();

  void set_material(Material* material)
  {
    mMaterial = material;
  }

protected:
  Matrix4x4 mScale;
  Matrix4x4 mInvScale;
  Material* mMaterial;
  Primitive* mPrimitive;
};

#endif
