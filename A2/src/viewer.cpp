#include <iostream>
#include <cassert>
#include <cmath>
#include <GL/gl.h>
#include <GL/glu.h>
#include "viewer.hpp"
#include "appwindow.hpp"
#include "draw.hpp"
#include "a2.hpp"

std::vector<Point3D> Viewer::Gnomon::mGnomonPoints = { 
    Point3D(0.0, 0.0, 0.0), // 0
    Point3D(0.5, 0.0, 0.0), // X
    Point3D(0.0, 0.5, 0.0), // Y
    Point3D(0.0, 0.0, 0.5), // Z   
};

std::vector<std::pair<int, int> > Viewer::Gnomon::mGnomonIndices = {
    std::make_pair(0, 1), 
    std::make_pair(0, 2), 
    std::make_pair(0, 3)
};

std::vector<Point3D> Viewer::mModelPoints = { 
    Point3D(-1.0, -1.0, -1.0), // 0
    Point3D(-1.0, -1.0, 1.0), // 1 
    Point3D(-1.0, 1.0, -1.0), // 2
    Point3D(-1.0, 1.0, 1.0), // 3    
    Point3D(1.0, -1.0, -1.0), // 4
    Point3D(1.0, -1.0, 1.0), // 5
    Point3D(1.0, 1.0, -1.0), // 6
    Point3D(1.0, 1.0, 1.0)  // 7
};

std::vector<std::pair<int, int> > Viewer::mModelIndices = {
    std::make_pair(0, 1), 
    std::make_pair(0, 2), 
    std::make_pair(0, 4), 
    std::make_pair(7, 3), 
    std::make_pair(7, 5), 
    std::make_pair(7, 6), 
    std::make_pair(2, 3), 
    std::make_pair(2, 6), 
    std::make_pair(1, 3), 
    std::make_pair(1, 5), 
    std::make_pair(4, 6),
    std::make_pair(4, 5)
};

Viewer::Viewer(AppWindow* parent)
  : mMode(MODE_EYE_TRANSLATION)
  , mModelGnomon(this)
  , mWorldGnomon(this)
  , mButton1Press(false)
  , mButton2Press(false)
  , mButton3Press(false)
  , mFOV(30.0)
  , mAspect(1.0)
  , mNear(0.5)
  , mFar(10.0)
  , mParent(parent)
{
  mViewportDimensions.mWidth = 270;
  mViewportDimensions.mHeight = 270;
  mViewportDimensions.mMin[0] = 15;
  mViewportDimensions.mMin[1] = 15;
  mViewportDimensions.mMax[0] = 285;
  mViewportDimensions.mMax[1] = 285;

  Glib::RefPtr<Gdk::GL::Config> glconfig;

  // Ask for an OpenGL Setup with
  //  - red, green and blue component colour
  //  - a depth buffer to avoid things overlapping wrongly
  //  - double-buffered rendering to avoid tearing/flickering
  glconfig = Gdk::GL::Config::create(Gdk::GL::MODE_RGB |
                                     Gdk::GL::MODE_DEPTH |
                                     Gdk::GL::MODE_DOUBLE);
  if (glconfig == 0) {
    // If we can't get this configuration, die
    std::cerr << "Unable to setup OpenGL Configuration!" << std::endl;
    abort();
  }

  // Accept the configuration
  set_gl_capability(glconfig);

  // Register the fact that we want to receive these events
  add_events(Gdk::BUTTON1_MOTION_MASK    |
             Gdk::BUTTON2_MOTION_MASK    |
             Gdk::BUTTON3_MOTION_MASK    |
             Gdk::BUTTON_PRESS_MASK      | 
             Gdk::BUTTON_RELEASE_MASK    |
             Gdk::VISIBILITY_NOTIFY_MASK);
  set_perspective(mFOV, mAspect, mNear, mFar);
  mView = translation(Vector3D(0.0, 0.0, 2.0));
}

Viewer::~Viewer()
{
  // Nothing to do here right now.
}

void Viewer::invalidate()
{
  // Force a rerender
  Gtk::Allocation allocation = get_allocation();
  get_window()->invalidate_rect( allocation, false);
}

