#include "pages.h"

Page::Page(const Glib::ustring &t) : title(t)
{
}
Page::~Page() {}

WelcomePage::WelcomePage()
    : Page("CCTC Imaging Toolkit")
{
    ;
}

WelcomePage::~WelcomePage()
{
}

EvidPage::EvidPage()
    : Page("CCTC Imaging Toolkit")
{
    ;
}

EvidPage::~EvidPage()
{
}
