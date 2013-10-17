#include "viewer.hpp"
#include "algebra.hpp"
#include <iostream>
#include <math.h>
#include <GL/gl.h>
#include <GL/glu.h>

static int BUFFER_SIZE = 512;

Viewer::Viewer(SceneNode* node) 
  : tempAngle(0.0)
  , mRoot(node)
  , mTranslation(node->get_translation())
  , mRotation(node->get_rotation())
  , mPosition(0.0, 0.0, 0.0)
  , mRotateX(0.0)
  , mRotateY(0.0)
  , mIsButton1Active(false)
  , mIsButton2Active(false)
  , mIsButton3Active(false)
  , mIsTrackballOn(false)
  , mIsZBufferOn(true) // TODO: change to false
  , mIsBackfaceCullingOn(true)
  , mIsFrontfaceCullingOn(false)
  , mCurrentHistoryNode(-1)
  , mMode(MODE_ENTIRE_OBJ)
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
}

Viewer::~Viewer()
{
  // Nothing to do here right now.

  // TODO: delet undo/redo stack
}

void Viewer::invalidate()
{
  // Force a rerender
  Gtk::Allocation allocation = get_allocation();
  get_window()->invalidate_rect( allocation, false);
}

bool Viewer::toggleTrackball() {
  mIsTrackballOn = !mIsTrackballOn;
  return mIsTrackballOn;
}

bool Viewer::toggleZBuffer() {
  mIsZBufferOn = !mIsZBufferOn;
  return mIsZBufferOn;
}

bool Viewer::toggleBackfaceCulling() {
  mIsBackfaceCullingOn = !mIsBackfaceCullingOn;
  return mIsBackfaceCullingOn;
}

bool Viewer::toggleFrontfaceCulling() {
  mIsFrontfaceCullingOn = !mIsFrontfaceCullingOn;
  return mIsFrontfaceCullingOn;
}

void Viewer::resetPosition() {
  mRoot->set_translation(mTranslation);
}

void Viewer::resetOrientation() {
  mRoot->set_rotation(mRotation);
}

void Viewer::resetJoints() {
  mRoot->reset_joints();
  mHistoryStack.clear();
  mRoot->unselect_all();
}

void Viewer::resetAll() {
  mRoot->set_translation(mTranslation);
  mRoot->set_rotation(mRotation);
  mRoot->reset_joints();
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

  glShadeModel(GL_SMOOTH);
  glClearColor( 0.0, 0.0, 0.0, 0.0 );
  // glEnable(GL_DEPTH_TEST);

  gldrawable->gl_end();
}

bool Viewer::updateGame() {
  on_expose_event(NULL);
  return true;
}

bool Viewer::on_expose_event(GdkEventExpose* event)
{
  Glib::RefPtr<Gdk::GL::Drawable> gldrawable = get_gl_drawable();

  if (!gldrawable) return false;

  if (!gldrawable->gl_begin(get_gl_context()))
    return false;

  // Enabling the options
  if (mIsZBufferOn) {
    glEnable(GL_DEPTH_TEST);
  } else {
    glDisable(GL_DEPTH_TEST);
  }
  if (mIsBackfaceCullingOn || mIsFrontfaceCullingOn) {
    glEnable(GL_CULL_FACE);

    if (mIsBackfaceCullingOn && mIsFrontfaceCullingOn) {
      glCullFace(GL_FRONT_AND_BACK);
    } else if (mIsBackfaceCullingOn) {
      glCullFace(GL_BACK);
    } else if (mIsFrontfaceCullingOn) {
      glCullFace(GL_FRONT);
    }
  } else {
    glDisable(GL_CULL_FACE);
  }
  glEnable(GL_NORMALIZE);
  glDisable(GL_LIGHTING);
  glEnable(GL_COLOR_MATERIAL);

  // Clear framebuffer
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Set up for perspective drawing 
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  glViewport(0, 0, get_width(), get_height());
  gluPerspective(40.0, (GLfloat)get_width()/(GLfloat)get_height(), 0.1, 1000.0);

  // change to model view for drawing
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  // Set up lighting
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  GLfloat light_position[] = { 1.0, 1.0, 1.0, 0.0 };
  glLightfv(GL_LIGHT0, GL_POSITION, light_position);

  // Draw stuff
  // glTranslated(mPosition[0], mPosition[1], mPosition[2]);
  // glRotated(mRotateX, 1.0, 0.0, 0.0);
  // glRotated(mRotateY, 0.0, 1.0, 0.0);

  mRoot->walk_gl(false);
  // draw_sphere();

  if (mIsTrackballOn) {
    draw_trackball_circle();
  }

  glPopMatrix();
  glMatrixMode(GL_PROJECTION);

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
  // std::cerr << "Stub: Button " << event->button << " pressed" << std::endl;
  mLastMousePos = Point2D(event->x, event->y);
  mCurrentMousePos = Point2D(event->x, event->y);
  if (event->button == 1) 
    mIsButton1Active = true;
  else if (event->button == 2) 
    mIsButton2Active = true;
  else if (event->button == 3)
    mIsButton3Active = true;
  return true;
}