void Viewer::set_perspective(double fov, double aspect,
                             double near, double far)
{
  // Calculate the projection matrix
  double cotangent = 1.0 / std::tan(( 3.14159265*fov / 180.0) /2.0); 
  mProjection[0][0] = cotangent / aspect;
  mProjection[1][1] = cotangent;
  mProjection[2][2] = (near + far) / (far - near); 
  mProjection[2][3] = (-2.0 * far * near) / (far - near);
  mProjection[3][2] = 1.0;
  mProjection[3][3] = 0.0;

  mParent->LabelUpdates(mMode, near, far);
}

void Viewer::reset_view()
{
  mModelTransform = Matrix4x4();
  mModelScale = Matrix4x4();
  mViewport = 
  mView = translation(Vector3D(0.0, 0.0, 2.0)); 
  mNear = 0.5;
  mFar = 10.0;
  mFOV = 30.0;
  mViewportDimensions.mWidth = 270;
  mViewportDimensions.mHeight = 270;
  mViewportDimensions.mMin[0] = 15;
  mViewportDimensions.mMin[1] = 15;
  mViewportDimensions.mMax[0] = 285;
  mViewportDimensions.mMax[1] = 285;
  mAspect = 1.0;

  set_perspective(mFOV, mAspect, mNear, mFar);

  invalidate();
}

void Viewer::on_realize()
{
  // Do some OpenGL setup.
  // First, let the base class do whatever it needs to
  Gtk::GL::DrawingArea::on_realize();
  
  Glib::RefPtr<Gdk::GL::Drawable> gldrawable = get_gl_drawable();
  
  if (!gldrawable)
    return;

  if (!gldrawable->gl_begin(get_gl_context()))
    return;

  gldrawable->gl_end();
}

bool Viewer::on_expose_event(GdkEventExpose* event)
{
  Glib::RefPtr<Gdk::GL::Drawable> gldrawable = get_gl_drawable();

  if (!gldrawable) return false;

  if (!gldrawable->gl_begin(get_gl_context()))
    return false;

  // Calculating the Viewport  
  calculateViewport();

  // // Display all Matrices
  // displayAllMatrices();

  draw_init(get_width(), get_height());


  // Drawing the Model
  std::vector<Point3D> modelPoints;
  for (auto point : mModelPoints) {
      Point3D temp = mView * mModelTransform * mModelScale * point;
      // std::cout << "temp: (" << temp[0] << ", " << temp[1] << ", " << temp[2] << ")" << std::endl;
      modelPoints.push_back(temp);
  }

  // Draw the lines
  set_colour(Colour(0.2, 0.2, 0.2));
  for (auto line : mModelIndices) {
    drawLineWithClipping(modelPoints.at(line.first), modelPoints.at(line.second));
  }

  mModelGnomon.draw_gnomon(mModelTransform);
  mWorldGnomon.draw_gnomon(Matrix4x4());

  /* A few of lines are drawn below to show how it's done. */

  set_colour(Colour(0.1, 0.1, 0.1));

  // Draw the near plane box
  draw_line(mViewportDimensions.mMin, Point2D(mViewportDimensions.mMax[0], mViewportDimensions.mMin[1]));
  draw_line(Point2D(mViewportDimensions.mMax[0], mViewportDimensions.mMin[1]),
            mViewportDimensions.mMax);
  draw_line(mViewportDimensions.mMax,
            Point2D(mViewportDimensions.mMin[0], mViewportDimensions.mMax[1]));
  draw_line(Point2D(mViewportDimensions.mMin[0], mViewportDimensions.mMax[1]), 
            mViewportDimensions.mMin);

  draw_complete();
            
  // Swap the contents of the front and back buffers so we see what we
  // just drew. This should only be done if double buffering is enabled.
  gldrawable->swap_buffers();

  gldrawable->gl_end();

  return true;
}

bool Viewer::on_configure_event(GdkEventConfigure* event)
{
  Glib::RefPtr<Gdk::GL::Drawable> gldrawable = get_gl_drawable();

  if (!gldrawable) return false;
  
  if (!gldrawable->gl_begin(get_gl_context()))
    return false;

  gldrawable->gl_end();

  return true;
}

