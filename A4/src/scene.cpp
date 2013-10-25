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
  // glPushMatrix();
  // double trans[] = {
  //   mTrans[0][0], mTrans[1][0], mTrans[2][0], mTrans[3][0], 
  //   mTrans[0][1], mTrans[1][1], mTrans[2][1], mTrans[3][1], 
  //   mTrans[0][2], mTrans[1][2], mTrans[2][2], mTrans[3][2], 
  //   mTrans[0][3], mTrans[1][3], mTrans[2][3], mTrans[3][3] 
  // };

  // glMultMatrixd(trans);
  // ray = mTrans * ray;

  IntersectObj* minimum = NULL;
  for(auto& child : mChildren) {
    IntersectObj* intersectedObj = child->intersect(ray);
    if (minimum == NULL) {
      minimum = intersectedObj;
    } else if (minimum->mDistance > intersectedObj->mDistance) {
      delete minimum;
      minimum = intersectedObj;
    } else {
      delete intersectedObj;
    }
  } 

  return minimum;
  // glPopMatrix();
}

bool SceneNode::isInShadow(Ray ray) const {
  for (auto& child : mChildren) {
    if (child->isInShadow(ray)) {
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
  mTrans = mTrans * r;
  mInvTrans = mTrans.invert();
}

void SceneNode::scale(const Vector3D& amount)
{
  Matrix4x4 s;

  /*
  * [ x, 0, 0, 0 ] 
  * [ 0, y, 0, 0 ]
  * [ 0, 0, z, 0 ]
  * [ 0, 0, 0, 1 ]
  */

  s[0][0] = 1.0 * amount[0];
  s[1][1] = 1.0 * amount[1];
  s[2][2] = 1.0 * amount[2];

  mTrans = mTrans * s;
  mInvTrans = mInvTrans * s.invert();
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

  mTrans = t * mTrans;
  mInvTrans = mTrans.invert();
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

IntersectObj* JointNode::intersect(Ray ray) {
  // glPushMatrix();

  // Matrix4x4 x_rotate, y_rotate, temp;
  // double x_angle = (mAngleX/180 * M_PI);
  // double y_angle = (mAngleY/180 * M_PI);

  // x_rotate[1][1] = std::cos(x_angle);
  // x_rotate[1][2] = -std::sin(x_angle);
  // x_rotate[2][1] = std::sin(x_angle);
  // x_rotate[2][2] = std::cos(x_angle);

  // y_rotate[0][0] = std::cos(y_angle);
  // y_rotate[2][0] = -std::sin(y_angle);
  // y_rotate[0][2] = std::sin(y_angle);
  // y_rotate[2][2] = std::cos(y_angle);

  // temp = mTrans * x_rotate * y_rotate;

  // double trans[] = {
  //   temp[0][0], temp[1][0], temp[2][0], temp[3][0], 
  //   temp[0][1], temp[1][1], temp[2][1], temp[3][1], 
  //   temp[0][2], temp[1][2], temp[2][2], temp[3][2], 
  //   temp[0][3], temp[1][3], temp[2][3], temp[3][3] 
  // };
  // glMultMatrixd(trans);

  IntersectObj* minimum = NULL;
  for(auto& child : mChildren) {
    IntersectObj* intersectedObj = child->intersect(ray);
    if (minimum == NULL) {
      minimum = intersectedObj;
    } else if (minimum->mDistance > intersectedObj->mDistance) {
      delete minimum;
      minimum = intersectedObj;
    } else {
      delete intersectedObj;
    }
  } 

  return minimum;

  // glPopMatrix();
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

  // TODO
  IntersectObj* minimum = mPrimitive->intersect(ray);
  if (minimum != NULL) {
    minimum->mNode = this;
  }

  for(auto& child : mChildren) {
    IntersectObj* intersectedObj = child->intersect(ray);
    if (minimum == NULL) {
      minimum = intersectedObj;
    } else if (minimum->mDistance > intersectedObj->mDistance) {
      delete minimum;
      minimum = intersectedObj;
    } else {
      delete intersectedObj;
    }
  } 

  return minimum;
}

bool GeometryNode::isInShadow(Ray ray) const {
  if (mPrimitive->isInShadow(ray)) {
    return true;
  }
  for (auto& child : mChildren) {
    if (child->isInShadow(ray)) {
      return true;
    }
  }
  return false;
}

void GeometryNode::scale(const Vector3D& amount)
{
    /*
    * [ x, 0, 0, 0 ] 
    * [ 0, y, 0, 0 ]
    * [ 0, 0, z, 0 ]
    * [ 0, 0, 0, 1 ]
    */

    mScale[0][0] = 1.0 * amount[0];
    mScale[1][1] = 1.0 * amount[1];
    mScale[2][2] = 1.0 * amount[2];
}

const Material* GeometryNode::get_material() const {
  return mMaterial;
}

Material* GeometryNode::get_material() {
  return mMaterial;
}