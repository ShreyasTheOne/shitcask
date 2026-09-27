#include <bit>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <sys/fcntl.h>
#include <unistd.h>

#include "file.h"
#include "record.h"
#include <shitcask/shitcask.h>

Shitcask::Shitcask(std::string database_name) : database_name_(std::move(database_name)) {
  fd_tail_ = open(database_filename().c_str(), O_RDWR | O_CREAT, 0700);
  if (fd_tail_ == -1) {
    throw std::runtime_error("Failed to open file " + database_filename() + " in rdwr mode.");
  }

  const auto ret = lseek(fd_tail_, 0, SEEK_END);
  if (ret == -1) {
    throw std::runtime_error("Failed to seek to end of database file " + database_filename());
  }

  fd_read_ = open(database_filename().c_str(), O_RDONLY);
  if (fd_read_ == -1) {
    throw std::runtime_error("Failed to open file " + database_filename() + " in rdonly mode.");
  }

  load_all_records();
}

void Shitcask::load_all_records() {
  if (!is_connected()) {
    throw NoDatabaseConnection("No open database connection");
  }

  size_t bytes_read;
  alignas(Header) byte_t header_buffer[HeaderSize];
  size_t current_header_offset = 0;

  while (true) {
    current_header_offset = safe_lseek(fd_read_, 0, SEEK_CUR);
    if (bytes_read = safe_read(fd_read_, header_buffer, HeaderSize); bytes_read == 0) {
      break;
    }

    Header header = std::bit_cast<Header>(header_buffer);
    const std::string key = read_string(fd_read_, header.key_size);
    if (header.opcode == OpCode::INSERT) {
      offsets_[key] = current_header_offset;
    } else if (header.opcode == OpCode::ERASE) {
      offsets_.erase(key);
    }
  }
}

// Actions

void Shitcask::set(std::string key, std::string val) {
  if (!is_connected()) {
    throw NoDatabaseConnection("No open database connection");
  }

  Header header{//
                .opcode = OpCode::INSERT,
                .key_size = static_cast<uint8_t>(key.size()),
                .val_size = static_cast<uint8_t>(val.size())};
  const size_t buffer_size = HeaderSize + header.key_size + header.val_size;
  byte_t *buffer = new byte_t[buffer_size];

  // Capture header
  std::memcpy(buffer, &header, HeaderSize);
  std::memcpy(buffer + HeaderSize, key.c_str(), header.key_size);
  std::memcpy(buffer + HeaderSize + header.key_size, val.c_str(), header.val_size);

  try {
    off_t next_offset = get_next_offset();
    safe_write(fd_tail_, buffer, buffer_size);
    offsets_[key] = next_offset;
  } catch (const std::runtime_error &e) {
    std::cerr << "Failed to write key=" << key << " value=" << val << ": " << e.what() << "\n";
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
  safe_lseek(fd_read_, offset, SEEK_SET);

  alignas(Header) byte_t header_buffer[HeaderSize];
  safe_read(fd_read_, header_buffer, HeaderSize);
  Header header = std::bit_cast<Header>(header_buffer);

  if (header.opcode != OpCode::INSERT) {
    throw std::runtime_error("Key queried: " + key + " has entry marked as deleted in database");
  }

  const std::string key_stored = read_string(fd_read_, header.key_size);
  const std::string val_stored = read_string(fd_read_, header.val_size);

  if (key_stored != key) {
    throw std::runtime_error("Key queried: " + key + " does not match key read: " + key_stored + ". Data may be corrupted\n");
  }
  return val_stored;
}

bool Shitcask::erase(std::string key) {
  if (!is_connected()) {
    throw NoDatabaseConnection("No open database connection");
  }

  const auto itr = offsets_.find(key);
  if (itr == offsets_.end()) {
    return false;
  }

  Header header{
      //
      .opcode = OpCode::ERASE,
      .key_size = static_cast<uint8_t>(key.size()),
  };

  const size_t buffer_size = HeaderSize + header.key_size + header.val_size;
  byte_t *buffer = new byte_t[buffer_size];

  // Capture header
  std::memcpy(buffer, &header, HeaderSize);
  std::memcpy(buffer + HeaderSize, key.c_str(), header.key_size);

  try {
    off_t next_offset = get_next_offset();
    safe_write(fd_tail_, buffer, buffer_size);
    offsets_.erase(key);
  } catch (const std::runtime_error &e) {
    std::cerr << "Failed to erase key=" << key << ": " << e.what() << "\n";
  }

  return true;
}

// File descriptors

off_t Shitcask::get_next_offset() {
  if (!is_connected()) {
    throw NoDatabaseConnection("No open database connection");
  }

  return lseek(fd_tail_, 0, SEEK_END);
}

bool Shitcask::is_fd_valid(int fd) {
  return fd > 0; //  fcntl(fd, F_GETFD) != -1 || errno != EBADF;
}

bool Shitcask::is_connected() { return is_fd_valid(fd_tail_) && is_fd_valid(fd_read_); }

void Shitcask::close_connection() {
  std::cout << "Closing connection to db " << database_name_ << "\n";
  if (is_fd_valid(fd_tail_))
    close(fd_tail_);
  if (is_fd_valid(fd_read_))
    close(fd_read_);
}

std::string Shitcask::database_filename() { return database_name_ + ".db"; }

Shitcask::~Shitcask() { close_connection(); }