bool Viewer::on_button_press_event(GdkEventButton* event)
{
  // std::cerr << "Stub: Button " << event->button << " pressed" << std::endl;
  mLastMousePos = Point2D(event->x, event->y);
  mCurrentMousePos = Point2D(event->x, event->y);

  if (mMode == MODE_VIEWPORT) {
    mStartPoint = mCurrentMousePos;
    return true;
  }

  if (event->button == 1) 
    mButton1Press = true;
  else if (event->button == 2) 
    mButton2Press = true;
  else if (event->button == 3)
    mButton3Press = true;

  return true;
}

void Viewer::clamp(double &val, const double min, const double max) {
  assert(min < max);

  if (val > max)
    val = max;
  if (val < min)
    val = min;
}

double Viewer::min(double val1, double val2) {
  return (val1 > val2) ? val2 : val1;
}

double Viewer::max(double val1, double val2) {
  return (val1 < val2) ? val2 : val1;
}

bool Viewer::on_button_release_event(GdkEventButton* event)
{
  // std::cerr << "Stub: Button " << event->button << " released" << std::endl;
  mCurrentMousePos = Point2D(event->x, event->y);

  if (mMode == MODE_VIEWPORT) {
    mEndPoint = mCurrentMousePos;

    clamp(mStartPoint[0], 0.0, get_width());
    clamp(mEndPoint[0], 0.0, get_width());
    clamp(mStartPoint[1], 0.0, get_height());
    clamp(mEndPoint[1], 0.0, get_height());

    if (mEndPoint[0] - mStartPoint[0] == 0 
      || mEndPoint[1] - mStartPoint[1] == 0) {
      return true;
    }

    mViewportDimensions.mMin = Point2D(min(mStartPoint[0], mEndPoint[0]), 
                                       min(mStartPoint[1], mEndPoint[1]));
    mViewportDimensions.mMax = Point2D(max(mStartPoint[0], mEndPoint[0]), 
                                       max(mStartPoint[1], mEndPoint[1]));

    mViewportDimensions.mWidth = mViewportDimensions.mMax[0] - mViewportDimensions.mMin[0];
    mViewportDimensions.mHeight = mViewportDimensions.mMax[1] - mViewportDimensions.mMin[1];

    mAspect = mViewportDimensions.mWidth / mViewportDimensions.mHeight;
    set_perspective(mFOV, mAspect, mNear, mFar);

    invalidate();

    return true;
  }

  if (event->button == 1) 
    mButton1Press = false;
  else if (event->button == 2) 
    mButton2Press = false;
  else if (event->button == 3)
    mButton3Press = false;

  return true;
}

