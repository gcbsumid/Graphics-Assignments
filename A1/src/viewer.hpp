#ifndef CS488_VIEWER_HPP
#define CS488_VIEWER_HPP

#include <gtkmm.h>
#include <gtkglmm.h>
#include "algebra.hpp"
#include "game.hpp"

// The "main" OpenGL widget
class Viewer : public Gtk::GL::DrawingArea {
public:
  Viewer();
  virtual ~Viewer();

  // A useful function that forces this widget to rerender. If you
  // want to render a new frame, do not call on_expose_event
  // directly. Instead call this, which will cause an on_expose_event
  // call when the time is right.
  void invalidate();
  
  enum Mode {
    MODE_WIREFRAME = 0,
    MODE_FACE,
    MODE_MULTICOLOURED
  };
  
  void set_mode(Mode mode);

  bool updateGame();

  bool keypress(GdkEventKey *ev);

  void resetView();
  void newGame();

  enum Difficulty {
    DIFF_EASY = 60,
    DIFF_MEDIUM = 30,
    DIFF_HARD = 15,
    DIFF_GODLIKE = 6
  };

  void changeDifficulty(Difficulty diff);

protected:

  void drawCube(Point3D position, GLenum type, Colour colour, bool isMulticoloured = false, Point3D scale = Point3D(1.0, 1.0, 1.0));

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
  void drawFace(Point3D p1, Point3D p2, Point3D p3, Point3D p4, GLenum mode, Colour colour);

  Mode mMode;
  Game mGame;

  enum Axis {
    AXIS_X = 0,
    AXIS_Y,
    AXIS_Z
  };

  bool mIsRotateActive;
  Axis mActiveAxis;

  Point2D mLastMousePos;
  Point2D mCurrentMousePos;

  double mPersistenceValue;
  double mAngles[3];
  bool mIsPersistenceUsed;
  bool mIsScaleUsed;

  double mScaleValue;

  bool mShouldUpdate;

  int mCurrentDifficulty;
  int mCurrentTick;
};

#endif