void Viewer::processHits(int hits, unsigned int buffer[]) {
  unsigned int *ptr;

  ptr = (unsigned int*) buffer;

  float minZ = 1000.0; // Z is always less than 0.
  int id = 0;
  for (int i = 0; i < hits; ++i) {
    GLuint numOfNamesInHit = *ptr;
    float curMinZ = (float) *(ptr+1) / 0x7fffffff;

    ptr += 3 + numOfNamesInHit - 1;

    if (curMinZ < minZ) {
      id = *ptr;
      minZ = curMinZ;
    }
    ptr++;
  }
  mRoot->select(id);

  // this is for the undo/redo stack
  if (mRoot->isSelected(id)) {
    if (!mCurrentList.count(id)) {
      mCurrentList.insert(id);
    }
  } else {
    if (mCurrentList.count(id)) {
      mCurrentList.erase(id);
    }
  }
}

void Viewer::selection(GdkEventButton* event) {
  GLuint selectBuf[BUFFER_SIZE];
  GLint hits;
  GLint viewport[4];

  glGetIntegerv(GL_VIEWPORT, viewport);
  glSelectBuffer(BUFFER_SIZE, selectBuf);
  (void) glRenderMode(GL_SELECT);

  glInitNames();
  glPushName(0);

  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();

  gluPickMatrix((GLdouble)event->x, (GLdouble)(viewport[3] - event->y), 5, 5, viewport);
  glMatrixMode(GL_MODELVIEW);
  gluPerspective(40.0, (GLfloat)get_width()/(GLfloat)get_height(), 0.1, 1000.0);

  // glTranslated(mPosition[0], mPosition[1], mPosition[2]);
  mRoot->walk_gl(true);

  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
  hits = glRenderMode(GL_RENDER);

  if (hits > 0) {
    // std::cout << "There are " << hits << " hits!" << std::endl;
    processHits(hits, selectBuf);
  } else if (hits < 0) {
    // glRenderMode(GL_RENDER) returns negative if the buffer is too small
    BUFFER_SIZE *= 2;
  }
}