bool Viewer::on_motion_notify_event(GdkEventMotion* event)
{
  if (mMode == MODE_VIEWPORT) 
    return true;

  // std::cerr << "Stub: Motion at " << event->x << ", " << event->y << std::endl;
  mLastMousePos = mCurrentMousePos;
  mCurrentMousePos = Point2D(event->x, event->y);
  double diffx = (mCurrentMousePos[0] - mLastMousePos[0])/300.0;
  // std::cout<<"diffx: " << diffx << std::endl;
  Matrix4x4 translate, rotate, scale;

  if (mButton1Press) {
    translate = translation(Vector3D(diffx, 0.0, 0.0));
    rotate = rotation(diffx, 'x');
    scale = scaling(Vector3D(diffx, 0.0, 0.0));
  }

  if (mButton2Press) {
    translate = translation(Vector3D(0.0, diffx, 0.0)) * translate;
    rotate = rotation(diffx, 'y') * rotate;
    scale = scaling(Vector3D(0.0, diffx, 0.0)) * scale;
  }

  if (mButton3Press) {
    translate = translation(Vector3D(0.0, 0.0, diffx)) * translate;
    rotate = rotation(diffx, 'z') * rotate;
    scale = scaling(Vector3D(0.0, 0.0, diffx)) * scale;
  }

  Matrix4x4 mat;
  switch (mMode) {
    case MODE_EYE_ROTATION:
      mView = rotate * mView;
      break;
    case MODE_EYE_TRANSLATION:
      mView = translate * mView;
      break;
    case MODE_EYE_PERSPECTIVE:
      if (mButton1Press) {
        if (diffx > 0) 
          mFOV +=3;
        else if (diffx < 0) 
          mFOV -=3;

        if (mFOV > 160.0) 
          mFOV = 160.0;

        if (mFOV < 5.0) 
          mFOV = 5.0;
      }

      if (mButton2Press) {
        if (diffx > 0) 
          mNear += 0.05;
        else if (diffx < 0) 
          mNear -= 0.05;

        if (mNear < 0.1) 
          mNear = 0.1;

        if (mNear > mFar - 0.5) 
          mNear = mFar - 0.5;
      }

      if (mButton3Press) {
        if (diffx > 0) 
          mFar -= 0.1;
        else if (diffx < 0) 
          mFar += 0.1;

        if (mFar < mNear + 0.5) 
          mFar = mNear + 0.5;

        if (mFar > 15) 
          mFar = 15;  
      }

      // std::cout << "Field of View: " << mFOV << std::endl;
      // std::cout << "mAspect: " << mAspect << std::endl;
      // std::cout << "mNear: " << mNear << std::endl;
      // std::cout << "mFar: " << mFar << std::endl;
      set_perspective(mFOV, mAspect, mNear, mFar);
      break;
    case MODE_MODEL_ROTATION:
      // Apply Rotate to model before the current Model transform to get local rotations
      mModelTransform = mModelTransform * rotate;
      break;
    case MODE_MODEL_TRANSLATION:
      mModelTransform = mModelTransform * translate;
      break;
    case MODE_MODEL_SCALE:
      mModelScale = mModelScale * scale ;
      break;
    default: 
      std::cerr << "Error: invalid mode. \n";
  }

  invalidate();
  return true;
}

void Viewer::calculateViewport() {
  // std::cout << "Viewport Min Point: " << mViewportDimensions.mMin << std::endl;
  // std::cout << "Viewport Max Point: " << mViewportDimensions.mMax << std::endl;

  // std::cout << "Viewport Width: " << mViewportDimensions.mWidth << std::endl;
  // std::cout << "Viewport Height: " << mViewportDimensions.mHeight << std::endl;

  mViewport = translation(Vector3D(mViewportDimensions.mMin[0], mViewportDimensions.mMin[1], 0.0)) *
              translation(Vector3D(mViewportDimensions.mWidth/2.0, mViewportDimensions.mHeight/2.0, 0.0)) * 
              scaling(Vector3D(mViewportDimensions.mWidth/2.0, -mViewportDimensions.mHeight/2.0, 0.0));
}

void Viewer::displayAllMatrices() {
  std::cout << "Projection: \n" << mProjection << std::endl;
  std::cout << "Model: \n" << mModelTransform << std::endl;
  std::cout << "Eye: \n" << mView << std::endl;
  std::cout << "Viewport: \n" << mViewport << std::endl;
}

void Viewer::set_mode(Mode mode) {
  mMode = mode;
  mParent->LabelUpdates(mMode, mNear, mFar);
  // std::cout << "Mode: " << mMode << std::endl;
}

Viewer::Gnomon::Gnomon(Viewer* viewer) 
  : mParent(viewer) {}

void Viewer::Gnomon::draw_gnomon(Matrix4x4 transform) {
  // Drawing the Gnomon
  std::vector<Point3D> gnomonPoints;
  for (auto point : mGnomonPoints) {
      Point3D temp = mParent->mView * transform * point;
      gnomonPoints.push_back(temp);
  }

  // Draw the lines
  set_colour(Colour(0.0, 0.0, 1.0));
  mParent->drawLineWithClipping(gnomonPoints.at(mGnomonIndices.at(0).first), gnomonPoints.at(mGnomonIndices.at(0).second));
  set_colour(Colour(0.0, 1.0, 0.0));
  mParent->drawLineWithClipping(gnomonPoints.at(mGnomonIndices.at(1).first), gnomonPoints.at(mGnomonIndices.at(1).second));
  set_colour(Colour(1.0, 0.0, 0.0));
  mParent->drawLineWithClipping(gnomonPoints.at(mGnomonIndices.at(2).first), gnomonPoints.at(mGnomonIndices.at(2).second));

  // for (auto line : mGnomonIndices) {
  //   mParent->drawLineWithClipping(gnomonPoints.at(line.first), gnomonPoints.at(line.second));
  // }
}

