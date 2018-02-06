#include "appbook.h"
#include <iostream>

AppBook::AppBook()
    : prev_button("Previous"), 
    next_button("Next"),
    quit_button("Quit")
    
{
    set_show_tabs(false);

    add(bbox);

    bbox.set_layout(Gtk::BUTTONBOX_EDGE);
    bbox.set_size_request(-1, 100);

    bbox.pack_start(prev_button, FALSE, FALSE, 0);
    bbox.pack_start(next_button, FALSE, FALSE, 0);
    bbox.pack_start(quit_button, FALSE, FALSE, 0);
    
    prev_button.signal_clicked().connect(sigc::mem_fun(*this,
    &AppBook::on_prev));

    next_button.signal_clicked().connect(sigc::mem_fun(*this,
    &AppBook::on_next));

    quit_button.signal_clicked().connect(sigc::mem_fun(*this,
    &AppBook::on_quit));
}

AppBook::~AppBook()
{
}

void AppBook::on_prev()
{
    std::cout << "Back to the future" << std::endl;
}

void AppBook::on_next()
{
    std::cout << "Here we goooooo...." << std::endl;
}

void AppBook::on_quit()
{
    std::cout << "I'm sorry Dave, I'm afraid I can't let you do that" << std::endl;
}
