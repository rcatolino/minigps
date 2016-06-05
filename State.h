// Copyright (C) 2016 raphael.catolino@gmail.com
#ifndef STATE_H
#define STATE_H

#include <Arduino.h>

class State {
  public:
    State(void (*begin)(State&));
    void (*next) (State&);
};

#endif // GPS_H
