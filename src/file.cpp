#include <cerrno>
#include <cstdlib>
#include <fcntl.h>
#include <iostream>
#include <stdexcept>
#include <string>
#include <sys/fcntl.h>
#include <unistd.h>

#include "file.h"
#include "types.h"

size_t safe_read(int fd, byte_t *buf, const size_t num_bytes) {
  size_t total_bytes_read = 0;
  while (total_bytes_read < num_bytes) {
    int bytes_read =
        read(fd, buf + total_bytes_read, num_bytes - total_bytes_read);
    if (bytes_read == -1) {
      if (errno == EINTR) {
        continue;
      } else {
        std::cerr << "Failed to read bytes from file\n";
        std::exit(1);
      }
    } else if (bytes_read == 0) {
      break;
    } else {
      total_bytes_read += bytes_read;
    }
  }
  return total_bytes_read;
  // std::cout << "Read " << num_bytes << " bytes from fd: " << fd << "\n";
  // std::cout << "Read: " << buf << "\n";
}

size_t read_at_pos(int fd, byte_t *buf, const size_t num_bytes,
                   const off_t offset) {
  const off_t pos = lseek(fd, offset, SEEK_SET);
  if (pos == -1) {
    std::cerr << "Failed to seek to offset " << offset << " in file " << fd
              << "\n";
    std::exit(1);
  }
  return safe_read(fd, buf, num_bytes);
}

size_t safe_write(int fd, const byte_t *buf, const size_t num_bytes) {
  size_t total_bytes_written = 0;
  while (total_bytes_written < num_bytes) {
    // NOTE: Call to write() returns immediately
    // but the bytes are stored in a kernel buffer
    // for a brief period until being flushed out to disk
    int bytes_written =
        write(fd, buf + total_bytes_written, num_bytes - total_bytes_written);
    if (bytes_written == -1) {
      if (errno == EINTR) {
        continue;
      } else {
        throw std::runtime_error("Failed to write bytes to file\n");
        std::exit(1);
      }
    } else {
      total_bytes_written += bytes_written;
    }
    // NOTE: fdatasync note not available on macos
    int ret = fsync(fd);
    if (ret == -1) {
      throw std::runtime_error("Failed to fsync on fd: " + std::to_string(fd) +
                               "\n");
    }
  }
  return total_bytes_written;
  // std::cout << "Wrote " << num_bytes << " bytes to fd: " << fd << "\n";
  // std::cout << "Wrote: " << buf << "\n";
}

size_t write_at_pos(int fd, const byte_t *buf, const size_t num_bytes,
                    const off_t offset) {
  const off_t pos = lseek(fd, offset, SEEK_SET);
  if (pos == -1) {
    std::cerr << "Failed to seek to offset " << offset << " in file " << fd
              << "\n";
    std::exit(1);
  }
  return safe_write(fd, buf, num_bytes);
}

size_t safe_lseek(int fd, off_t offset, int mode) {
  const auto ret = lseek(fd, offset, mode);
  if (ret == -1) {
    throw std::runtime_error("Failed to seek fd: " + std::to_string(fd) +
                             ", offset: " + std::to_string(offset) +
                             " mode: " + std::to_string(mode));
  }
  return ret;
}
