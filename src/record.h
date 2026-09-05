#pragma once

#include <cstddef>
#include <string>

#include "types.h"

struct Record {
  std::string key;
  std::string value;

  size_t size() const;
  void fill(byte_t *buf) const;
};

constexpr size_t NUM_BYTES_KEY_SIZE = 1;
constexpr size_t NUM_BYTES_VALUE_SIZE = 1;
