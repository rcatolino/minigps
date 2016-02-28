#ifndef Soft_H
#define Soft_H

#include <Arduino.h>

template<size_t N>
int sendCommand(const String& cmd, String (&results)[N]);

#endif // Soft_H
