#include "io_loop.h"
#include <iostream>
#include <sstream>
#include <string>

/**********
 * Query
 */

void Query::read_command() {
  std::getline(std::cin, input);

  if (input.size() == 0)
    return;

  std::istringstream iss{input};
  std::string token;
  while (iss >> token)
    tokens.push_back(std::move(token));
}

/**********
 * IOLoop
 */

IOLoop::~IOLoop() {
  if (db_) {
    db_->close_connection();
  }
}

void IOLoop::run() {
  while (true) {
    Query query{};
    query.read_command();

    if (query.tokens.size() == 0) {
      continue;
    }

    const std::string_view command = query.tokens[0];
    if (command == "db") {
      use_db(query.tokens[1]);
    } else if (command == "set") {
      const std::string &key = query.tokens[1];
      const std::string &value = query.tokens[2];
      set(key, value);
    } else if (command == "get") {
      const std::string &key = query.tokens[1];
      std::cout << get(key) << "\n";
    } else {
      std::cout << "Invalid command\n";
    }
  }
}

void IOLoop::use_db(const std::string &db_name) { db_.emplace(db_name); }

void IOLoop::set(const std::string &key, const std::string &value) {
  if (!db_) {
    std::cout << "No database connection open\n";
    return;
  }
  db_->set(key, value);
}

std::string IOLoop::get(const std::string &key) {
  if (!db_) {
    std::cout << "No database connection open\n";
    return "";
  }
  return db_->get(key);
}
