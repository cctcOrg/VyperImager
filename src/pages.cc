#include <sstream>
#include <iostream>
#include <typeinfo>
#include <gtkmm.h>

#include "blockdev_liststore.h"
#include "pages.h"
#include "binary_interface.h"
#include <gtkmm/separator.h>

// This tells compiler to allocate InfoCont struct. Only happens once, not on
// every object construction
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
    next_page = EVID_PAGE;
}

bool WelcomePage::update_info()
{
    return true;
}

WelcomePage::~WelcomePage()
{
}

EvidPage::EvidPage()
    : Page("Evidence Device Selection"),
    no_device_dialog("An evidence device must be selected!"), 
    evid_prompt("Please choose the evidence device"),
    evid_device_tv(false, "null")
{
    pack_start(evid_prompt, true, true, 10);
    pack_start(evid_device_tv, true, true, 10);
    next_page = TGT_PAGE;
}

bool EvidPage::update_info()
{
    BlockdevCols cols;
    std::stringstream ss;

    Glib::RefPtr<Gtk::TreeSelection> sel;
    Glib::RefPtr<Gtk::TreeModel> model;
    Gtk::TreeIter iter;
    Gtk::TreeRow row;

    /* Get target device name */
    sel = evid_device_tv.get_selection(); 
    if ((iter = sel->get_selected(model)))
    {
        row = *iter;
        ss << "/dev/" << row[cols.device_name];
        info.evidence_device = ss.str();
        ss.str(""); 
        
        ss << "/dev/" << row[cols.device_name];
        info.evd_path = ss.str();
    } 
    else
    {
        no_device_dialog.confirm();
        return false;
    }

    if (bint::writeblock_evidence_device(info.evidence_device))
    {
        std::cerr << "Error writeblocking " + info.evidence_device << std::endl;
    }
    return true;
}

EvidPage::~EvidPage()
{
}

TargPage::TargPage()
    : Page("Target Device Selection"),
    no_device_dialog("A target device must be selected!"), 
    targ_prompt("Please choose the target device"),
    targ_device_tv(false, info.evidence_device),
    format_button("Format?")
{
    pack_start(targ_prompt, true, true, 10);
    pack_start(targ_device_tv, true, true, 10);
    pack_start(format_button, true, true, 20);

    next_page = TGTF_PAGE;
}

bool TargPage::update_info()
{
    char **cmd = nullptr;
    GSubprocess *subp = nullptr;

    BlockdevCols cols;
    std::stringstream ss;

    Glib::RefPtr<Gtk::TreeSelection> sel;
    Glib::RefPtr<Gtk::TreeModel> model;
    Gtk::TreeIter iter;
    Gtk::TreeRow row;

    /* Get target device name */
    sel = targ_device_tv.get_selection(); 
    if ((iter = sel->get_selected(model)))
    {
        row = *iter;
        ss << "/dev/" << row[cols.device_name];
        info.target_device = ss.str();
        ss.str(""); 
        
        ss << "/dev/" << row[cols.device_name];
        info.tgt_path = ss.str();
    } 
    else
    {
        no_device_dialog.confirm();
        return false;
    }

    // If the format button is not checked, mount and ignore formatting later
    if (! format_button.get_active())
    {
        cmd = bint::mount_target_device(info.target_device);
        // If it is nullptr, that means the target is already mounted
        if (cmd != nullptr)
        {
            mount_diag.dialog_area->pack_start(*Gtk::manage(new Gtk::Label("Mounting...")));

            subp = g_subprocess_newv((const gchar *const *) cmd, G_SUBPROCESS_FLAGS_NONE, NULL);
            g_subprocess_wait_async(subp, NULL, on_mount_finished, this);

            mount_diag.confirm();
        }
        info.target_filesystem = "N/A";

        next_page = LOC_PAGE; 
    }
    else
    {
        next_page = TGTF_PAGE;
    }

    return true;
}

