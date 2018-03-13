#include <string>

using std::string;

typedef struct InfoCont
{
    string casenum;
    string itemnum;
    string examiner;
    string desc;
    string notes;
    string evidence_device;
    string target_device;
    string evd_path;
    string tgt_path;
    string target_filesystem;
    string target_filename;
    string target_directory;
    string device_type;
    string hash_type;
    string compression_type;
} InfoCont;