bool Viewer::on_button_release_event(GdkEventButton* event)
{
  // std::cerr << "Stub: Button " << event->button << " released" << std::endl;

  // Joint selection mdoe and left button click
  if (mMode == MODE_JOINTS) {
    if (event->button == 1) {
      selection(event);
    } else if ((event->button == 2 || event->button == 3) && !mCurrentList.empty()) {
      if (!mHistoryStack.empty() && mCurrentHistoryNode < mHistoryStack.size()-1) {
        while (mCurrentHistoryNode < mHistoryStack.size() - 1) {
          mHistoryStack.pop_back();
        }
      }

      mHistoryStack.push_back(mCurrentList);
      mCurrentHistoryNode++;
      mRoot->push_stack();

      // std::cout << "mCurrentHistoryNode: " << mCurrentHistoryNode << " mHistoryStack Size: " << mHistoryStack.size() << std::endl;
      if (event->button == 2) {
        mRotateX = 0;
      } else {
        mRotateY = 0;
      }

    //   // int last = mHistoryStack.size() - 1;
    //   // std::cout << "Last: " << last << " and current: " << mCurrentHistoryNode << std::endl;
    //   // while (last != mCurrentHistoryNode && !mHistoryStack.empty()) {
    //   //   delete mHistoryStack.at(last);
    //   //   mHistoryStack.pop_back();
    //   //   last--;
    //   // }


    //   // mHistoryStack.push_back(new HistoryNode(mRotateX, 0.0, mListOfSelectedNodes));
    //   // mCurrentHistoryNode++;
    //   std::cout << "New Size: " << mHistoryStack.size() << " Current Node: " << mCurrentHistoryNode << std::endl;
    // } else if (event->button == 3 && !mListOfSelectedNodes.empty()) {
    //   if (!mHistoryStack.empty() && mCurrentList < mHistoryStack.size()-1) {
    //     while (mCurrentList < mHistoryStack.size() - 1) {
    //       mHistoryStack.pop_back();
    //     }
    //   }

    //   mHistoryStack.push_back(mListOfSelectedNodes);
    //   mCurrentList++;
    //   mRoot->push_stack();
    //   mRotateY = 0;
    }
  }

  mCurrentMousePos = Point2D(event->x, event->y);
  mPosition = Vector3D(0.0, 0.0, 0.0);
  if (event->button == 1)
    mIsButton1Active = false;
  else if (event->button == 2)
    mIsButton2Active = false;
  else if (event->button == 3) 
    mIsButton3Active = false;
  return true;
}

bool Viewer::on_motion_notify_event(GdkEventMotion* event)
{
  // std::cerr << "Stub: Motion at " << event->x << ", " << event->y << std::endl;
  
  mLastMousePos = mCurrentMousePos;
  mCurrentMousePos = Point2D(event->x, event->y);
  double deltaX = mCurrentMousePos[0] - mLastMousePos[0];
  double deltaY = mCurrentMousePos[1] - mLastMousePos[1];
  if (mMode == MODE_ENTIRE_OBJ) {
    if (mIsButton1Active) {
      deltaX *= 0.1;
      deltaY *= 0.1;
      // mPosition = Vector3D(mPosition[0] + deltaX, mPosition[1] - deltaY, mPosition[2]);
      mPosition = Vector3D(deltaX, -deltaY, mPosition[2]);
    }  

    if (mIsButton2Active) {
      deltaY *= 0.1;
      // mPosition = Vector3D(mPosition[0], mPosition[1], mPosition[2] + deltaY);
      mPosition = Vector3D(mPosition[0], mPosition[1], deltaY);
    }

    mRoot->translate(mPosition);
    if (mIsButton3Active) {
      performTransform(mLastMousePos, mCurrentMousePos);

      // mRotateX = deltaX;
      // mRotateY = deltaY;
      // mRoot->rotate('x', mRotateX);
      // mRoot->rotate('y', mRotateY);
    }

  } else {
    if (mIsButton2Active) {
      mRotateX += -deltaY;
      mRoot->selection_rotate('x', -deltaY);
    }

    if (mIsButton3Active) {
      mRotateY += -deltaX;
      mRoot->selection_rotate('y', -deltaX);
    }
  }

  return true;
}

