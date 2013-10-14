#include "appwindow.hpp"
#include <iostream>

AppWindow::AppWindow() 
{
  set_title("488 Tetrominoes on the Wall");

  // A utility class for constructing things that go into menus, which
  // we'll set up next.
  using Gtk::Menu_Helpers::MenuElem;
  
  sigc::slot1<void, Viewer::Mode> mode_slot = 
    sigc::mem_fun(mViewer, &Viewer::set_mode);

  // Set up the application menu
  // The slot we use here just causes AppWindow::hide() on this,
  // which shuts down the application.
  mMenuApp.items().push_back(MenuElem("_New Game", Gtk::AccelKey("n"),
    sigc::mem_fun(mViewer, &Viewer::newGame)));
  mMenuApp.items().push_back(MenuElem("_Reset", Gtk::AccelKey("r"),
    sigc::mem_fun(mViewer, &Viewer::resetView)));
  mMenuApp.items().push_back(MenuElem("_Quit", Gtk::AccelKey("q"),
    sigc::mem_fun(*this, &AppWindow::hide)));

  mMenuMode.items().push_back(Gtk::Menu_Helpers::RadioMenuElem(mMenuGroup, "_Wire-frame", Gtk::AccelKey("w"),
    sigc::bind( mode_slot, Viewer::MODE_WIREFRAME )));
  mMenuMode.items().push_back(Gtk::Menu_Helpers::RadioMenuElem(mMenuGroup, "_Face", Gtk::AccelKey("f"),
    sigc::bind( mode_slot, Viewer::MODE_FACE )));
  mMenuMode.items().push_back(Gtk::Menu_Helpers::RadioMenuElem(mMenuGroup, "_Multicoloured", Gtk::AccelKey("m"),
    sigc::bind( mode_slot, Viewer::MODE_MULTICOLOURED )));

  sigc::slot1<void, Viewer::Difficulty> difficulty_slot = 
    sigc::mem_fun(mViewer, &Viewer::changeDifficulty);

  mMenuDifficulty.items().push_back(Gtk::Menu_Helpers::RadioMenuElem(mDifficultyGroup, "_Slow Reflexes", Gtk::AccelKey("1"),
    sigc::bind( difficulty_slot, Viewer::DIFF_EASY )));
  mMenuDifficulty.items().push_back(Gtk::Menu_Helpers::RadioMenuElem(mDifficultyGroup, "_Normal Reflexes", Gtk::AccelKey("2"),
    sigc::bind( difficulty_slot, Viewer::DIFF_MEDIUM )));
  mMenuDifficulty.items().push_back(Gtk::Menu_Helpers::RadioMenuElem(mDifficultyGroup, "_Cat-like Reflexes", Gtk::AccelKey("3"),
    sigc::bind( difficulty_slot, Viewer::DIFF_HARD )));
  mMenuDifficulty.items().push_back(Gtk::Menu_Helpers::RadioMenuElem(mDifficultyGroup, "_Inhuman Reflexes", Gtk::AccelKey("4"),
    sigc::bind( difficulty_slot, Viewer::DIFF_GODLIKE )));

  // Setting up the timer
  sigc::slot<bool> gameTick = sigc::mem_fun(mViewer, &Viewer::updateGame);
  Glib::signal_timeout().connect(gameTick, 16);

  // Set up the menu bar
  mMenubar.items().push_back(Gtk::Menu_Helpers::MenuElem("_Application", mMenuApp));
  mMenubar.items().push_back(Gtk::Menu_Helpers::MenuElem("_Draw", mMenuMode));
  mMenubar.items().push_back(Gtk::Menu_Helpers::MenuElem("_Difficulty", mMenuDifficulty));
  // Pack in our widgets
  
  // First add the vertical box as our single "top" widget
  add(mVbox);

  // Put the menubar on the top, and make it as small as possible
  mVbox.pack_start(mMenubar, Gtk::PACK_SHRINK);

  // Put the viewer below the menubar. pack_start "grows" the widget
  // by default, so it'll take up the rest of the window.
  mViewer.set_size_request(300, 600);
  mVbox.pack_start(mViewer);

  show_all();
}

bool AppWindow::on_key_press_event( GdkEventKey *ev )
{
        // This is a convenient place to handle non-shortcut
        // keys.  You'll want to look at ev->keyval.

	// An example key; delete and replace with the
	// keys you want to process
        if( ev->keyval == 't' ) {
          std::cerr << "Hello!" << std::endl;
          return true;
        } else if ( ev->keyval == GDK_Left ||
                    ev->keyval == GDK_Right || 
                    ev->keyval == GDK_Up ||
                    ev->keyval == GDK_Down ||
                    ev->keyval == GDK_space ) {
          return mViewer.keypress(ev);
        } else {
                return Gtk::Window::on_key_press_event( ev );
        }
}
