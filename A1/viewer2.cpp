#include "viewer.hpp"
#include <iostream>
#include <cmath>
#include <GL/gl.h>
#include <GL/glu.h>
#include <gtkmm.h>
#include <gtkglmm.h>

static const Point3D CUBE[] = {
  Point3D(0.0, 0.0, 0.0), 
  Point3D(1.0, 0.0, 0.0), 
  Point3D(0.0, 1.0, 0.0), 
  Point3D(0.0, 0.0, -1.0),
  Point3D(1.0, 1.0, 0.0), 
  Point3D(0.0, 1.0, -1.0),
  Point3D(1.0, 0.0, -1.0),
  Point3D(1.0, 1.0, -1.0) 
};

static const Colour MULTICOLOURED_FACE[] = {
    Colour(1.0, 0.0, 0.0),
    Colour(0.0, 1.0, 0.0),
    Colour(0.0, 0.0, 1.0),
    Colour(1.0, 1.0, 0.0),
    Colour(0.0, 1.0, 1.0),
    Colour(1.0, 0.0, 1.0)    
  };

static const Colour NORMAL_FACE(0.0, 0.0, 0.0); 
static const Colour RED_FACE(1.0, 0.0, 0.0); 

static const int HEIGHT = 20;
static const int WIDTH = 10;

static double distance(const Point2D& p1, const Point2D& p2) {
  return sqrt( pow( p2[0] - p1[0] , 2) + pow( p2[1] - p1[1] , 2) );
}

Viewer::Viewer()
  : mMode(MODE_WIREFRAME)
  , mGame(WIDTH, HEIGHT)
  , mIsRotateActive(false)
  , mActiveAxis(AXIS_X)
  , mLastMousePos(0.0, 0.0)
  , mCurrentMousePos(0.0, 0.0)
  , mPersistenceValue(0.0)
  , mIsPersistenceUsed(false)
  , mScaleValue(1.0)
  , mShouldUpdate(true)
{
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

  resetView();
}

Viewer::~Viewer()
{
  // Nothing to do here right now.
}

bool Viewer::updateGame() {
  
  if (mGame.tick() >= 0) {
    on_expose_event(NULL);
  }
  
  return true;
}

// bool Viewer::refreshScreen() {
//   if (mIsPersistenceUsed) {
//     if (mActiveAxis == AXIS_X) {
//       mAngles[0] += mPersistenceValue;
//     } else if (mActiveAxis == AXIS_Y) {
//       mAngles[1] += mPersistenceValue;
//     } else if (mActiveAxis == AXIS_Z) {
//       mAngles[2] += mPersistenceValue;
//     }
//   }

//   for (int i = 0; i < 3; ++i) {
//     if (mAngles[i] >= 360) {
//       mAngles[i] -= 360.0;
//     }
//   }

//   on_expose_event(NULL);
//   return true;
// }

bool Viewer::applyPersistence() {
  // TODO: This
  return true;
}

bool Viewer::keypress(GdkEventKey *ev) {
  switch (ev->keyval) {
    case GDK_Left:
      mGame.moveLeft();
      break;
    case GDK_Right:
      mGame.moveRight();
      break;
    case GDK_Up:
      mGame.rotateCCW();
      break;
    case GDK_Down:
      mGame.rotateCW();
      break;
    case GDK_space:
      mGame.drop();
      break;
    default:
      std::cerr << "Error: Invalid Key Value pressed." << std::endl;
      return false;
  }
  on_expose_event(NULL);
  return true;
}

void Viewer::set_mode(Mode mode) {
  mMode = mode;
  on_expose_event(NULL);
}

void Viewer::resetView() {
  for (int i = 0; i < 3; ++i) {
    mAngles[i] = 0.0;
  }

  mScaleValue = 1.0;
  on_expose_event(NULL);
}

void Viewer::newGame() {
  mGame.reset();
  on_expose_event(NULL);
}

