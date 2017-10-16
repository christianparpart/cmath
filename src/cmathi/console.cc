// This file is part of the "cmath" project, http://github.com/christianparpart/cmath>
//   (c) 2017 Christian Parpart <christian@parpart.family>
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT

#include "console.h"
#include <readline/history.h>
#include <readline/readline.h>
#include <stdio.h>

Readline::Readline(const std::string& historyFilename) : histfile_(historyFilename) {
  history_write_timestamps = 1;
  using_history();
  read_history(historyFilename.c_str());
}

Readline::~Readline() {
  write_history(histfile_.c_str());
}

std::tuple<bool, std::string> Readline::getline(const char* prompt) {
  char* line = readline(prompt);
  if (!line)
    return {true, ""};

  if (*line)
    addHistory(line);

  return {false, line};
}

void Readline::addHistory(const std::string& line) {
  if (line.empty())
    return;

  int which = history_search_pos(line.c_str(), 1, 0);
  if (which < 0) {
    add_history(line.c_str());
  } else {
    add_history_time(line.c_str());
  }
}