void TargPage::on_mount_finished(GObject *source_object, GAsyncResult *res, gpointer udata)
{
    (void) source_object;
    (void) res;

    TargPage *page = (TargPage*) udata;

    page->mount_diag.dialog_area->pack_start(*Gtk::manage(
                new Gtk::Label("Done!")), true, true, 20);

    page->mount_diag.spinner.stop();
    page->mount_diag.show_all();
    
    page->mount_diag.complete();
}

TargPage::~TargPage()
{
}

FormatPage::FormatPage()
    : Page("Target Device Format"),
    no_format_dialog("Please select which systems the target drive should be readable by."), 
    confirm_dialog("All data on selected device will be lost, are you sure you wish to continue?"),
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
        format << "/usr/local/icons/" << os_names[i] << "_200px.svg";
        os_button = Gtk::manage(new Gtk::ToggleButton());
        os_image = Gtk::manage(new Gtk::Image(format.str()));

        os_button->set_image(*os_image);
        os_button->set_tooltip_text(os_names[i]);
        os_button_box.pack_start(*os_button);
        format.str("");
    }

    os_box.pack_start(os_button_box);
    pack_start(os_box);

    next_page = LOC_PAGE;
}

bool FormatPage::update_info()
{
    size_t buttons_state = 0;
    string fs_choice;
    Gtk::ToggleButton *b;

    size_t i = 0;
    for (Gtk::Widget* w : os_button_box.get_children())
    {
        b = dynamic_cast<Gtk::ToggleButton*>(w);
        if (b->get_active())
            buttons_state |= 1<<i;
        i++;
    }
    
    switch (buttons_state)
    {
        /* Windows, Mac, Linux */
        case 0b011:
        case 0b111:
        case 0b110:
        case 0b010:
            fs_choice = "ntfs";
            break;
        /* Mac, Linux */
        case 0b100:
        case 0b101:
            fs_choice = "hfsplus";
            break;
        /* Linux */
        case 0b001:
            fs_choice = "ext4";
            break;
        default:
            no_format_dialog.confirm();
            return false;
    }

    info.target_filesystem = fs_choice; 
    return true;
}

FormatPage::~FormatPage()
{
}

TargetLocPage::TargetLocPage()
    : Page("Target Location"), grid(), filename(), directory(Gtk::FILE_CHOOSER_ACTION_SELECT_FOLDER)
{
    Gtk::Label *label;

    grid.set_halign(Gtk::ALIGN_CENTER);
    grid.set_valign(Gtk::ALIGN_CENTER);
    
    label = Gtk::manage(new Gtk::Label("Filename (no extension): "));
    grid.attach(*label, 0, 0, 1, 1);

    grid.attach(filename, 1, 0, 1, 1);

    label = Gtk::manage(new Gtk::Label("Directory: "));
    grid.attach(*label, 0, 1, 1, 1);

    directory.set_current_folder("/media/EVID_TARGET");
    grid.attach(directory, 1, 1, 1, 1);

    pack_start(grid);

    next_page = CASE_PAGE;
}

bool TargetLocPage::update_info()
{
    info.target_filename = filename.get_text();
    info.target_directory = directory.get_current_folder();
    return true;
}

TargetLocPage::~TargetLocPage()
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
    next_page = IMGI_PAGE;
}

bool CaseMetadataPage::update_info()
{
    Glib::RefPtr<Gtk::TextBuffer> buff;

    info.casenum = casenum.get_text();
    info.itemnum = itemnum.get_text();
    info.examiner = examiner.get_text();
    info.notes = notes.get_text();

    buff = desc.get_buffer();
    info.desc = buff->get_text();
    return true;
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
    next_page = SUMM_PAGE;
}

bool ImageMetadataPage::update_info()
{
    info.device_type = type_cb.get_active_text(); 
    info.hash_type = veri_cb.get_active_text();
    info.compression_type = comp_cb.get_active_text();
    return true;
}

ImageMetadataPage::~ImageMetadataPage()
{
}

