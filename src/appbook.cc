#include "pages.h"
#include "appbook.h"

#include <iostream>

AppBook::AppBook(Gtk::ButtonBox* b, Gtk::HeaderBar* hb)
    : prev_button("Previous"), 
    next_button("Next") 
{
    header = hb;
    bbox = b;
    current_page = 0;

    set_show_tabs(false);

    bbox->pack_start(prev_button, FALSE, FALSE, 0);
    bbox->pack_start(next_button, FALSE, FALSE, 0);

    append_page_obj(wp);
    append_page_obj(ep);

    hb->set_title(wp.title);
    
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
    current_page = pg_stack.top();
    pg_stack.pop();
    std::cout << current_page << std::endl;

    // If we just emptied it turn off prev button
    if (pg_stack.empty()) {
        prev_button.set_sensitive(false);
    }

    set_current_page(current_page);
    const Page* p = (Page*) get_nth_page(current_page);
    std::cout << p->title << std::endl;
    header->set_title(p->title);
}

void AppBook::on_next()
{
    pg_stack.push(current_page);
    std::cout << "Pushing page " << current_page << std::endl;

    // Go to the next page, as specificed by the current page
    const Page* p = (Page*) get_nth_page(current_page);
    current_page = p->next_page;
    std::cout<< "Going to page " << current_page << std::endl;

    set_current_page(current_page);
    prev_button.set_sensitive(true);

    const Page* pn = (Page*) get_nth_page(current_page);
    header->set_title(pn->title);
}

int AppBook::append_page_obj(Page& p)
{
    append_page(p, p.title);
}
