#include "scene.hpp"
#include <iostream>
#include <GL/gl.h>
#include <GL/glu.h>

SceneNode::SceneNode(const std::string& name)
  : mName(name)
{
  std::cout << "Scene Node " << name  << " is created." << std::endl;
}

SceneNode::~SceneNode()
{
}

void SceneNode::walk_gl(bool picking) const
{
  // TODO: Picking
  glPushMatrix();
  // glLoadIdentity();
  double trans[] = {
    mTrans[0][0], mTrans[1][0], mTrans[2][0], mTrans[3][0], 
    mTrans[0][1], mTrans[1][1], mTrans[2][1], mTrans[3][1], 
    mTrans[0][2], mTrans[1][2], mTrans[2][2], mTrans[3][2], 
    mTrans[0][3], mTrans[1][3], mTrans[2][3], mTrans[3][3] 
  };
  
  // std::cout << "Matrix in scene node: \n";
  // std::cout << "{ " << mTrans[0][0] << ", " << mTrans[1][0] << ", " << mTrans[2][0] << ", " << mTrans[3][0] << "}\n";
  // std::cout << "{ " << mTrans[0][1] << ", " << mTrans[1][1] << ", " << mTrans[2][1] << ", " << mTrans[3][1] << "}\n";
  // std::cout << "{ " << mTrans[0][2] << ", " << mTrans[1][2] << ", " << mTrans[2][2] << ", " << mTrans[3][2] << "}\n";
  // std::cout << "{ " << mTrans[0][3] << ", " << mTrans[1][3] << ", " << mTrans[2][3] << ", " << mTrans[3][3] << "}\n";

  glMultMatrixd(trans);

  for(auto& child : mChildren) {
    child->walk_gl();
  } 

  glPopMatrix();
}

void SceneNode::rotate(char axis, double angle)
{
  // std::cerr << "Stub: Rotate " << mName << " around " << axis << " by " << angle << std::endl;
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
  mInvTrans = mInvTrans * r.invert();
}

void SceneNode::scale(const Vector3D& amount)
{
  // std::cerr << "Stub: Scale " << mName << " by " << amount << std::endl;
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
  // std::cerr << "Stub: Translate " << mName << " by " << amount << std::endl;
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

    mTrans = mTrans * t;
    mInvTrans = mInvTrans * t.invert();
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

void JointNode::walk_gl(bool picking) const
{
  glPushMatrix();
  // glLoadIdentity();

  double trans[] = {
    mTrans[0][0], mTrans[1][0], mTrans[2][0], mTrans[3][0], 
    mTrans[0][1], mTrans[1][1], mTrans[2][1], mTrans[3][1], 
    mTrans[0][2], mTrans[1][2], mTrans[2][2], mTrans[3][2], 
    mTrans[0][3], mTrans[1][3], mTrans[2][3], mTrans[3][3] 
  };

  // std::cout << "Matrix in Joint node: \n";
  // std::cout << "{ " << mTrans[0][0] << ", " << mTrans[1][0] << ", " << mTrans[2][0] << ", " << mTrans[3][0] << "}\n";
  // std::cout << "{ " << mTrans[0][1] << ", " << mTrans[1][1] << ", " << mTrans[2][1] << ", " << mTrans[3][1] << "}\n";
  // std::cout << "{ " << mTrans[0][2] << ", " << mTrans[1][2] << ", " << mTrans[2][2] << ", " << mTrans[3][2] << "}\n";
  // std::cout << "{ " << mTrans[0][3] << ", " << mTrans[1][3] << ", " << mTrans[2][3] << ", " << mTrans[3][3] << "}\n";

  glMultMatrixd(trans);

  // TODO: do something about the fact that these are for joints
  for(auto& child : mChildren) {
    child->walk_gl();
  } 

  glPopMatrix();
}

bool JointNode::is_joint() const
{
  return true;
}

void JointNode::set_joint_x(double min, double init, double max)
{
  mJointX.min = min;
  mJointX.init = init;
  mJointX.max = max;
}

void JointNode::set_joint_y(double min, double init, double max)
{
  mJointY.min = min;
  mJointY.init = init;
  mJointY.max = max;
}

GeometryNode::GeometryNode(const std::string& name, Primitive* primitive)
  : SceneNode(name),
    mPrimitive(primitive)
{
}

GeometryNode::~GeometryNode()
{
}

void GeometryNode::walk_gl(bool picking) const
{
  glPushMatrix();
  // std::cout <<"Drawing " << mName << std::endl;
  // glLoadIdentity();
  // glTranslated(0.0, 0.0, -2.0);

  double trans[] = {
    mTrans[0][0], mTrans[1][0], mTrans[2][0], mTrans[3][0], 
    mTrans[0][1], mTrans[1][1], mTrans[2][1], mTrans[3][1], 
    mTrans[0][2], mTrans[1][2], mTrans[2][2], mTrans[3][2], 
    mTrans[0][3], mTrans[1][3], mTrans[2][3], mTrans[3][3] 
  };
  double scale[] = {
    mScale[0][0], mScale[1][0], mScale[2][0], mScale[3][0], 
    mScale[0][1], mScale[1][1], mScale[2][1], mScale[3][1], 
    mScale[0][2], mScale[1][2], mScale[2][2], mScale[3][2], 
    mScale[0][3], mScale[1][3], mScale[2][3], mScale[3][3] 
  };


  // std::cout << "Matrix in GeometryNode: \n";
  // std::cout << "{ " << mTrans[0][0] << ", " << mTrans[1][0] << ", " << mTrans[2][0] << ", " << mTrans[3][0] << "}\n";
  // std::cout << "{ " << mTrans[0][1] << ", " << mTrans[1][1] << ", " << mTrans[2][1] << ", " << mTrans[3][1] << "}\n";
  // std::cout << "{ " << mTrans[0][2] << ", " << mTrans[1][2] << ", " << mTrans[2][2] << ", " << mTrans[3][2] << "}\n";
  // std::cout << "{ " << mTrans[0][3] << ", " << mTrans[1][3] << ", " << mTrans[2][3] << ", " << mTrans[3][3] << "}\n";

  glMultMatrixd(trans);
  
  glPushMatrix();
  glMultMatrixd(scale);
  mMaterial->apply_gl();
  mPrimitive->walk_gl(picking);
  glPopMatrix();

  for(auto& child : mChildren) {
    child->walk_gl();
  } 

  glPopMatrix();
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

    mInvScale = mScale.invert();
}
