// Copyright (C) 2016 raphael.catolino@gmail.com
#ifndef ByteBuffer_H
#define ByteBuffer_H

#include <Arduino.h>
#include <string.h>

template <size_t N>
class ByteBuffer {
  public:
    ByteBuffer() : buffer_size(N) {}
    int push(const ByteBuffer &piece) {
      if (piece.len + len > buffer_size) {
        return -1;
      }

      memcpy(buffer+len, piece.buffer, piece.len);
      buffer[len + piece.len] = '\0';
      len += piece.len;
      return 0;
    }

    int push(char piece) {
      if (len + 1 > buffer_size) {
        return -1;
      }

      buffer[len] = piece;
      buffer[len + 1] = '\0';
      len += 1;
      return 0;
    }

    int push(const String &piece) {
      if (len + piece.length() > buffer_size) {
        return -1;
      }

      memcpy(buffer+len, piece.c_str(), piece.len());
      buffer[len + piece.len()] = '\0';
      len += piece.len();
      return 0;
    }

    template <size_t n>
    int push(const char (&piece)[n]) {
      size_t plen = n - 1;
      if (len + plen > buffer_size) {
        return -1;
      }

      memcpy(buffer+len, piece, plen);
      buffer[len + plen] = '\0';
      len += plen;
      return 0;
    }

    size_t length() {
      return len;
    }

    bool operator==(ByteBuffer &other) {
      return (strcmp(buffer, other.buffer) == 0);
    }

    bool startsWith(ByteBuffer &prefix) {
      if (prefix.len > len) {
        return false;
      }

      return (strncmp(buffer, prefix.buffer, prefix.len) == 0);
    }

    bool endsWith(ByteBuffer &suffix) {
      if (suffix.len > len) {
        return false;
      }

      return (strncmp(buffer+len-prefix.len, prefix.buffer, prefix.len) == 0);
    }

  private:
    char buffer[N+1];
    const size_t buffer_size;
    unsigned int len;
};

#endif // ByteBuffer_H
