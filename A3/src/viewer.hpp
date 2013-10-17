#ifndef CS488_VIEWER_HPP
#define CS488_VIEWER_HPP

#include <vector>
#include <set>
#include <gtkmm.h>
#include <gtkglmm.h>
#include "scene.hpp"

// The "main" OpenGL widget
class Viewer : public Gtk::GL::DrawingArea {
public:
  Viewer(SceneNode* node);
  virtual ~Viewer();

  bool updateGame();
  // A useful function that forces this widget to rerender. If you
  // want to render a new frame, do not call on_expose_event
  // directly. Instead call this, which will cause an on_expose_event
  // call when the time is right.
  void invalidate();

  bool toggleTrackball();
  bool toggleZBuffer();
  bool toggleBackfaceCulling();
  bool toggleFrontfaceCulling();
  
  // Resetting stuff
  void resetPosition();
  void resetOrientation();
  void resetJoints();
  void resetAll();

  // Editting stuff
  void undo();
  void redo();

  // Selection Mode
  enum Mode {
    MODE_ENTIRE_OBJ = 0,
    MODE_JOINTS
  };
  void setSelectionMode(Mode mode) { 
    mMode = mode; 
    mRoot->unselect_all();
  }

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

  // Draw a circle for the trackball, with OpenGL commands.
  // Assumes the context for the viewer is active.
  void draw_trackball_circle();
  
private:

  void draw_sphere();
  void selection(GdkEventButton* event);
  void processHits(int hits, unsigned int buffer[]);

  void performTransform(Point2D last, Point2D cur);
  void calcRotVec(float fNewX, float fNewY,
                 float fOldX, float fOldY,
                 float fDiameter,
                 float *fVecX, float *fVecY, float *fVecZ);
  void axisRotMatrix(float fVecX, float fVecY, float fVecZ, Matrix4x4& mNewMat);

  double tempAngle;

  SceneNode* mRoot;
  Matrix4x4 mTranslation;
  Matrix4x4 mRotation;


  Point2D mLastMousePos;
  Point2D mCurrentMousePos;
  Vector3D mPosition;
  double mRotateX;
  double mRotateY;
  bool mIsButton1Active;
  bool mIsButton2Active;
  bool mIsButton3Active;

  // Options menu
  bool mIsTrackballOn;
  bool mIsZBufferOn;
  bool mIsBackfaceCullingOn;
  bool mIsFrontfaceCullingOn;

  std::vector<std::set<int>> mHistoryStack; 
  std::set<int> mCurrentList;
  int mCurrentHistoryNode;

  Mode mMode;
};

#endif
