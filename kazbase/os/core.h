#pragma once

#ifndef OS_CORE_H
#define OS_CORE_H

#include "../unicode.h"

namespace os {

void touch(const std::string& path);
void touch(const unicode& path);

void rename(const unicode& old, const unicode& new_path);

void remove(const unicode& path);
void remove_dir(const unicode& path);
void remove_dirs(const unicode& path);

void make_dir(const unicode& path, mode_t mode=0777);
void make_dirs(const unicode& path, mode_t mode=0777);
void make_link(const unicode& source, const unicode& dest);

std::string temp_dir();

}

#endif
