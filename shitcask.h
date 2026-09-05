#pragma once

#include <string>
#include <unordered_map>

#include "record.h"

const int FD_UNSET = -2;

class Shitcask {
  int fd_tail_{FD_UNSET};
  int fd_read_{FD_UNSET};
  std::string database_name_{};
  std::unordered_map<std::string, off_t> offsets_{};

public:
  Shitcask(std::string);
  ~Shitcask();
  void set(std::string, std::string);
  std::string get(std::string);

private:
  std::string database_filename();
  off_t get_next_offset();
  void load_all_records();
  Record read_record_at_offset(const off_t offset);
};
