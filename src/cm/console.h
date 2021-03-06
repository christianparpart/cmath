// This file is part of the "cmath" project, http://github.com/christianparpart/cmath>
//   (c) 2017 Christian Parpart <christian@parpart.family>
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT

#pragma once

#include <string>

class ConsoleReader {
 public:
  virtual ~ConsoleReader() {}

  virtual bool getline(const char* prompt, std::string* line) = 0;
  virtual void addHistory(const std::string& line) = 0;
};

class Readline : public ConsoleReader {
 public:
  explicit Readline(const std::string& historyFilename);
  ~Readline();

  bool getline(const char* prompt, std::string* line) override;
  void addHistory(const std::string& line) override;

 private:
  std::string histfile_;
};
