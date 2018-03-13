#include<sstream>

#include "pages.h"
#include <gtkmm/separator.h>

// This tells compiler to allocate InfoCont struct, but not sure why I can't do
// it in the constructor
InfoCont Page::info;

Page::Page(const Glib::ustring &t) : title(t)
{
    set_orientation(Gtk::Orientation::ORIENTATION_VERTICAL);
    set_margin_start(30);
    set_margin_end(30);
    set_margin_bottom(50);
    set_margin_top(20);
}
Page::~Page() {}

WelcomePage::WelcomePage()
    : Page("CCTC Imaging Toolkit"),
    welcome_label("Welcome to the First Responder Imaging Toolkit!", false)
{
    pack_start(welcome_label, true, true, 30);
    next_page = 1;

    info.desc = "Test";
    std::cout << "Initial value of string: " << info.desc << std::endl;
    std::cout << "Is it empty? " << info.desc.empty() << std::endl;
}

void WelcomePage::update_info()
{
    std::cout << "Welcome" << std::endl;
}

WelcomePage::~WelcomePage()
{
}

EvidPage::EvidPage()
    : Page("Evidence Device Selection"),
    evid_prompt("Please choose the evidence device"),
    evid_device_tv(false)
{
    pack_start(evid_prompt, true, true, 10);
    pack_start(evid_device_tv, true, true, 10);
    next_page = 2;
}

void EvidPage::update_info()
{
    std::cout << "Welcome" << std::endl;
}

EvidPage::~EvidPage()
{
}

TargPage::TargPage()
    : Page("Target Device Selection"),
    targ_prompt("Please choose the target device"),
    targ_device_tv(false),
    format_button("Format?")
{
    pack_start(targ_prompt, true, true, 10);
    pack_start(targ_device_tv, true, true, 10);
    pack_start(format_button, true, true, 20);

    next_page = 3;
}

void TargPage::update_info()
{
    std::cout << "Welcome" << std::endl;
}

TargPage::~TargPage()
{
}

FormatPage::FormatPage()
    : Page("Target Device Format"),
    os_box(Gtk::ORIENTATION_VERTICAL, 10),
    format_prompt("Please select which types of systems you would like to read this drive from (Linux/Windows/Apple)"),
    os_button_box(Gtk::ORIENTATION_HORIZONTAL)
{
    std::stringstream format;
    string os_names[] = {"Linux", "Windows", "Apple"};

    Gtk::Button *os_button;
    Gtk::Image *os_image;

    os_box.pack_start(format_prompt, true, true, 20);
    os_box.set_valign(Gtk::ALIGN_CENTER);

    os_button_box.set_layout(Gtk::BUTTONBOX_SPREAD);
    for (size_t i=0; i<3; i++)
    {
        format << "icons/" << os_names[i] << "_200px.svg";
        os_button = Gtk::manage(new Gtk::Button());
        os_image = Gtk::manage(new Gtk::Image(format.str()));

        os_button->set_image(*os_image);
        os_button->set_tooltip_text(os_names[i]);
        os_button_box.pack_start(*os_button);
        format.str("");
    }

    os_box.pack_start(os_button_box);
    pack_start(os_box);

    next_page = 4;
}

void FormatPage::update_info()
{
    std::cout << "Welcome" << std::endl;
}

FormatPage::~FormatPage()
{
}

CaseMetadataPage::CaseMetadataPage()
    : Page("Case Metadata"),
    grid(), frame(), swindow()
{
    grid.set_halign(Gtk::ALIGN_CENTER); 
    grid.set_valign(Gtk::ALIGN_CENTER); 
    grid.set_size_request(600, 600);
    
    grid.attach(*Gtk::manage(new Gtk::Label("Case number: ")), 0, 0, 1, 1);
    grid.attach(*Gtk::manage(new Gtk::Label("Item number: ")), 0, 1, 1, 1);
    grid.attach(*Gtk::manage(new Gtk::Label("Examiner: ")),    0, 2, 1, 1);
    grid.attach(*Gtk::manage(new Gtk::Label("Description: ")), 0, 3, 1, 1);
    grid.attach(*Gtk::manage(new Gtk::Label("Notes: ")),       0, 4, 1, 1);

    grid.attach(casenum, 1, 0, 1, 1);
    grid.attach(itemnum, 1, 1, 1, 1);
    grid.attach(examiner,1, 2, 1, 1);

    frame.set_size_request(200, 200);
    swindow.set_hexpand();
    swindow.set_border_width(3);
    
    desc.set_wrap_mode(Gtk::WRAP_WORD);
    swindow.add(desc);
    frame.add(swindow);

    grid.attach(frame, 1, 3, 1, 1);

    grid.attach(notes, 1, 4, 1, 1);

    pack_start(grid);
    next_page = 5;
}