SummaryPage::SummaryPage()
    : Page("Summary"), grid()
{
    Gtk::Label *label;

    grid.set_halign(Gtk::ALIGN_CENTER);
    grid.set_valign(Gtk::ALIGN_CENTER);
    grid.set_margin_start(200);
    grid.set_margin_end(200);

    grid.set_row_spacing(30);
    grid.set_column_spacing(20);

    label = new_managed_xaligned_label("Evidence device: ");
    grid.attach(*label, 0, 0, 1, 1);
    grid.attach(eviddev, 1, 0, 1, 1);

    label = new_managed_xaligned_label("Target device: ");
    grid.attach(*label, 0, 1, 1, 1);
    grid.attach(targdev, 1, 1, 1, 1);

    label = new_managed_xaligned_label("Target filesystem: ");
    grid.attach(*label, 0, 2, 1, 1);
    grid.attach(targfisys, 1, 2, 1, 1);

    label = new_managed_xaligned_label("Target directory: ");
    grid.attach(*label, 0, 3, 1, 1);
    grid.attach(targdir, 1, 3, 1, 1);

    label = new_managed_xaligned_label("Target filename: ");
    grid.attach(*label, 0, 4, 1, 1);
    grid.attach(targfiname, 1, 4, 1, 1);

    grid.attach(*Gtk::manage(new Gtk::Separator()), 0, 5, 2, 1); 

    label = new_managed_xaligned_label("Case number: ");
    grid.attach(*label, 0, 6, 1, 1);
    grid.attach(casenum, 1, 6, 1, 1);

    label = new_managed_xaligned_label("Item number: ");
    grid.attach(*label, 0, 7, 1, 1);
    grid.attach(itemnum, 1, 7, 1, 1);

    label = new_managed_xaligned_label("Examiner: ");
    grid.attach(*label, 0, 8, 1, 1);
    grid.attach(examiner, 1, 8, 1, 1);

    label = new_managed_xaligned_label("Description: ");
    grid.attach(*label, 0, 9, 1, 1);
    grid.attach(desc, 1, 9, 1, 1);

    label = new_managed_xaligned_label("Notes: ");
    grid.attach(*label, 0, 10, 1, 1);
    grid.attach(notes, 1, 10, 1, 1);

    grid.attach(*Gtk::manage(new Gtk::Separator()), 0, 11, 2, 1); 

    label = new_managed_xaligned_label("Device Type: ");
    grid.attach(*label, 0, 12, 1, 1);
    grid.attach(devtype, 1, 12, 1, 1);

    label = new_managed_xaligned_label("Compression Type: ");
    grid.attach(*label, 0, 13, 1, 1);
    grid.attach(comptype, 1, 13, 1, 1);

    label = new_managed_xaligned_label("Hash Type: ");
    grid.attach(*label, 0, 14, 1, 1);
    grid.attach(hashtype, 1, 14, 1, 1);

    pack_start(grid);

    next_page = WELCOME_PAGE;

}

bool SummaryPage::update_info()
{
    eviddev.set_text(info.evidence_device);
    targdev.set_text(info.target_device);
    targfisys.set_text(info.target_filesystem);
    targdir.set_text(info.target_directory);
    targfiname.set_text(info.target_filename);

    casenum.set_text(info.casenum);
    itemnum.set_text(info.itemnum);
    examiner.set_text(info.examiner);
    desc.set_text(info.desc);
    notes.set_text(info.notes);

    devtype.set_text(info.device_type);
    comptype.set_text(info.compression_type);
    hashtype.set_text(info.hash_type);

    return true;
}

void SummaryPage::on_mkfs_finished(GObject *source_object, GAsyncResult *res, gpointer udata)
{
    (void) source_object;
    (void) res;

    GSubprocess *subp;
    char **cmd;
    SummaryPage *page = (SummaryPage*) udata;

    page->mount_diag.dialog_area->pack_start(*Gtk::manage(new Gtk::Label("Mounting...")));
    page->mount_diag.show_all();

    cmd = bint::mount_target_device(page->info.target_device);
    /* mount_target_device may return NULL, but if it does we still want the 
     * final callback to execute -- so just do nothing */
    if (cmd == nullptr) {
        cmd = new char*[2];
        cmd[0] = new char[5]; 
        strcpy(cmd[0], "true"); 
        cmd[1] = NULL;
    }

    subp = g_subprocess_newv((const gchar *const *)cmd, G_SUBPROCESS_FLAGS_NONE, NULL);
    g_subprocess_wait_async(subp, NULL, on_mount_finished, page);
    g_strfreev(cmd);
}