void Viewer::draw_trackball_circle()
{
  int current_width = get_width();
  int current_height = get_height();

  // Set up for orthogonal drawing to draw a circle on screen.
  // You'll want to make the rest of the function conditional on
  // whether or not we want to draw the circle this time around.
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glViewport(0, 0, current_width, current_height);
  glOrtho(0.0, (float)current_width, 
           0.0, (float)current_height, -0.1, 0.1);

  // change to model view for drawing
  glMatrixMode(GL_MODELVIEW);

  // Reset modelview matrix
  glLoadIdentity();

  // draw a circle for use with the trackball 
  glDisable(GL_LIGHTING);
  glEnable(GL_COLOR_MATERIAL);
  // glEnable(GL_LIGHTING);
  // glEnable(GL_LIGHT0);
  glEnable(GL_LINE_SMOOTH);
  glColor3f(1.0, 1.0, 1.0);
  double radius = current_width < current_height ? 
    (float)current_width * 0.25 : (float)current_height * 0.25;
  glTranslated((float)current_width * 0.5, (float)current_height * 0.5, 0);
  glBegin(GL_LINE_LOOP);
  for(size_t i=0; i<40; ++i) {
    double cosine = radius * cos(i*2*M_PI/40);
    double sine = radius * sin(i*2*M_PI/40);
    glVertex2f(cosine, sine);
  }
  glEnd();
  glColor3f(0.0, 0.0, 0.0);
  glDisable(GL_LINE_SMOOTH);
}

void Viewer::undo() {
  if(mHistoryStack.empty() || mCurrentHistoryNode < 0){
    // std::cerr << "Viewer Error: Stack is empty or node < 0." << std::endl;
    return;
  }

  mRoot->undo(mHistoryStack.at(mCurrentHistoryNode));
  mCurrentHistoryNode--;

  // HistoryNode* node = mHistoryStack.at(mCurrentHistoryNode--);
  // mRoot->undo(*node);

  // std::cout << "Undo! New Current: " << mCurrentHistoryNode << " Size: " << mHistoryStack.size() << std::endl;
}

void Viewer::redo() {
  if(mHistoryStack.empty() || mCurrentHistoryNode >= (int)mHistoryStack.size()-1) {
    // std::cerr << "Viewer Error: Stack is empty or node < 0." << std::endl;
    return;
  }

  mCurrentHistoryNode++;
  mRoot->redo(mHistoryStack.at(mCurrentHistoryNode));
  // std::cout << "Undo! New Current: " << mCurrentHistoryNode << " Size: " << mHistoryStack.size() << std::endl;
  // TODO
}

void Viewer::draw_sphere() {
  float theta = 0.0f, phi = 0.0f, deltaTheta = M_PI / 20.0, deltaPhi = M_PI / 20.0;
  Point3D p1, p2, p3, p4;
  float r = 1.0;

  tempAngle += 1.0;
  if (tempAngle >= 360.0) {
    tempAngle = tempAngle - 360.0;
  }

  glPushMatrix();
  // glLoadIdentity();
  glTranslated(0.0, 0.0, -4.0);
  glRotated(tempAngle, 0.0, 1.0, 0.0);

  Colour black = Colour(0.0, 0.0, 0.0);
  Colour white = Colour(1.0, 1.0, 1.0);
  bool isBlack = true;
  for (theta = 0; theta <= 2 * M_PI; theta += deltaTheta) {
    glBegin(GL_QUAD_STRIP);
    for (phi = 0; phi <= 2 * M_PI; phi += deltaPhi) {
      p1 = Point3D(
        r * cosf(theta) * sinf(phi),
        r * sinf(theta) * sinf(phi),
        r * cosf(phi));

      p2 = Point3D(
        r * cosf(theta + deltaTheta) * sinf(phi),
        r * sinf(theta + deltaTheta) * sinf(phi),
        r * cosf(phi));

      // p1 = Point3D(
      //   r * sin(phi + deltaPhi) * sin(theta + deltaTheta),
      //   r * cos(phi + deltaPhi),
      //   r * sin(phi + deltaPhi) * cos(theta + deltaTheta));

      // p2 = Point3D(
      //   r * sin(phi) * sin(theta + deltaTheta),
      //   r * cos(phi),
      //   r * sin(phi) * cos(theta + deltaTheta));

      // p3 = Point3D(
      //   r * sin(phi) * sin(theta),
      //   r * cos(phi),
      //   r * sin(phi) * cos(theta));

      // p4 = Point3D(
      //   r * sin(phi + deltaPhi) * sin(theta),
      //   r * cos(phi + deltaPhi),
      //   r * sin(phi + deltaPhi) * cos(theta));

      // or whatever colour it is
      // glColor3f(1.0, 1.0, 1.0);
      // glNormal somehow
      Colour temp(white);
      if (isBlack) {
        temp = black;
        isBlack = false;
      } else {
        temp = white;
        isBlack = true;
      }


      glColor3f(temp.R(), temp.G(), temp.B());
      glVertex3f(p1[0], p1[1], p1[2]);
      glVertex3f(p2[0], p2[1], p2[2]);
      // glVertex3f(p1[0], p1[1], p1[2]);
      // glVertex3f(p2[0], p2[1], p3[2]);
      // glVertex3f(p3[0], p3[1], p3[2]);
      // glVertex3f(p4[0], p4[1], p4[2]);
    }
    glEnd();
  }

  glPopMatrix();
}