void CaseMetadataPage::update_info()
{
    std::cout << "Welcome" << std::endl;
}

CaseMetadataPage::~CaseMetadataPage()
{
}

static Gtk::Label* new_managed_xaligned_label(const char* text)
{
    Gtk::Label *l = Gtk::manage(new Gtk::Label(text));
    l->set_xalign(0.0f);
    return l;
}

ImageMetadataPage::ImageMetadataPage()
    : Page("Image Metadata"),
    grid(), type_cb(), veri_cb(), comp_cb()
{
    // Set grid properties
    grid.set_halign(Gtk::ALIGN_CENTER);
    grid.set_valign(Gtk::ALIGN_CENTER);
    grid.set_row_spacing(20);
    grid.set_column_spacing(20);

    // To be used for labels to follow
    Gtk::Label* label;

    label = new_managed_xaligned_label(
            "Please select from one of the following:\n" 
            "- Fixed: internal drives\n"
            "- Removable: Flash drives/SD cards/External hard drives\n"
            "- Optical: CD/DVD, any kind of disc\n"
            "- Memory: RAM"
            );
    grid.attach(*label, 0, 0, 1, 1);

    type_cb.append("Fixed");
    type_cb.append("Removable");
    type_cb.append("Optical");
    type_cb.append("Memory");

    type_cb.set_active(0);
    grid.attach(type_cb, 1, 0, 1, 1);

    label = new_managed_xaligned_label(
            "Please select a verification hash:\n"
            "- SHA1: Fastest, but will not give complete confidence \n"
            "- SHA256: Use this for a real case"
            );
    grid.attach(*label, 0, 1, 1, 1);

    veri_cb.append("SHA1");
    veri_cb.append("SHA256");

    veri_cb.set_active(0);
    grid.attach(veri_cb, 1, 1, 1, 1);


    label = new_managed_xaligned_label(
            "Please select a compression type:\n"
            "- None: Fastest, image will be the size of the drive\n"
            "- Fast: Middle ground for speed and size\n"
            "- Best: Will make image as small as possible, "
                "may take significantly longer on older hardware"
            ); 
    grid.attach(*label, 0, 2, 1, 1);

    comp_cb.append("None");
    comp_cb.append("Best");
    comp_cb.append("Fast");

    comp_cb.set_active(0);
    grid.attach(comp_cb, 1, 2, 1, 1);

    pack_start(grid);
    next_page = 6;
}

void ImageMetadataPage::update_info()
{
    std::cout << "Welcome" << std::endl;
}

ImageMetadataPage::~ImageMetadataPage()
{
}

SummaryPage::SummaryPage()
    : Page("Summary"), grid()
{
    Gtk::Label *label;

    grid.set_margin_start(200);
    grid.set_margin_end(200);

    grid.set_row_spacing(30);
    grid.set_column_spacing(20);

    label = new_managed_xaligned_label("Evidence device: ");
    grid.attach(*label, 0, 0, 1, 1);

    label = new_managed_xaligned_label("Target device: ");
    grid.attach(*label, 0, 1, 1, 1);

    label = new_managed_xaligned_label("Target filesystem: ");
    grid.attach(*label, 0, 2, 1, 1);

    label = new_managed_xaligned_label("Target directory: ");
    grid.attach(*label, 0, 3, 1, 1);

    label = new_managed_xaligned_label("Target filename: ");
    grid.attach(*label, 0, 4, 1, 1);

    grid.attach(*Gtk::manage(new Gtk::Separator()), 0, 5, 2, 1); 

    label = new_managed_xaligned_label("Case number: ");
    grid.attach(*label, 0, 6, 1, 1);

    label = new_managed_xaligned_label("Item number: ");
    grid.attach(*label, 0, 7, 1, 1);

    label = new_managed_xaligned_label("Examiner: ");
    grid.attach(*label, 0, 8, 1, 1);

    label = new_managed_xaligned_label("Description: ");
    grid.attach(*label, 0, 9, 1, 1);

    label = new_managed_xaligned_label("Notes: ");
    grid.attach(*label, 0, 10, 1, 1);

    grid.attach(*Gtk::manage(new Gtk::Separator()), 0, 11, 2, 1); 

    label = new_managed_xaligned_label("Examiner: ");
    grid.attach(*label, 0, 12, 1, 1);

    label = new_managed_xaligned_label("Description: ");
    grid.attach(*label, 0, 13, 1, 1);

    label = new_managed_xaligned_label("Notes: ");
    grid.attach(*label, 0, 14, 1, 1);

    add(grid);

    next_page = 0;
}

void SummaryPage::update_info()
{
    std::cout << "Welcome" << std::endl;
}

SummaryPage::~SummaryPage()
{
}
