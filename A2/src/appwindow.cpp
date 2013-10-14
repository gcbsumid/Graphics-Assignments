#include "appwindow.hpp"
#include <sstream>

AppWindow::AppWindow() 
  : mViewer(this)
{
  set_title("CS488 Assignment Two");

  // A utility class for constructing things that go into menus, which
  // we'll set up next.
  using Gtk::Menu_Helpers::MenuElem;
  
  // Set up the application menu
  // The slot we use here just causes AppWindow::hide() on this,
  // which shuts down the application.
  sigc::slot1<void, Viewer::Mode> mode_slot = 
    sigc::mem_fun(mViewer, &Viewer::set_mode);

  mMenuApp.items().push_back(MenuElem("_Quit", Gtk::AccelKey("q"),
    sigc::mem_fun(*this, &AppWindow::hide)));
    mMenuApp.items().push_back(MenuElem("_Reset", Gtk::AccelKey("a"),
    sigc::mem_fun(mViewer, &Viewer::reset_view)));
  
  mMenuInteraction.items().push_back(Gtk::Menu_Helpers::RadioMenuElem(mMenuInteractionGroup, "_Eye Translation", Gtk::AccelKey("n"), sigc::bind( mode_slot, Viewer::MODE_EYE_TRANSLATION )));
  mMenuInteraction.items().push_back(Gtk::Menu_Helpers::RadioMenuElem(mMenuInteractionGroup, "_Eye Rotation", Gtk::AccelKey("o"), sigc::bind( mode_slot, Viewer::MODE_EYE_ROTATION )));
  mMenuInteraction.items().push_back(Gtk::Menu_Helpers::RadioMenuElem(mMenuInteractionGroup, "_Eye Perspective", Gtk::AccelKey("p"), sigc::bind( mode_slot, Viewer::MODE_EYE_PERSPECTIVE )));
  mMenuInteraction.items().push_back(Gtk::Menu_Helpers::RadioMenuElem(mMenuInteractionGroup, "_Model Rotation", Gtk::AccelKey("r"), sigc::bind( mode_slot, Viewer::MODE_MODEL_ROTATION )));
  mMenuInteraction.items().push_back(Gtk::Menu_Helpers::RadioMenuElem(mMenuInteractionGroup, "_Model Translation", Gtk::AccelKey("t"), sigc::bind( mode_slot, Viewer::MODE_MODEL_TRANSLATION )));
  mMenuInteraction.items().push_back(Gtk::Menu_Helpers::RadioMenuElem(mMenuInteractionGroup, "_Model Scale", Gtk::AccelKey("s"), sigc::bind( mode_slot, Viewer::MODE_MODEL_SCALE )));
  mMenuInteraction.items().push_back(Gtk::Menu_Helpers::RadioMenuElem(mMenuInteractionGroup, "_Viewport Mode", Gtk::AccelKey("v"), sigc::bind( mode_slot, Viewer::MODE_VIEWPORT)));


  // Set up the menu bar
  mMenuBar.items().push_back(Gtk::Menu_Helpers::MenuElem("_Application", mMenuApp));
  mMenuBar.items().push_back(Gtk::Menu_Helpers::MenuElem("_Mode", mMenuInteraction));
  
  // Pack in our widgets
  
  // First add the vertical box as our single "top" widget
  add(mVBox);

  // Put the menubar on the top, and make it as small as possible
  mVBox.pack_start(mMenuBar, Gtk::PACK_SHRINK);

  // Put the viewer below the menubar. pack_start "grows" the widget
  // by default, so it'll take up the rest of the window.
  mViewer.set_size_request(300, 300);
  mVBox.pack_start(mViewer);
  mVBox.pack_start(mCurrentMode, Gtk::PACK_SHRINK);
  mVBox.pack_start(mPlaneValue, Gtk::PACK_SHRINK);

  show_all();
}

void AppWindow::LabelUpdates(Viewer::Mode mode, double nearPlaneValue, double farPlaneValue) {

  switch (mode) {
    case Viewer::MODE_EYE_ROTATION:
      mCurrentMode.set_text("Current Mode: Eye Rotation");
      break;
    case Viewer::MODE_EYE_TRANSLATION:
      mCurrentMode.set_text("Current Mode: Eye Translation");
      break;
    case Viewer::MODE_EYE_PERSPECTIVE:
      mCurrentMode.set_text("Current Mode: Eye Perspective");
      break;
    case Viewer::MODE_MODEL_ROTATION:
      mCurrentMode.set_text("Current Mode: Model Rotation");
      break;
    case Viewer::MODE_MODEL_TRANSLATION:
      mCurrentMode.set_text("Current Mode: Model Translation");
      break;
    case Viewer::MODE_MODEL_SCALE:
      mCurrentMode.set_text("Current Mode: Model Scale");
      break;
    case Viewer::MODE_VIEWPORT:
      mCurrentMode.set_text("Current Mode: Viewport Mode");
      break;
    default:
      std::cerr << "Error: Invalid Mode when updating labels." << std::endl;
  }

  std::stringstream ss;
  ss << "Near Plane Value: " << nearPlaneValue << std::endl;
  ss << "Far Plane Value: " << farPlaneValue << std::endl;
  mPlaneValue.set_text(ss.str()); 
}