// TRACKBALL stuff
void Viewer::performTransform(Point2D last, Point2D cur) {
  float  fRotVecX, fRotVecY, fRotVecZ;
  Matrix4x4 mNewMat;
  float fDiameter;
  int iCenterX, iCenterY;
  float fNewModX, fNewModY, fOldModX, fOldModY;

  /* vCalcRotVec expects new and old positions in relation to the center of the
   * trackball circle which is centered in the middle of the window and
   * half the smaller of nWinWidth or nWinHeight.
   */
  float width = get_width();
  float height = get_height();

  fDiameter = (width < height) ? width * 0.5 : height * 0.5;
  iCenterX = width / 2;
  iCenterY = height / 2;
  fOldModX = last[0] - iCenterX;
  fOldModY = last[1] - iCenterY;
  fNewModX = cur[0] - iCenterX;
  fNewModY = cur[1] - iCenterY;

  calcRotVec(fNewModX, fNewModY,
                  fOldModX, fOldModY,
                  fDiameter,
                  &fRotVecX, &fRotVecY, &fRotVecZ);
  /* Negate Y component since Y axis increases downwards
   * in screen space and upwards in OpenGL.
   */
  axisRotMatrix(fRotVecX, -fRotVecY, fRotVecZ, mNewMat);

  // Since all these matrices are meant to be loaded
  // into the OpenGL matrix stack, we need to transpose the
  // rotation matrix (since OpenGL wants rows stored
  // in columns)
  // mNewMat.transpose();
  mRoot->multiplyRotation(mNewMat);
  // vRightMultiply(mRotations, mNewMat);
}

