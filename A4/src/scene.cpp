#include "scene.hpp"
#include <iostream>

SceneNode::SceneNode(const std::string& name)
  : mName(name)
{
}

SceneNode::~SceneNode()
{
}

IntersectObj* SceneNode::intersect(Ray ray) {

  ray.mOrigin = mInvScale * mInvRotate * mInvTranslate * ray.mOrigin;
  ray.mDirection = mInvScale * mInvRotate * ray.mDirection;


  IntersectObj* minimum = NULL;
  for(auto& child : mChildren) {
    IntersectObj* intersectedObj = child->intersect(ray);
    if (intersectedObj) {
      Vector3D r = intersectedObj->mPoint - ray.mOrigin;
      intersectedObj->mDistance = r.length();
    }

    if (intersectedObj == NULL) {
      continue;
    } else if (minimum == NULL) {
      minimum = intersectedObj;
    } else if (minimum->mDistance > intersectedObj->mDistance) {
      delete minimum;
      minimum = intersectedObj;
    } else {
      delete intersectedObj;
    }
  } 

  if (minimum != NULL) {
    minimum->mPoint = mTranslate * mRotate * mScale * minimum->mPoint;
    minimum->mNormal = mRotate * mInvScale * minimum->mNormal;
  }

  return minimum;
}

bool SceneNode::isInShadow(Ray ray, SceneNode* node) const {
  ray.mOrigin = mInvScale * mInvRotate * mInvTranslate * ray.mOrigin;
  ray.mDirection = mInvScale * mInvRotate * ray.mDirection;
  for (auto& child : mChildren) {
    if (child->isInShadow(ray, node)) {
      return true;
    }
  }
  return false;
}


void SceneNode::rotate(char axis, double angle)
{
  Matrix4x4 r;
  angle = (angle/180 * M_PI);

  switch (axis) {
    case 'x':
      r[1][1] = std::cos(angle);
      r[1][2] = -std::sin(angle);
      r[2][1] = std::sin(angle);
      r[2][2] = std::cos(angle);
      break;
    case 'y':
      r[0][0] = std::cos(angle);
      r[2][0] = -std::sin(angle);
      r[0][2] = std::sin(angle);
      r[2][2] = std::cos(angle);
      break;
    case 'z':
      r[0][0] = std::cos(angle);
      r[0][1] = -std::sin(angle);
      r[1][0] = std::sin(angle);
      r[1][1] = std::cos(angle);
      break;
    default:
      std::cerr << "Error: Invalid Rotation Axis." << std::endl;
      break;
  }

  mRotate = mRotate * r;
  mInvRotate = mRotate.invert();

  mTrans = mTrans * r;
  mInvTrans = mTrans.invert();

  // recalculate_bounding_box();
}

void SceneNode::scale(const Vector3D& amount)
{
  Matrix4x4 s;

  // std::cout << "SceneNode Amount: " << amount << std::endl;
  /*
  * [ x, 0, 0, 0 ] 
  * [ 0, y, 0, 0 ]
  * [ 0, 0, z, 0 ]
  * [ 0, 0, 0, 1 ]
  */

  s[0][0] = 1.0 * amount[0];
  s[1][1] = 1.0 * amount[1];
  s[2][2] = 1.0 * amount[2];

  mScale = mScale * s;
  mInvScale = mScale.invert();

  mTrans = mTrans * s;
  mInvTrans = mInvTrans * s.invert();
  // recalculate_bounding_box();
}

void SceneNode::translate(const Vector3D& amount)
{
  Matrix4x4 t;

  /*
  * [ 1, 0, 0, x ] 
  * [ 0, 1, 0, y ]
  * [ 0, 0, 1, z ]
  * [ 0, 0, 0, 1 ]
  */

  t[0][3] = amount[0];
  t[1][3] = amount[1];
  t[2][3] = amount[2];

  mTranslate = t * mTranslate;
  mInvTranslate = mTranslate.invert();

  mTrans = t * mTrans;
  mInvTrans = mTrans.invert();
  // recalculate_bounding_box();
}

bool SceneNode::is_joint() const
{
  return false;
}

JointNode::JointNode(const std::string& name)
  : SceneNode(name)
{
}

JointNode::~JointNode()
{
}

bool JointNode::is_joint() const
{
  return true;
}

void JointNode::set_joint_x(double min, double init, double max)
{
  m_joint_x.min = min;
  m_joint_x.init = init;
  m_joint_x.max = max;
}

void JointNode::set_joint_y(double min, double init, double max)
{
  m_joint_y.min = min;
  m_joint_y.init = init;
  m_joint_y.max = max;
}

GeometryNode::GeometryNode(const std::string& name, Primitive* primitive)
  : SceneNode(name),
    mPrimitive(primitive)
{
}

GeometryNode::~GeometryNode()
{
}

IntersectObj* GeometryNode::intersect(Ray ray)
{
  ray.mOrigin = mInvScale * mInvRotate * mInvTranslate * ray.mOrigin;
  ray.mDirection = mInvScale * mInvRotate * ray.mDirection;

  IntersectObj* minimum = mPrimitive->intersect(ray);
  if (minimum != NULL) {
    minimum->mNode = this;
    Vector3D r = minimum->mPoint - ray.mOrigin;
    minimum->mDistance = r.length();
  }

  for(auto& child : mChildren) {
    IntersectObj* intersectedObj = child->intersect(ray);
    if (intersectedObj) {
      Vector3D r = intersectedObj->mPoint - ray.mOrigin;
      intersectedObj->mDistance = r.length();
    }

    if (intersectedObj == NULL) {
      continue;
    } else if (minimum == NULL) {
      minimum = intersectedObj;
    } else if (minimum->mDistance > intersectedObj->mDistance) {
      delete minimum;
      minimum = intersectedObj;
    } else {
      delete intersectedObj;
    }
  } 

  if (minimum != NULL) {
    minimum->mPoint = mTranslate * mRotate * mScale * minimum->mPoint;
    // minimum->mNormal = mRotate * mInvScale * minimum->mNormal;
    minimum->mNormal = mRotate * mInvScale * minimum->mNormal;
    // std::cout << "I'm intersecting something!" << std::endl;
  }

  return minimum;
}

bool GeometryNode::isInShadow(Ray ray, SceneNode* node) const {
  ray.mOrigin = mInvScale * mInvRotate * mInvTranslate * ray.mOrigin;
  ray.mDirection = mInvScale * mInvRotate * ray.mDirection;

  // if ((node->get_name() != get_name()) && mPrimitive->isInShadow(ray)) {
  if (mPrimitive->isInShadow(ray)) {
    return true;
  }
  for (auto& child : mChildren) {
    if (child->isInShadow(ray, node)) {
      return true;
    }
  }
  return false;
}

void GeometryNode::scale(const Vector3D& amount)
{
  Matrix4x4 s;

  // std::cout << "SceneNode Amount: " << amount << std::endl;
  /*
  * [ x, 0, 0, 0 ] 
  * [ 0, y, 0, 0 ]
  * [ 0, 0, z, 0 ]
  * [ 0, 0, 0, 1 ]
  */

  s[0][0] = 1.0 * amount[0];
  s[1][1] = 1.0 * amount[1];
  s[2][2] = 1.0 * amount[2];

  mScale = mScale * s;
  mInvScale = mScale.invert();

  mTrans = mTrans * s;
  mInvTrans = mInvTrans * s.invert();
  // recalculate_bounding_box();
}

const Material* GeometryNode::get_material() const {
  return mMaterial;
}

Material* GeometryNode::get_material() {
  return mMaterial;
}