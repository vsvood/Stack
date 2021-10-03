//
// Created by vsvood on 10/2/21.
//

#ifndef STACK_LOGGER_H
#define STACK_LOGGER_H

struct Logger {
  int log_descriptor;
  static LoggerStatus Ctor(Logger* self, char* path);
  static LoggerStatus Dtor(Logger* self);
};

#endif //STACK_LOGGER_H
