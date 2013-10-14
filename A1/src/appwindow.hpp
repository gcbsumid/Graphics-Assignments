#ifndef APPWINDOW_HPP
#define APPWINDOW_HPP

#include <gtkmm.h>
#include "viewer.hpp"

class AppWindow : public Gtk::Window {
public:
  AppWindow();
  
protected:
  virtual bool on_key_press_event( GdkEventKey *ev );

private:
  // A "vertical box" which holds everything in our window
  Gtk::VBox mVbox;

  // The menubar, with all the menus at the top of the window
  Gtk::MenuBar mMenubar;
  // Each menu itself
  Gtk::Menu mMenuApp;

  // Radio button group for what type of display mode
  Gtk::RadioMenuItem::Group mMenuGroup;
  Gtk::Menu mMenuMode;

  // Radio button for difficulty setting
  Gtk::RadioMenuItem::Group mDifficultyGroup;
  Gtk::Menu mMenuDifficulty;

  Glib::RefPtr<Glib::TimeoutSource> mTimeoutSource;

  // The main OpenGL area
  Viewer mViewer;
};

#endif
