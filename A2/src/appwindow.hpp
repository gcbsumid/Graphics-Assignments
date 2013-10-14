#ifndef APPWINDOW_HPP
#define APPWINDOW_HPP

#include <gtkmm.h>
#include "viewer.hpp"

class AppWindow : public Gtk::Window {
public:
  AppWindow();

  void LabelUpdates(Viewer::Mode mode, double nearPlaneValue, double farPlaneValue);
  
protected:

private:
  // A "vertical box" which holds everything in our window
  Gtk::VBox mVBox;

  // The menubar, with all the menus at the top of the window
  Gtk::MenuBar mMenuBar;
  // Each menu itself
  Gtk::Menu mMenuApp;

  // RadioButton group for interaction mode
  Gtk::RadioMenuItem::Group mMenuInteractionGroup;
  Gtk::Menu mMenuInteraction;

  // Labels
  Gtk::Label mCurrentMode;
  Gtk::Label mPlaneValue;

  // The main OpenGL area
  Viewer mViewer;
};

#endif
