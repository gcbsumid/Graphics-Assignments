#include "appwindow.hpp"

AppWindow::AppWindow(SceneNode* node)
  : mViewer(node)
{
  set_title("Advanced Ergonomics Laboratory");

  // A utility class for constructing things that go into menus, which
  // we'll set up next.
  using Gtk::Menu_Helpers::MenuElem;
  
  // Set up the application menu
  // The slot we use here just causes AppWindow::hide() on this,
  // which shuts down the application.
  mMenuApp.items().push_back(MenuElem("_Quit", Gtk::AccelKey("q"),
    sigc::mem_fun(*this, &AppWindow::hide)));
  
  sigc::slot<bool> gameTick = sigc::mem_fun(mViewer, &Viewer::updateGame);
  Glib::signal_timeout().connect(gameTick, 50);
  
  // Set up Options Menu
  mMenuOptions.items().push_back(MenuElem("_Circle", Gtk::AccelKey("c"), 
    sigc::mem_fun(mViewer, &Viewer::toggleTrackball)));
  mMenuOptions.items().push_back(MenuElem("_Z-buffer", Gtk::AccelKey("z"), 
    sigc::mem_fun(mViewer, &Viewer::toggleZBuffer)));
  mMenuOptions.items().push_back(MenuElem("_Backface Culling", Gtk::AccelKey("b"), 
    sigc::mem_fun(mViewer, &Viewer::toggleBackfaceCulling)));
  mMenuOptions.items().push_back(MenuElem("_Frontface Culling", Gtk::AccelKey("f"), 
    sigc::mem_fun(mViewer, &Viewer::toggleFrontfaceCulling)));

  // Set up the menu bar
  mMenubar.items().push_back(Gtk::Menu_Helpers::MenuElem("_Application", mMenuApp));
  mMenubar.items().push_back(Gtk::Menu_Helpers::MenuElem("_Edit", mMenuEdit));
  mMenubar.items().push_back(Gtk::Menu_Helpers::MenuElem("_Mode", mMenuMode));
  mMenubar.items().push_back(Gtk::Menu_Helpers::MenuElem("_Options", mMenuOptions));

  // Pack in our widgets
  
  // First add the vertical box as our single "top" widget
  add(mVBox);

  // Put the menubar on the top, and make it as small as possible
  mVBox.pack_start(mMenubar, Gtk::PACK_SHRINK);

  // Put the viewer below the menubar. pack_start "grows" the widget
  // by default, so it'll take up the rest of the window.
  mViewer.set_size_request(300, 300);
  mVBox.pack_start(mViewer);

  show_all();
}
