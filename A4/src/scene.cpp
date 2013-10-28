#include "scene.hpp"
#include <iostream>

SceneNode::SceneNode(const std::string& name)
  : mName(name)
{
}

SceneNode::~SceneNode()
{
}

void SceneNode::DisplayBoundingBox() {
  std::cout << mName << " bounding box: " << std::endl;
  std::cout << "\tMin: " << mBoundingBox.mMin << std::endl;
  std::cout << "\tMax: " << mBoundingBox.mMax << std::endl;

  for (auto& child : mChildren) {
    child->DisplayBoundingBox();
  }
}

IntersectObj* SceneNode::intersect(Ray ray) {

  // if (!check_bounding_box(ray)) {
  //   return NULL;
  // }

  ray.mOrigin = mInvScale * mInvRotate * mInvTranslate * ray.mOrigin;
  ray.mDirection = mInvScale * mInvRotate * ray.mDirection;
  // ray = mTrans * ray;
  // TODO: fix bounding boxes


  IntersectObj* minimum = NULL;
  for(auto& child : mChildren) {
    IntersectObj* intersectedObj = child->intersect(ray);
    if (intersectedObj) {
      Vector3D r = intersectedObj->mPoint - ray.mOrigin;
      intersectedObj->mDistance = r.length();
      // std::cout << intersectedObj->mNode->get_name() << " distance: " 
      //           << intersectedObj->mPoint << " to " << ray.mOrigin 
      //           << " is " << intersectedObj->mDistance << std::endl;
    }

    // if (minimum && intersectedObj) {
    //   std::cout << minimum->mNode->get_name() << " " << minimum->mDistance << " > "
    //             << intersectedObj->mNode->get_name() << " " << intersectedObj->mDistance << std::endl;
    // }

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
    // minimum->mNormal = mRotate * minimum->mNormal;
    // std::cout << "I'm progressing." << std::endl;
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

AABB SceneNode::recalculate_bounding_box() {
  // TODO
  AABB box;

  // Get the AABB for all objects encompassed by this
  for (auto& child : mChildren) {
    AABB childAABB = child->recalculate_bounding_box(); 

    compare_bounding_boxes(box, childAABB);
  }
  std::cout << mName << " box: " << std::endl;
  std::cout << "\tMin: " << box.mMin << std::endl;
  std::cout << "\tMax: " << box.mMax << std::endl;

  // Once we have the AABB, transform the points
  box.mMin = mTranslate * mRotate * mScale * box.mMin;
  box.mMax = mTranslate * mRotate * mScale * box.mMax;


  for (int i = 0; i < 3; i++) {
    if (box.mMin[i] > box.mMax[i]) {
      double temp = box.mMin[i];
      box.mMin[i] = box.mMax[i];
      box.mMax[i] = temp;
    } 
  }
  std::cout << "after translation Bounding box: "<< std::endl;
  std::cout << "\tMin: " << box.mMin << std::endl;
  std::cout << "\tMax: " << box.mMax << std::endl;

  // Now Set the AABB of mBoundingBox
  compare_bounding_boxes(mBoundingBox, box);
  std::cout << "Bounding box: "<< std::endl;
  std::cout << "\tMin: " << mBoundingBox.mMin << std::endl;
  std::cout << "\tMax: " << mBoundingBox.mMax << std::endl;
  return mBoundingBox;
}

void SceneNode::compare_bounding_boxes(AABB& initial, AABB other) {
  if (other.mMin[0] < initial.mMin[0]) {
    initial.mMin[0] = other.mMin[0];
  } 
  
  if (other.mMax[0] > initial.mMax[0]) {
    initial.mMax[0] = other.mMax[0];
  }

  if (other.mMin[1] < initial.mMin[1]) {
    initial.mMin[1] = other.mMin[1];
  }

  if (other.mMax[1] > initial.mMax[1]) {
    initial.mMax[1] = other.mMax[1];
  }

  if (other.mMin[2] < initial.mMin[2]) {
    initial.mMin[2] = other.mMin[2];
  }

  if (other.mMax[2] > initial.mMax[2]) {
    initial.mMax[2] = other.mMax[2];
  }
}

bool SceneNode::check_bounding_box(Ray ray) {

  if (ray.mOrigin[0] >= mBoundingBox.mMin[0] && 
      ray.mOrigin[0] <= mBoundingBox.mMax[0] &&
      ray.mOrigin[1] >= mBoundingBox.mMin[1] && 
      ray.mOrigin[1] <= mBoundingBox.mMax[1] &&
      ray.mOrigin[2] >= mBoundingBox.mMin[2] && 
      ray.mOrigin[2] <= mBoundingBox.mMax[2]) {
    return true;
  }

  Vector3D inverse_ray_direction = Vector3D(1/ray.mDirection[0], 1/ray.mDirection[1], 1/ray.mDirection[2]);

  double tmin, tmax, tymin, tymax, tzmin, tzmax;
  if (inverse_ray_direction[0] < 0) {
    tmin = (mBoundingBox.mMax[0] - ray.mOrigin[0]) * inverse_ray_direction[0];
    tmax = (mBoundingBox.mMin[0] - ray.mOrigin[0]) * inverse_ray_direction[0];
    tymin = (mBoundingBox.mMax[1] - ray.mOrigin[1]) * inverse_ray_direction[1];
    tymax = (mBoundingBox.mMin[1] - ray.mOrigin[1]) * inverse_ray_direction[1];
  } else {
    tmin = (mBoundingBox.mMin[0] - ray.mOrigin[0]) * inverse_ray_direction[0];
    tmax = (mBoundingBox.mMax[0] - ray.mOrigin[0]) * inverse_ray_direction[0];
    tymin = (mBoundingBox.mMax[1] - ray.mOrigin[1]) * inverse_ray_direction[1];
    tymax = (mBoundingBox.mMin[1] - ray.mOrigin[1]) * inverse_ray_direction[1];
  }

  // std::cout << "tmin: " << tmin << std::endl;
  // std::cout << "tymax: " << tymax << std::endl;
  // std::cout << "tmax: " << tmax << std::endl;
  // std::cout << "tymin: " << tymin << std::endl;

  if ((tmin > tymax) || (tymin > tmax)) {
    return false;
  } 


  if (tymin > tmin) {
    tmin = tymin;
  }

  if (tymax < tmax) {
    tmax = tymax;
  }

  if (inverse_ray_direction[0] < 0) {
    tzmin = (mBoundingBox.mMax[2] - ray.mOrigin[2]) * inverse_ray_direction[2];
    tzmax = (mBoundingBox.mMin[2] - ray.mOrigin[2]) * inverse_ray_direction[2];
  } else {
    tzmin = (mBoundingBox.mMin[2] - ray.mOrigin[2]) * inverse_ray_direction[2];
    tzmax = (mBoundingBox.mMax[2] - ray.mOrigin[2]) * inverse_ray_direction[2];
  }
  if ( (tmin > tzmax) || (tzmin > tmax) ) {
    return false;
  }

  if (tzmin > tmin)
    tmin = tzmin;
  if (tzmax < tmax)
    tmax = tzmax;

  return true;











  // double t_min = -100000.0;
  // double t_max = 100000.0;

  // Point3D position = Point3D(
  //                     (mBoundingBox.mMin[0] + mBoundingBox.mMax[0])/2.0,
  //                     (mBoundingBox.mMin[1] + mBoundingBox.mMax[1])/2.0,
  //                     (mBoundingBox.mMin[2] + mBoundingBox.mMax[2])/2.0
  //                     );

  // // std::cout << "position: " << position << std::endl;
  // Vector3D delta = position - ray.mOrigin;
  // // std::cout << "delta: " << delta << std::endl;

  // // Compute the intersections with 2 planes that delimit the x Axis
  // Vector3D xAxis = Vector3D(1.0, 0, 0);
  // Vector3D yAxis = Vector3D(0, 1.0, 0);
  // Vector3D zAxis = Vector3D(0, 0, 1.0);
  // double e_x = xAxis.dot(delta);
  // double f_x = ray.mDirection.dot(xAxis);
  // double e_y = yAxis.dot(delta);
  // double f_y = ray.mDirection.dot(yAxis);
  // double e_z = zAxis.dot(delta);
  // double f_z = ray.mDirection.dot(zAxis);
  
  // double t1_x = (e_x + mBoundingBox.mMin[0])/f_x; // intersection with left plane
  // double t2_x = (e_x + mBoundingBox.mMax[0])/f_x; // intersection with right plane
  // double t1_y = (e_y + mBoundingBox.mMin[1])/f_y; // intersection with top plane
  // double t2_y = (e_y + mBoundingBox.mMax[1])/f_y; // intersection with bottom plane
  // double t1_z = (e_z + mBoundingBox.mMin[2])/f_z; // intersection with front plane
  // double t2_z = (e_z + mBoundingBox.mMax[2])/f_z; // intersection with back plane

  // // std::cout << "t1_x: " << t1_x << std::endl; // intersection with left plane
  // // std::cout << "t2_x: " << t2_x << std::endl; // intersection with right plane
  // // std::cout << "t1_y: " << t1_y << std::endl; // intersection with top plane
  // // std::cout << "t2_y: " << t2_y << std::endl; // intersection with bottom plane
  // // std::cout << "t1_z: " << t1_z << std::endl; // intersection with front plane
  // // std::cout << "t2_z: " << t2_z << std::endl; // intersection with back plane
  // // X axis
  // if (t1_x > t2_x) {
  //   double temp = t1_x;
  //   t1_x = t2_x;
  //   t2_x = temp;
  // }

  // if (t2_x < t_max) {
  //   t_max = t2_x;
  // }

  // if (t1_x > t_min) { 
  //   t_min = t1_x;
  // }

  // // Y axis
  // if (t1_y > t2_y) {
  //   double temp = t1_y;
  //   t1_y = t2_y;
  //   t2_y = temp;
  // }

  // if (t2_y < t_max) {
  //   t_max = t2_y;
  // }

  // if (t1_y > t_min) { 
  //   t_min = t1_y;
  // }

  // // Z Axis
  // if (t1_z > t2_z) {
  //   double temp = t1_z;
  //   t1_z = t2_z;
  //   t2_z = temp;
  // }

  // if (t2_z < t_max) {
  //   t_max = t2_z;
  // }

  // if (t1_z > t_min) { 
  //   t_min = t1_z;
  // }

  // // std::cout << "t_max: " << t_max << " t_min: " << t_min << std::endl;
  // if (t_max < t_min) {
  //   return false;
  // }



  // return true;
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

  // if (!check_bounding_box(ray)) {
  //   return NULL;
  // }

  // TODO
  // std::cout << "Invert transform: \n" << mInvTrans << std::endl;
  // std::cout << "Invert scale: \n" << mInvScale << std::endl;
  // std::cout << "before point: " << ray.mOrigin << std::endl;
  // std::cout << "before dir: " << ray.mDirection << std::endl;
  ray.mOrigin = mInvScale * mInvRotate * mInvTranslate * ray.mOrigin;
  ray.mDirection = mInvScale * mInvRotate * ray.mDirection;
  // std::cout << "after point: " << ray.mOrigin << std::endl;
  // std::cout << "after dir: " << ray.mDirection << std::endl;

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


AABB GeometryNode::recalculate_bounding_box() {
  // TODO
  AABB box = mPrimitive->getBoundingBox();

  // Get the AABB for all objects encompassed by this
  for (auto& child : mChildren) {
    AABB childAABB = child->recalculate_bounding_box(); 

    compare_bounding_boxes(box, childAABB);
  }

  // Once we have the AABB, transform the points
  box.mMin = mTranslate * mRotate * mScale * box.mMin;
  box.mMax = mTranslate * mRotate * mScale * box.mMax;

  for (int i = 0; i < 3; i++) {
    if (box.mMin[i] > box.mMax[i]) {
      double temp = box.mMin[i];
      box.mMin[i] = box.mMax[i];
      box.mMax[i] = temp;
    } 
  }

  // Now Set the AABB of mBoundingBox
  compare_bounding_boxes(mBoundingBox, box);
  return mBoundingBox;
}

const Material* GeometryNode::get_material() const {
  return mMaterial;
}

Material* GeometryNode::get_material() {
  return mMaterial;
}