#include "shitcask.h"
#include <iostream>

int main() {
  Shitcask store{"hello"};

  // store.set("key", "value");
  std::string value = store.get("key");

  std::cout << "Retrieved value " << value << "\n";
}
