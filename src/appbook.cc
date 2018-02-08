#include "appbook.h"
#include <iostream>

AppBook::AppBook(Gtk::ButtonBox* b)
    : prev_button("Previous"), 
    next_button("Next") 
{
    bbox = b;
    current_page = 0;

    set_show_tabs(false);

    bbox->pack_start(prev_button, FALSE, FALSE, 0);
    bbox->pack_start(next_button, FALSE, FALSE, 0);
    
    prev_button.signal_clicked().connect(sigc::mem_fun(*this,
    &AppBook::on_prev));
    prev_button.set_sensitive(false);

    next_button.signal_clicked().connect(sigc::mem_fun(*this,
    &AppBook::on_next));

}

AppBook::~AppBook()
{
}

void AppBook::on_prev()
{
    if (pg_stack.empty()) {
        prev_button.set_sensitive(false);
        return;
    }

    current_page = pg_stack.top();
    pg_stack.pop();
    std::cout << current_page << std::endl;
}

void AppBook::on_next()
{
    pg_stack.push(current_page);
    current_page += 1;
    prev_button.set_sensitive(true);
    std::cout << current_page << std::endl;
}

