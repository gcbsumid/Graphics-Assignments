#include "appwindow.hpp"
#include <sstream>

AppWindow::AppWindow(SceneNode* node)
  : mViewer(node)
  , mTrack(false)
  , mZBuff(false)
  , mBack(false)
  , mFront(false)
{
  set_title("Advanced Ergonomics Laboratory");

  // A utility class for constructing things that go into menus, which
  // we'll set up next.
  using Gtk::Menu_Helpers::MenuElem;
  
  // Set up the application menu
  // The slot we use here just causes AppWindow::hide() on this,
  // which shuts down the application.
  mMenuApp.items().push_back(MenuElem("_Reset Position", Gtk::AccelKey("i"), 
    sigc::mem_fun(mViewer, &Viewer::resetPosition)));
  mMenuApp.items().push_back(MenuElem("_Reset Orientation", Gtk::AccelKey("o"), 
    sigc::mem_fun(mViewer, &Viewer::resetOrientation)));
  mMenuApp.items().push_back(MenuElem("_Reset Joints", Gtk::AccelKey("n"), 
    sigc::mem_fun(mViewer, &Viewer::resetJoints)));
  mMenuApp.items().push_back(MenuElem("_Reset All", Gtk::AccelKey("a"), 
    sigc::mem_fun(mViewer, &Viewer::resetAll)));
  mMenuApp.items().push_back(MenuElem("_Quit", Gtk::AccelKey("q"),
    sigc::mem_fun(*this, &AppWindow::hide)));
  
  sigc::slot<bool> gameTick = sigc::mem_fun(mViewer, &Viewer::updateGame);
  Glib::signal_timeout().connect(gameTick, 50);

  mMenuEdit.items().push_back(MenuElem("_Undo", Gtk::AccelKey("u"),
    sigc::mem_fun(mViewer, &Viewer::undo)));
  mMenuEdit.items().push_back(MenuElem("_Redo", Gtk::AccelKey("r"),
    sigc::mem_fun(mViewer, &Viewer::redo)));

  sigc::slot1<void, Viewer::Mode> mode_slot = sigc::mem_fun(mViewer, &Viewer::setSelectionMode);
  mMenuMode.items().push_back(Gtk::Menu_Helpers::RadioMenuElem(mMenuGroup, 
    "_Position/Orientation", Gtk::AccelKey("p"), 
    sigc::bind( mode_slot, Viewer::MODE_ENTIRE_OBJ)));
  mMenuMode.items().push_back(Gtk::Menu_Helpers::RadioMenuElem(mMenuGroup, 
    "_Joints", Gtk::AccelKey("j"), 
    sigc::bind( mode_slot, Viewer::MODE_JOINTS)));
  
  // Set up Options Menu
  mMenuOptions.items().push_back(MenuElem("_Circle", Gtk::AccelKey("c"), 
    sigc::mem_fun(*this, &AppWindow::toggleTrackball)));
  mMenuOptions.items().push_back(MenuElem("_Z-buffer", Gtk::AccelKey("z"), 
    sigc::mem_fun(*this, &AppWindow::toggleZBuffer)));
  mMenuOptions.items().push_back(MenuElem("_Backface Culling", Gtk::AccelKey("b"), 
    sigc::mem_fun(*this, &AppWindow::toggleBackfaceCulling)));
  mMenuOptions.items().push_back(MenuElem("_Frontface Culling", Gtk::AccelKey("f"), 
    sigc::mem_fun(*this, &AppWindow::toggleFrontfaceCulling)));


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

  LabelUpdate();
  // Put the viewer below the menubar. pack_start "grows" the widget
  // by default, so it'll take up the rest of the window.
  mViewer.set_size_request(500, 500);
  mVBox.pack_start(mViewer);
  mVBox.pack_start(mToggles, Gtk::PACK_SHRINK);

  show_all();
}


void AppWindow::toggleTrackball() {
  mTrack = mViewer.toggleTrackball();
  LabelUpdate();
}

void AppWindow::toggleZBuffer() {
  mZBuff = mViewer.toggleZBuffer();
  LabelUpdate();
}

void AppWindow::toggleBackfaceCulling() {
  mBack = mViewer.toggleBackfaceCulling();
  LabelUpdate();
}

void AppWindow::toggleFrontfaceCulling() {
  mFront = mViewer.toggleFrontfaceCulling();
  LabelUpdate();
}

void AppWindow::LabelUpdate() {
  std::stringstream ss;
  ss << "Trackball: " << ((mTrack) ? "true" : "false") << std::endl;
  ss << "Z Buffer: " << ((mZBuff) ? "true" : "false") << std::endl;
  ss << "Backface Culling: " << ((mBack) ? "true" : "false") << std::endl;
  ss << "Frontface Culling: " << ((mFront) ? "true" : "false") << std::endl;

  mToggles.set_text(ss.str());
}