void Viewer::calcRotVec(float fNewX, float fNewY,
                 float fOldX, float fOldY,
                 float fDiameter,
                 float *fVecX, float *fVecY, float *fVecZ) {
   // long  nXOrigin, nYOrigin;
   float fNewVecX, fNewVecY, fNewVecZ,        /* Vector corresponding to new mouse location */
         fOldVecX, fOldVecY, fOldVecZ,        /* Vector corresponding to old mouse location */
         fLength;

   /* Vector pointing from center of virtual trackball to
    * new mouse position
    */
   fNewVecX    = fNewX * 2.0 / fDiameter;
   fNewVecY    = fNewY * 2.0 / fDiameter;
   fNewVecZ    = (1.0 - fNewVecX * fNewVecX - fNewVecY * fNewVecY);

   /* If the Z component is less than 0, the mouse point
    * falls outside of the trackball which is interpreted
    * as rotation about the Z axis.
    */
   if (fNewVecZ < 0.0) {
      fLength = sqrt(1.0 - fNewVecZ);
      fNewVecZ  = 0.0;
      fNewVecX /= fLength;
      fNewVecY /= fLength;
   } else {
      fNewVecZ = sqrt(fNewVecZ);
   }

   /* Vector pointing from center of virtual trackball to
    * old mouse position
    */
   fOldVecX    = fOldX * 2.0 / fDiameter;
   fOldVecY    = fOldY * 2.0 / fDiameter;
   fOldVecZ    = (1.0 - fOldVecX * fOldVecX - fOldVecY * fOldVecY);
 
   /* If the Z component is less than 0, the mouse point
    * falls outside of the trackball which is interpreted
    * as rotation about the Z axis.
    */
   if (fOldVecZ < 0.0) {
      fLength = sqrt(1.0 - fOldVecZ);
      fOldVecZ  = 0.0;
      fOldVecX /= fLength;
      fOldVecY /= fLength;
   } else {
      fOldVecZ = sqrt(fOldVecZ);
   }

   /* Generate rotation vector by calculating cross product:
    * 
    * fOldVec x fNewVec.
    * 
    * The rotation vector is the axis of rotation
    * and is non-unit length since the length of a crossproduct
    * is related to the angle between fOldVec and fNewVec which we need
    * in order to perform the rotation.
    */
   *fVecX = fOldVecY * fNewVecZ - fNewVecY * fOldVecZ;
   *fVecY = fOldVecZ * fNewVecX - fNewVecZ * fOldVecX;
   *fVecZ = fOldVecX * fNewVecY - fNewVecX * fOldVecY;
}

void Viewer::axisRotMatrix(float fVecX, float fVecY, float fVecZ, Matrix4x4& mNewMat) {
    float fRadians, fInvLength, fNewVecX, fNewVecY, fNewVecZ;

    /* Find the length of the vector which is the angle of rotation
     * (in radians)
     */
    fRadians = sqrt(fVecX * fVecX + fVecY * fVecY + fVecZ * fVecZ);

    /* If the vector has zero length - return the identity matrix */
    if (fRadians > -0.000001 && fRadians < 0.000001) {

        mNewMat = Matrix4x4();
        return;
    }

    /* Normalize the rotation vector now in preparation for making
     * rotation matrix. 
     */
    fInvLength = 1 / fRadians;
    fNewVecX   = fVecX * fInvLength;
    fNewVecY   = fVecY * fInvLength;
    fNewVecZ   = fVecZ * fInvLength;

    /* Create the arbitrary axis rotation matrix */
    double dSinAlpha = sin(fRadians);
    double dCosAlpha = cos(fRadians);
    double dT = 1 - dCosAlpha;

    mNewMat[0][0] = dCosAlpha + fNewVecX*fNewVecX*dT;
    mNewMat[0][1] = fNewVecX*fNewVecY*dT + fNewVecZ*dSinAlpha;
    mNewMat[0][2] = fNewVecX*fNewVecZ*dT - fNewVecY*dSinAlpha;
    mNewMat[0][3] = 0;

    mNewMat[1][0] = fNewVecX*fNewVecY*dT - dSinAlpha*fNewVecZ;
    mNewMat[1][1] = dCosAlpha + fNewVecY*fNewVecY*dT;
    mNewMat[1][2] = fNewVecY*fNewVecZ*dT + dSinAlpha*fNewVecX;
    mNewMat[1][3] = 0;

    mNewMat[2][0] = fNewVecZ*fNewVecX*dT + dSinAlpha*fNewVecY;
    mNewMat[2][1] = fNewVecZ*fNewVecY*dT - dSinAlpha*fNewVecX;
    mNewMat[2][2] = dCosAlpha + fNewVecZ*fNewVecZ*dT;
    mNewMat[2][3] = 0;

    mNewMat[3][0] = 0;
    mNewMat[3][1] = 0;
    mNewMat[3][2] = 0;
    mNewMat[3][3] = 1;
}