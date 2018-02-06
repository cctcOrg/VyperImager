#include "appwin.h"
#include <iostream>

AppWin::AppWin()
    : app_box(Gtk::ORIENTATION_VERTICAL, 0)
{
    fullscreen();

    add(app_box);

    app_box.pack_start(hb, FALSE, FALSE, 0);
    app_box.pack_end(notebook, TRUE, TRUE, 0);
    
    hb.set_title("Evidence and Target Devices");

    //m_button.signal_clicked().connect(sigc::mem_fun(*this,
    //&AppWin::on_button_clicked));

    //// This packs the button into the Window (a container).
    //add(m_button);

    //// The final step is to display this newly created widget...
    //m_button.show();
    show_all();
}

AppWin::~AppWin()
{
}

//void AppWin::on_button_clicked()
//{
//std::cout << "Don't do that" << std::endl;
//}
