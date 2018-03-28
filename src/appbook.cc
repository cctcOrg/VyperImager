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
    bbox->pack_end(next_button, FALSE, FALSE, 0);

    append_page_obj(wp);
    append_page_obj(ep);
    append_page_obj(tp);
    append_page_obj(fp);
    append_page_obj(tlp);
    append_page_obj(cmdp);
    append_page_obj(imdp);
    append_page_obj(sp);

    header->set_title(wp.title);
    
    prev_button.signal_clicked().connect(sigc::mem_fun(*this,
    &AppBook::on_prev));
    prev_button.set_sensitive(false);

    next_button.signal_clicked().connect(sigc::mem_fun(*this,
    &AppBook::on_next));

    signal_switch_page().connect(sigc::mem_fun(*this, &AppBook::on_page_switch));
}

AppBook::~AppBook()
{
}

void AppBook::on_prev()
{
    current_page = pg_stack.top();
    pg_stack.pop();

    // If we just emptied it turn off prev button
    if (pg_stack.empty()) {
        prev_button.set_sensitive(false);
    }

    set_current_page(current_page);
    const Page* p = (Page*) get_nth_page(current_page);
    header->set_title(p->title);
}

void AppBook::on_next()
{
    if (current_page == SUMM_PAGE)
    {
        sp.image();
    }
    
    // Go to the next page, as specificed by the current page
    Page* p = (Page*) get_nth_page(current_page);
    
    // If the update info fails, that means the user made an error
    // NOTE: This may also update the next page, so it must be run before
    // accessing next_page to get the intended next page
    if (! p->update_info())
    {
        return;
    }

    pg_stack.push(current_page);
    current_page = p->next_page;

    set_current_page(current_page);
    prev_button.set_sensitive(true);

    const Page* pn = (Page*) get_nth_page(current_page);
    header->set_title(pn->title);
}

void AppBook::on_page_switch(Gtk::Widget *w, guint pn)
{
    (void) w;
    (void) pn;

    if (current_page == SUMM_PAGE)
    {
        next_button.set_label("Image");
        sp.update_info();
    }
    else
    {
        next_button.set_label("Next");
    }
}

int AppBook::append_page_obj(Page& p)
{
    return append_page(p, p.title);
}
