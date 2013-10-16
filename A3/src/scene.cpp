#include "scene.hpp"
#include <iostream>
#include <GL/gl.h>
#include <GL/glu.h>

int SceneNode::nextID = 0;

SceneNode::SceneNode(const std::string& name)
  : mName(name)
  , mSelected(false)
{
  mId = ++nextID;
  // std::cout << "Scene Node " << name  << " has id " << mId << std::endl;
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
    child->walk_gl(picking);
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
  mRotation = mRotation * r;
  mRotationInverse = mRotation.invert();
  mTrans = mTrans * r;
  mInvTrans = mTrans.invert();
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

  mScale = mScale * s;
  // mScaleInverse = mScale.invert();
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

  mTranslation = mTranslation * t;
  mTranslationInverse = mTranslation.invert();  
  mTrans = mTrans * t;
  mInvTrans = mInvTrans * t.invert();
}

bool SceneNode::is_joint() const
{
  return false;
}

bool SceneNode::select(int id) {
  for (auto& child : mChildren) {
    child->select(id);
  }
  return false;
}

void SceneNode::selection_rotate(char axis, double angle) {
  for (auto& child : mChildren) {
    child->selection_rotate(axis, angle);
  }
}

void SceneNode::apply_select_to_children(bool isSelect) {
  mSelected = isSelect;
  for (auto& child : mChildren) {
    child->apply_select_to_children(isSelect);
  }
  // if (isSelect) {
  //   mSelected = isSelect;
  // } else if (mSelected) {
  //   mSelected = isSelect;
  // }
}

void SceneNode::reset_joints() {
  for (auto& child : mChildren) {
    child->reset_joints();
  }
}

void SceneNode::unselect_all() {
  mSelected = false;
  for (auto& child : mChildren) {
    child->unselect_all();
  }
}

JointNode::JointNode(const std::string& name)
  : SceneNode(name)
  , mAngleX(0.0)
  , mAngleY(0.0)
{
}

JointNode::~JointNode()
{
}

void JointNode::walk_gl(bool picking) const
{
  glPushMatrix();
  // glLoadIdentity();

  Matrix4x4 x_rotate, y_rotate, temp;
  double x_angle = (mAngleX/180 * M_PI);
  double y_angle = (mAngleY/180 * M_PI);

  x_rotate[1][1] = std::cos(x_angle);
  x_rotate[1][2] = -std::sin(x_angle);
  x_rotate[2][1] = std::sin(x_angle);
  x_rotate[2][2] = std::cos(x_angle);

  y_rotate[0][0] = std::cos(y_angle);
  y_rotate[2][0] = -std::sin(y_angle);
  y_rotate[0][2] = std::sin(y_angle);
  y_rotate[2][2] = std::cos(y_angle);

  temp = mTrans * x_rotate * y_rotate;

  double trans[] = {
    temp[0][0], temp[1][0], temp[2][0], temp[3][0], 
    temp[0][1], temp[1][1], temp[2][1], temp[3][1], 
    temp[0][2], temp[1][2], temp[2][2], temp[3][2], 
    temp[0][3], temp[1][3], temp[2][3], temp[3][3] 
  };

  // std::cout << "Matrix in Joint node: \n";
  // std::cout << "{ " << mTrans[0][0] << ", " << mTrans[1][0] << ", " << mTrans[2][0] << ", " << mTrans[3][0] << "}\n";
  // std::cout << "{ " << mTrans[0][1] << ", " << mTrans[1][1] << ", " << mTrans[2][1] << ", " << mTrans[3][1] << "}\n";
  // std::cout << "{ " << mTrans[0][2] << ", " << mTrans[1][2] << ", " << mTrans[2][2] << ", " << mTrans[3][2] << "}\n";
  // std::cout << "{ " << mTrans[0][3] << ", " << mTrans[1][3] << ", " << mTrans[2][3] << ", " << mTrans[3][3] << "}\n";

  glMultMatrixd(trans);

  // TODO: do something about the fact that these are for joints
  for(auto& child : mChildren) {
    child->walk_gl(picking);
  } 

  glPopMatrix();
}

void JointNode::reset_joints() {
  mAngleX = mJointX.init;
  mAngleY = mJointY.init;

  for (auto& child : mChildren) {
    child->reset_joints();
  }
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
  mAngleX = init;
}

void JointNode::set_joint_y(double min, double init, double max)
{
  mJointY.min = min;
  mJointY.init = init;
  mJointY.max = max;
  mAngleY = init;
}

void JointNode::selection_rotate(char axis, double angle) {
  if (mSelected) {
    if (axis == 'x') {
      if (mAngleX < mJointX.min || mAngleX > mJointX.max){
        return;
      }

      if (mAngleX + angle < mJointX.min) {
        // double diff = mAngleX - mJointX.min;
        mAngleX = mJointX.min;
        // rotate('x', -diff);
      } else if (mAngleX + angle > mJointX.max) {
        // double diff = mJointX.max - mAngleX;
        mAngleX = mJointX.max;
        // rotate('x', diff);
      } else {
        mAngleX += angle;
        // rotate('x', angle);
      }
    } else if (axis == 'y') {
      if (mAngleY < mJointY.min || mAngleY > mJointY.max){
        return;
      }

      if (mAngleY + angle < mJointY.min) {
        // double diff = mAngleY - mJointY.min;
        mAngleY = mJointY.min;
        // rotate('y', -diff);
      } else if (mAngleY + angle > mJointY.max) {
        // double diff = mJointY.max - mAngleY;
        mAngleY = mJointY.max;
        // rotate('y', diff);
      } else {
        mAngleY += angle;
        // rotate('y', angle);
      }    
    }
  } else {
    for (auto& child : mChildren) {
      child->selection_rotate(axis, angle);
    }
  }

}

GeometryNode::GeometryNode(const std::string& name, Primitive* primitive)
  : SceneNode(name),
    mPrimitive(primitive)
{
}

GeometryNode::~GeometryNode()
{
}

bool GeometryNode::select(int id) {
  // TODO: deal with if it is already selected
  if (mId == id) {
    if (mParent->is_joint()){
      mSelected = !mSelected;
      mParent->apply_select_to_children(mSelected);
      for (auto& child : mChildren) {
        child->apply_select_to_children(mSelected);
      }
    }
  } else {
    for (auto& child : mChildren) {
      child->select(id);
    }
  }
  return false;
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

  if (picking) {
    // std::cout << "Pushing " << mId << " which belongs to " << mName << std::endl;
    glPushName(mId);
  } else {
    mMaterial->apply_gl(mSelected);
  }
  mPrimitive->walk_gl(picking);
  glPopMatrix();

  for(auto& child : mChildren) {
    child->walk_gl(picking);
  } 

  if (picking) {
    // std::cout << "Popping " << mId << " which belongs to " << mName << std::endl;
    glPopName();
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
