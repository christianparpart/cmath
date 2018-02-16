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
#include <editline/readline.h>
#else
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

bool Readline::getline(const char* prompt, std::string *output) {
  char* line = readline(prompt);
  if (!line)
    return true;

  if (*line)
    addHistory(line);

  *output = line;
  return false;
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
