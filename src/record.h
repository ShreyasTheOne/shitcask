#pragma once

#include <cstddef>
#include <cstdint>

enum class OpCode : uint8_t { UNKNOWN = 0, INSERT = 1, ERASE = 2 };

struct Header {
  OpCode opcode;
  uint8_t key_size;
  uint8_t val_size;
};

constexpr std::size_t HeaderSize = sizeof(Header);
