#pragma once

#include "types.h"
#include <cstddef>
#include <sys/_types/_off_t.h>

size_t safe_read(int fd, byte_t *buf, const size_t num_bytes);
size_t safe_write(int fd, const byte_t *buf, const size_t num_bytes);
size_t write_at_pos(int fd, const byte_t *buf, const size_t num_bytes,
                    const off_t offset);
size_t read_at_pos(int fd, byte_t *buf, const size_t num_bytes,
                   const off_t offset);

size_t safe_lseek(int fd, off_t offset, int mode);
