#include "record.h"
#include <cstddef>

size_t Record::size() const {
  return NUM_BYTES_KEY_SIZE + NUM_BYTES_VALUE_SIZE + key.size() + value.size();
}

void Record::fill(byte_t *buf) const {
  buf[0] = key.size();
  buf[1] = value.size();

  // TODO: Casting char to unsigned char can cause
  // unwanted behaviour if char represents negative value

  size_t current_index = 2;
  for (int i = 0; i < key.size(); i++) {
    buf[current_index++] = key[i];
  }
  for (int i = 0; i < value.size(); i++) {
    buf[current_index++] = value[i];
  }
}
