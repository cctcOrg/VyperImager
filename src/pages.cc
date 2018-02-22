#include "pages.h"

Page::Page(const Glib::ustring &t) : title(t)
{
}
Page::~Page() {}

WelcomePage::WelcomePage()
    : Page("CCTC Imaging Toolkit"),
    welcome_label("Welcome to the First Responder Imaging Toolkit!", false)
{
    pack_start(welcome_label, true, true, 30);
    next_page = 1;
}

WelcomePage::~WelcomePage()
{
}

EvidPage::EvidPage()
    : Page("Evidence Device Selection")
{
    next_page = 2;
}

EvidPage::~EvidPage()
{
}