// TODO: CLIPPING TO ALL SIDES
void Viewer::drawLineWithClipping(Point3D p1, Point3D p2) {
  Plane nearPlane(Vector3D(0.0, 0.0, 1.0), Point3D(0.0, 0.0, mNear));
  Plane farPlane(Vector3D(0.0, 0.0, -1.0), Point3D(0.0, 0.0, mFar));

  // After we have two points that are inside the plane,
  // we do projection

  p1 = mProjection * p1;
  p2 = mProjection * p2;

  // Clip planes
  if (!clip(p1, p2, nearPlane)) {
    // std::cout << "I'm clipping in near plane!\n";
    return;
  }
  if (!clip(p1, p2, farPlane)) {
    // std::cout << "I'm clipping in far plane!\n";
    return;
  }

  // Do homoginization
  homoginize(p1);
  homoginize(p2);

  // std::cout << "After homoginization: \n";
  // std::cout << "\t" << p1 << std::endl;
  // std::cout << "\t" << p2 << std::endl;

  // TODO: Line clipping for the sides
  Plane topPlane(Vector3D(0.0, -1.0, 0.0), Point3D(0.0, 1.0, 0.0));
  Plane bottomPlane(Vector3D(0.0, 1.0, 0.0), Point3D(0.0, -1.0, 0.0));
  Plane leftPlane(Vector3D(1.0, 0.0, 0.0), Point3D(-1.0, 0.0, 0.0));
  Plane rightPlane(Vector3D(-1.0, 0.0, 0.0), Point3D(1.0, 0.0, 0.0));
 
  // Clip planes
  if (!clip(p1, p2, topPlane)) {
    // std::cout << "I'm clipping in top plane!\n";
    return;
  }
  if (!clip(p1, p2, bottomPlane)) {
    // std::cout << "I'm clipping in bottom plane!\n";
    return;
  }
  if (!clip(p1, p2, leftPlane)) {
    // std::cout << "I'm clipping in left plane!\n";
    return;
  }
  if (!clip(p1, p2, rightPlane)) {
    // std::cout << "I'm clipping in right plane!\n";
    return;
  }
 
  // Do Viewport transformations
  p1 = mViewport * p1;
  p2 = mViewport * p2;

  // Draw Lines
  draw_line(Point2D(p1[0], p1[1]), Point2D(p2[0], p2[1]));
}

void Viewer::homoginize(Point3D &p) {
  p[0] = p[0] * mNear / p[2];
  p[1] = p[1] * mNear / p[2];
  p[2] = (p[2] * mFar + p[2] * mNear - 2 * mFar * mNear) / (p[2] * (mFar - mNear));
}

bool Viewer::clip(Point3D &p1, Point3D &p2, const Plane& plane) {
  double wecA = plane.mNormal.dot(p1 - plane.mPoint);
  double wecB = plane.mNormal.dot(p2 - plane.mPoint);

  // if Both are outside, don't draw function and return
  if (wecA < 0 && wecB < 0) 
    return false;

  // if One of the points is outside and the other is inside,
  // Find out which one is outside, and clip that one
  if (!(wecA >= 0 && wecB >= 0)) {

    // std::cout << "I got trimmmmmmmmmmmmed!" << std::endl;
    double t = wecA / (wecA - wecB);

    Vector3D line = p2 - p1;
    double x =  p1[0] + t * (line[0]) ;
    double y =  p1[1] + t * (line[1]) ;
    double z =  p1[2] + t * (line[2]) ;
    Point3D newPoint(x, y, z);

    if (wecA < 0)  {
      // std::cout << "\tP1 got trimmed from: " << p1;
      p1 = newPoint;
      // std::cout << " to " << p1 << std::endl;
    } else {
      // std::cout << "\tP2 got trimmed from: " << p2;
      p2 = newPoint;
      // std::cout << " to " << p2 << std::endl;
    }
  }

  return true;
}