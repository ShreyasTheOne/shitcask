#pragma once

#include <cstddef>
#include <cstdint>

struct Header {
  uint8_t key_size;
  uint8_t val_size;
};

constexpr std::size_t HeaderSize = sizeof(Header);
