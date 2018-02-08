#include "appwin.h"
#include <gtkmm/application.h>

int main (int argc, char *argv[])
{
  auto app = Gtk::Application::create(argc, argv, "org.cctc.vyper");

  AppWin appwin(app);

  //Shows the window and returns when it is closed.
  return app->run(appwin);
}