void SummaryPage::on_mount_finished(GObject *source_object, GAsyncResult *res, gpointer udata)
{
    (void) source_object;
    (void) res;

    SummaryPage *page = (SummaryPage*) udata;

    page->mount_diag.dialog_area->pack_start(*Gtk::manage(
                new Gtk::Label("Done!")), true, true, 20);

    page->mount_diag.spinner.stop();
    page->mount_diag.show_all();
    
    page->mount_diag.complete();
}


static double get_percent(char *buffer)
{
    char *percent_complete_str = nullptr;
    char percent[3];
    char *ppercent = percent;
    long l_per;

    percent_complete_str = strstr(buffer, "s: at");
    if ( ! (percent_complete_str == nullptr || *percent_complete_str == '\0') )
    {
        /* Offset to start reading in number */
        percent_complete_str += 6;

        while (*percent_complete_str != '%')
        {
            *(ppercent++) = *percent_complete_str;
            percent_complete_str++;
        }
        percent[2] = '\0';

        l_per = strtol(percent, NULL, 10);
        return l_per/100.0;
    }

    return 0.0;
}

bool SummaryPage::image()
{
    char **cmd = nullptr;

    // Oh, we haven't formatted yet? 
    if (info.target_filesystem.compare("N/A") != 0)
    {
        bint::partition_device(info.target_device, info.target_filesystem);

        cmd = bint::format_target_device(info.target_device, info.target_filesystem);
        subp = g_subprocess_newv((const gchar *const *)cmd, G_SUBPROCESS_FLAGS_NONE, NULL);
        g_subprocess_wait_async(subp, NULL, on_mkfs_finished, this);
        g_strfreev(cmd);
        
        mount_diag.confirm();
    }

    char so_buf[274];
    GInputStream *std_out = NULL;
    cmd = bint::create_forensic_image(info);
    //std::cout << g_strjoinv(" ", cmd) << std::endl; 

    prog_diag.dialog_area->pack_start(*Gtk::manage(new Gtk::Label("Imaging...")),
            true, true, 10);
    prog_diag.show_all();

    subp = g_subprocess_newv((const gchar *const *)cmd, 
            G_SUBPROCESS_FLAGS_STDOUT_PIPE, NULL);
            //G_SUBPROCESS_FLAGS_NONE, NULL);
    std_out = g_subprocess_get_stdout_pipe(subp);

    g_input_stream_read_async(std_out, so_buf, 274, G_PRIORITY_DEFAULT, 
            NULL, on_status_read, this);

    g_subprocess_wait_async(subp, NULL, imaging_done, this);
    g_strfreev(cmd);

    prog_diag.confirm();

    return true;
}

void SummaryPage::on_status_read(GObject *cmd_std, GAsyncResult *r, gpointer udata)
{
    GInputStream *std_out = G_INPUT_STREAM(cmd_std);
    SummaryPage *page = (SummaryPage*) udata;
    (void) r;

    char so_buf[274];
    double prog_val = 0.0;

    if (g_subprocess_get_if_exited(page->subp) == true)
    {
        return;
    }

    g_usleep(1*G_USEC_PER_SEC);
    g_input_stream_read_async(std_out, so_buf, 274, G_PRIORITY_DEFAULT, 
            NULL, on_status_read, page);

    so_buf[273] = '\0';
    std::cout << "Ran callback, got \"" << so_buf << "\"" << std::endl;

    prog_val = get_percent(so_buf);
    page->prog_diag.set_progress(prog_val);
    std::cout << "Progress bar at " << prog_val << std::endl;
}

void SummaryPage::imaging_done(GObject *cmd_std, GAsyncResult *r, gpointer udata)
{
    (void) cmd_std;
    (void) r;

    SummaryPage *page = (SummaryPage*) udata;
    page->prog_diag.complete();
}

SummaryPage::~SummaryPage()
{
}