void Viewer::invalidate()
{
  //Force a rerender
  Gtk::Allocation allocation = get_allocation();
  get_window()->invalidate_rect( allocation, false);
  
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

  // Just enable depth testing and set the background colour.
  glEnable(GL_DEPTH_TEST);
  glClearColor(0.7, 0.7, 1.0, 0.0);

  gldrawable->gl_end();
}

bool Viewer::on_expose_event(GdkEventExpose* event)
{
  Glib::RefPtr<Gdk::GL::Drawable> gldrawable = get_gl_drawable();

  if (!gldrawable) return false;

  if (!gldrawable->gl_begin(get_gl_context()))
    return false;

  // Clear the screen

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Modify the current projection matrix so that we move the 
  // camera away from the origin.  We'll draw the game at the
  // origin, and we need to back up to see it.

  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glTranslated(0.0, 0.0, -40.0);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  // Not implemented: set up lighting (if necessary)

  // Not implemented: scale and rotate the scene
  glScaled(mScaleValue, mScaleValue, mScaleValue);

  // Rotate when mouse is pressed.
  // glTranslated(-5, -10, 0);
  glRotated(mAngles[0], 1.0, 0.0, 0.0);
  glRotated(mAngles[1], 0.0, 1.0, 0.0);
  glRotated(mAngles[2], 0.0, 0.0, 1.0);
  // glTranslated(5, 10, 0);

  // You'll be drawing unit cubes, so the game will have width
  // 10 and height 24 (game = 20, stripe = 4).  Let's translate
  // the game so that we can draw it starting at (0,0) but have
  // it appear centered in the window.
  glTranslated(-5.0, -12.0, 0.0);

  // Creating walls
  Point3D leftWall(-1.0, 0.0, 0.0);
  drawCube(leftWall, GL_QUADS, NORMAL_FACE, Point3D(1.0, 20.0, 1.0));

  Point3D bottomWall(-1.0, -1.0, 0.0);
  drawCube(bottomWall, GL_QUADS, NORMAL_FACE, Point3D(12.0, 1.0, 1.0));

  Point3D rightWall(10.0, 0.0, 0.0);
  drawCube(rightWall, GL_QUADS, NORMAL_FACE, Point3D(1.0, 20.0, 1.0));

  // Creating the pieces
  if (mMode != MODE_WIREFRAME) {
    for (int i = 0; i < mGame.getWidth(); ++i) {
      for (int j = 0; j < mGame.getHeight() + 4; ++j) {
        int piece = mGame.get(j, i);
        if (piece != -1) {
          Point3D cube(i, j, 0);
          Colour colour = (mMode == MODE_MULTICOLOURED) ? MULTICOLOURED_FACE[piece] : RED_FACE;
          drawCube(cube, GL_QUADS, colour);
        }
      }
    } 
  }

  // Creating the Wireframes
  if (mMode == MODE_WIREFRAME) {
    for (int i = 0; i < mGame.getWidth(); ++i) {
      for (int j = 0; j < mGame.getHeight() + 4; ++j) {
        if (mGame.get(j, i) != -1) {
          Point3D cube(i, j, 0);
          drawCube(cube, GL_LINE_LOOP, NORMAL_FACE);
        }
      }
    } 
  }

  // We pushed a matrix onto the PROJECTION stack earlier, we 
  // need to pop it.

  glMatrixMode(GL_PROJECTION);
  glPopMatrix();

  // Swap the contents of the front and back buffers so we see what we
  // just drew. This should only be done if double buffering is enabled.
  gldrawable->swap_buffers();

  gldrawable->gl_end();

  return true;
}

