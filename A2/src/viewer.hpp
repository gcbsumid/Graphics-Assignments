#ifndef CS488_VIEWER_HPP
#define CS488_VIEWER_HPP

#include <gtkmm.h>
#include <gtkglmm.h>
#include <vector>
#include <utility>
#include "algebra.hpp"

class AppWindow;

// The "main" OpenGL widget
class Viewer : public Gtk::GL::DrawingArea {
public:
  Viewer(AppWindow* parent);
  virtual ~Viewer();

  // A useful function that forces this widget to rerender. If you
  // want to render a new frame, do not call on_expose_event
  // directly. Instead call this, which will cause an on_expose_event
  // call when the time is right.
  void invalidate();

  // *** Fill in these functions (in viewer.cpp) ***

  // Set the parameters of the current perspective projection using
  // the semantics of gluPerspective().
  void set_perspective(double fov, double aspect,
                       double near, double far);

  // Restore all the transforms and perspective parameters to their
  // original state. Set the viewport to its initial size.
  void reset_view();

  enum Mode {
    MODE_EYE_ROTATION = 0,
    MODE_EYE_TRANSLATION,
    MODE_EYE_PERSPECTIVE,
    MODE_MODEL_ROTATION,
    MODE_MODEL_TRANSLATION,
    MODE_MODEL_SCALE,
    MODE_VIEWPORT
  };

  void set_mode(Mode mode);

protected:

  // Events we implement
  // Note that we could use gtkmm's "signals and slots" mechanism
  // instead, but for many classes there's a convenient member
  // function one just needs to define that'll be called with the
  // event.

  // Called when GL is first initialized
  virtual void on_realize();
  // Called when our window needs to be redrawn
  virtual bool on_expose_event(GdkEventExpose* event);
  // Called when the window is resized
  virtual bool on_configure_event(GdkEventConfigure* event);
  // Called when a mouse button is pressed
  virtual bool on_button_press_event(GdkEventButton* event);
  // Called when a mouse button is released
  virtual bool on_button_release_event(GdkEventButton* event);
  // Called when the mouse moves
  virtual bool on_motion_notify_event(GdkEventMotion* event);

private:

  class Gnomon {
  public: 
    Gnomon(Viewer* viewer);
    ~Gnomon() {}
    void draw_gnomon(Matrix4x4 transform);

    Viewer* mParent;
    static std::vector<Point3D> mGnomonPoints;
    static std::vector<std::pair<int, int> > mGnomonIndices;
  };

  struct Plane {
    Plane(Vector3D normal, Point3D point) 
      : mNormal(normal), mPoint(point) {}
    Vector3D mNormal;
    Point3D mPoint;
  };

  static void clamp(double &val, const double min, const double max);
  static double min(double val1, double val2);
  static double max(double val1, double val2);

  void calculateViewport();
  void displayAllMatrices();
  void drawLineWithClipping(Point3D p1, Point3D p2);
  void homoginize(Point3D &p);
  bool clip(Point3D &p1, Point3D &p2, const Plane& plane);

  // bool clipLine(Point3D &p1, Point3D &p2, const Plane& plane);
  // bool clip(Point3D &p1, Point3D &p2, const Vector3D& line);

  Mode mMode;
  char mAxis;

  static std::vector<Point3D> mModelPoints;
  static std::vector<std::pair<int, int> > mModelIndices;
  Point2D mLastMousePos;
  Point2D mCurrentMousePos;

  Gnomon mModelGnomon;
  Gnomon mWorldGnomon;

  // Buttons
  bool mButton1Press;
  bool mButton2Press;
  bool mButton3Press;

  double mFOV;
  double mAspect;
  double mNear;
  double mFar;


  struct Viewport {
    double mWidth;
    double mHeight;
    Point2D mMin;
    Point2D mMax;
  } mViewportDimensions;

  Point2D mStartPoint;
  Point2D mEndPoint;

  Matrix4x4 mModelTransform; // stores translation and rotation 
  Matrix4x4 mModelScale; // stores scale transformation
  Matrix4x4 mView; // stores view transform
  Matrix4x4 mProjection; // stores the projection matrix
  Matrix4x4 mViewport; // stores viewport matrix

  AppWindow *mParent;
};

#endif
