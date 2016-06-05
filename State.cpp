// Copyright (C) 2016 raphael.catolino@gmail.com
#include <Arduino.h>
#include <string.h>

#include "State.h"

State::State(void (*begin)(State&)) {
  this->next = begin;
}