void Viewer::drawCube(Point3D position, GLenum type, Colour colour, Point3D scale) {
  glPushMatrix();
  glTranslated(position[0], position[1], position[2]);
  glScaled(scale[0], scale[1], scale[2]);

  glColor3d(0.0, 0.0, 0.0);

  drawFace(CUBE[0], CUBE[1], CUBE[4], CUBE[2], type, colour);
  drawFace(CUBE[2], CUBE[5], CUBE[7], CUBE[4], type, colour);
  drawFace(CUBE[5], CUBE[7], CUBE[6], CUBE[3], type, colour);
  drawFace(CUBE[0], CUBE[1], CUBE[6], CUBE[3], type, colour);
  drawFace(CUBE[0], CUBE[2], CUBE[5], CUBE[3], type, colour);
  drawFace(CUBE[4], CUBE[7], CUBE[6], CUBE[1], type, colour);

  glPopMatrix();
}

void Viewer::drawFace(
  Point3D p1, 
  Point3D p2, 
  Point3D p3, 
  Point3D p4, 
  GLenum mode, 
  Colour colour) 
{
    glBegin(mode);
    glColor3d(colour.R(), colour.G(), colour.B());
    glVertex3d(p1[0], p1[1], p1[2]);
    glVertex3d(p2[0], p2[1], p2[2]);
    glVertex3d(p3[0], p3[1], p3[2]);
    glVertex3d(p4[0], p4[1], p4[2]);
    glEnd();
}

bool Viewer::on_configure_event(GdkEventConfigure* event)
{
  Glib::RefPtr<Gdk::GL::Drawable> gldrawable = get_gl_drawable();

  if (!gldrawable) return false;
  
  if (!gldrawable->gl_begin(get_gl_context()))
    return false;

  // Set up perspective projection, using current size and aspect
  // ratio of display

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glViewport(0, 0, event->width, event->height);
  gluPerspective(40.0, (GLfloat)event->width/(GLfloat)event->height, 0.1, 1000.0);

  // Reset to modelview matrix mode
  
  glMatrixMode(GL_MODELVIEW);

  gldrawable->gl_end();

  return true;
}

bool Viewer::on_button_press_event(GdkEventButton* event)
{
  switch (event->button) {
    case 1: // Left mouse click
      mActiveAxis = AXIS_X;
      break;
    case 2: // Middle (scroll wheel) mouse click
      mActiveAxis = AXIS_Y;
      break;
    case 3: // Right mouse click
      mActiveAxis = AXIS_Z;
      break;
    default:
      std::cerr << "Error: Invalid button clicked. Only Left, Middle (scroll wheel), and Right mouse buttons are valid." << std::endl;
  }
  // std::cerr << "Stub: Button " << event->button << " pressed" << std::endl;
  mLastMousePos = Point2D(event->x, event->y);
  mCurrentMousePos = Point2D(event->x, event->y);
  mIsRotateActive = true;
  mIsPersistenceUsed = true;
  mPersistenceValue = 0.0;
  return true;
}

bool Viewer::on_button_release_event(GdkEventButton* event)
{
  // std::cerr << "Stub: Button " << event->button << " released" << std::endl;
  mCurrentMousePos = Point2D(event->x, event->y);
  if (distance(mLastMousePos, mCurrentMousePos) == 0) {
    mIsPersistenceUsed = false;
  }

  mIsRotateActive = false;
  return true;
}

bool Viewer::on_motion_notify_event(GdkEventMotion* event)
{
    // std::cerr << "Stub: Motion at " << event->x << ", " << event->y << std::endl;
  mCurrentMousePos = Point2D(event->x, event->y);
  double dist = distance(mCurrentMousePos, mLastMousePos);

  // TODO: do something for the Y
  // std::cerr << "distance: " << dist << std::endl;
  if (mCurrentMousePos[0] - mLastMousePos[0] < 0) 
  {
    dist *= -1;
  }
  mPersistenceValue = dist;

  if (event->state & GDK_SHIFT_MASK ){
    mScaleValue += mPersistenceValue*0.01;
    mIsPersistenceUsed = false;
  } else {
    mIsPersistenceUsed = true;
  }

  mLastMousePos = mCurrentMousePos;
  // std::cerr << "Stub: Motion at " << event->x << ", " << event->y << std::endl;
  return true;
}
