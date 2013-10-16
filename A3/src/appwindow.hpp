#ifndef APPWINDOW_HPP
#define APPWINDOW_HPP

#include <gtkmm.h>
#include "viewer.hpp"
#include "scene.hpp"

class AppWindow : public Gtk::Window {
public:
  AppWindow(SceneNode* node);
  
protected:

private:
  // A "vertical box" which holds everything in our window
  Gtk::VBox mVBox;

  // The menubar, with all the menus at the top of the window
  Gtk::MenuBar mMenubar;
  // Each menu itself
  Gtk::Menu mMenuApp;
  Gtk::Menu mMenuEdit;
  Gtk::Menu mMenuMode;
  Gtk::Menu mMenuOptions;

  Gtk::RadioMenuItem::Group mMenuGroup;

  // The main OpenGL area
  Viewer mViewer;
};

#endif
