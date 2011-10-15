#include <string>

namespace os {

void touch(const std::string& path);
void make_dirs(const std::string& path);
bool delete_path(const std::string& path, bool recursive=false, bool fail_silently=false);
std::string temp_dir();

}

