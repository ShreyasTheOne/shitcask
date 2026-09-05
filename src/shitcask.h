#pragma once

#include <stdexcept>
#include <string>
#include <unordered_map>

#include "record.h"

const int FD_UNSET = -2;

class Shitcask {
public:
  Shitcask(std::string);
  ~Shitcask();

  void set(std::string, std::string);
  std::string get(std::string);

  bool is_connected();
  void close_connection();

private:
  std::string database_filename();
  bool is_fd_valid(int);
  off_t get_next_offset();
  void load_all_records();
  Record read_record_at_offset(const off_t offset);

private:
  std::string database_name_{};
  std::unordered_map<std::string, off_t> offsets_{};

  int fd_tail_{FD_UNSET};
  int fd_read_{FD_UNSET};
};

class NoDatabaseConnection : public std::runtime_error {
public:
  // Accept either std::string or const char* and forward it to the base class
  explicit NoDatabaseConnection(const std::string &message)
      : std::runtime_error(message) {}

  explicit NoDatabaseConnection(const char *message)
      : std::runtime_error(message) {}
};
