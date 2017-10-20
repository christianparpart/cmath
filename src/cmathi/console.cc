// This file is part of the "cmath" project, http://github.com/christianparpart/cmath>
//   (c) 2017 Christian Parpart <christian@parpart.family>
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT

#include "console.h"
#include <cmath/sysconfig.h>
#include <stdio.h>

#if HAVE_EDITLINE_READLINE_H
#warning editline used
#include <editline/readline.h>
#else
#warning readline used
#include <readline/history.h>
#include <readline/readline.h>
#endif

Readline::Readline(const std::string& historyFilename) : histfile_(historyFilename) {
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

  // TODO: only add items that aren't in there yet
  int which = history_search_prefix(line.c_str(), 0);
  if (which >= 0) {
    // HIST_ENTRY* entry = history_get(which);
    // if (entry) {
    //   printf("  line: %s\n", entry->line);
    // } else {
    //   printf("history_get(%d) = NULL\n", which);
    // }
  } else {  // if (which < 0) {
    add_history(line.c_str());
  }
}
