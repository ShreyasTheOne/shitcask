#include <cstdio>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <sys/fcntl.h>
#include <unistd.h>

#include "file.h"
#include "record.h"
#include "shitcask.h"

Shitcask::Shitcask(std::string database_name)
    : database_name_(std::move(database_name)) {
  fd_tail_ = open(database_filename().c_str(), O_RDWR | O_CREAT, 0700);
  if (fd_tail_ == -1) {
    throw std::runtime_error("Failed to open file " + database_filename() +
                             " in rdwr mode.");
  }

  const auto ret = lseek(fd_tail_, 0, SEEK_END);
  if (ret == -1) {
    throw std::runtime_error("Failed to seek to end of database file " +
                             database_filename());
  }

  fd_read_ = open(database_filename().c_str(), O_RDONLY);
  if (fd_read_ == -1) {
    throw std::runtime_error("Failed to open file " + database_filename() +
                             " in rdonly mode.");
  }

  load_all_records();
}

void Shitcask::set(std::string key, std::string value) {
  if (!is_connected()) {
    throw NoDatabaseConnection("No open database connection");
  }
  off_t next_offset = get_next_offset();
  Record record{.key = std::move(key), .value = std::move(value)};

  // Build a single buffer with entire record
  const size_t buf_size = record.size();
  byte_t *buf = new byte_t[buf_size];
  record.fill(buf);

  try {
    safe_write(fd_tail_, buf, buf_size);
    offsets_[record.key] = next_offset;
  } catch (const std::runtime_error &e) {
    std::cerr << "Failed to wrote key=" << key << " value=" << value << ": "
              << e.what() << "\n";
  }
}

std::string Shitcask::get(std::string key) {
  if (!is_connected()) {
    throw NoDatabaseConnection("No open database connection");
  }

  const auto itr = offsets_.find(key);
  if (itr == offsets_.end()) {
    return "";
  }

  const off_t offset = itr->second;
  const Record record = read_record_at_offset(offset);

  if (record.key != key) {
    throw std::runtime_error("Key queried: " + key +
                             " does not match key read: " + record.key +
                             ". Data may be corrupted\n");
  }
  return record.value;
}

Record Shitcask::read_record_at_offset(const off_t offset) {
  if (!is_connected()) {
    throw NoDatabaseConnection("No open database connection");
  }

  byte_t sizes[2];
  read_at_pos(fd_read_, sizes, NUM_BYTES_KEY_SIZE + NUM_BYTES_VALUE_SIZE,
              offset);
  const size_t key_sz = sizes[0];
  const size_t value_sz = sizes[1];
  byte_t *key_bytes = new byte_t[key_sz + 1];
  byte_t *value_bytes = new byte_t[value_sz + 1];
  safe_read(fd_read_, key_bytes, key_sz);
  safe_read(fd_read_, value_bytes, value_sz);
  key_bytes[key_sz] = '\0';
  value_bytes[value_sz] = '\0';

  char *key_chars = new char[key_sz];
  char *value_chars = new char[value_sz];

  std::memcpy(key_chars, key_bytes, key_sz + 1);
  std::memcpy(value_chars, value_bytes, value_sz + 1);

  return Record{std::string{key_chars}, std::string{value_chars}};
}

std::string Shitcask::database_filename() { return database_name_ + ".db"; }

off_t Shitcask::get_next_offset() {
  if (!is_connected()) {
    throw NoDatabaseConnection("No open database connection");
  }

  return lseek(fd_tail_, 0, SEEK_END);
}

void Shitcask::load_all_records() {
  if (!is_connected()) {
    throw NoDatabaseConnection("No open database connection");
  }

  byte_t key_sz_bytes;
  size_t key_sz;
  size_t curr_key_offset = 0;
  while (true) {
    curr_key_offset = safe_lseek(fd_read_, 0, SEEK_CUR);
    const size_t bytes_read =
        safe_read(fd_read_, &key_sz_bytes, NUM_BYTES_KEY_SIZE);

    if (bytes_read == 0) {
      break;
    }

    const Record record = read_record_at_offset(curr_key_offset);
    offsets_[record.key] = curr_key_offset;
  }
}

bool Shitcask::is_fd_valid(int fd) {
  return fd > 0; //  fcntl(fd, F_GETFD) != -1 || errno != EBADF;
}

bool Shitcask::is_connected() {
  return is_fd_valid(fd_tail_) && is_fd_valid(fd_read_);
}

void Shitcask::close_connection() {
  std::cout << "Closing connection to db " << database_name_ << "\n";
  if (is_fd_valid(fd_tail_))
    close(fd_tail_);
  if (is_fd_valid(fd_read_))
    close(fd_read_);
}

Shitcask::~Shitcask() { close_connection(); }
