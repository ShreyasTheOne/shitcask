#pragma once
#include "shitcask.h"
#include <istream>
#include <optional>
#include <string>
#include <vector>

struct Query {
  std::string input;
  std::vector<std::string> tokens;

  void read_command();
};

class IOLoop {
private:
  std::optional<Shitcask> db_{std::nullopt};

public:
  IOLoop() = default;
  ~IOLoop();

  void run();

private:
  void use_db(const std::string &);
  void set(const std::string &, const std::string &);
  std::string get(const std::string &);
};
