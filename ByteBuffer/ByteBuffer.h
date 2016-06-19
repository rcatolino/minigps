// Copyright (C) 2016 raphael.catolino@gmail.com
#ifndef ByteBuffer_H
#define ByteBuffer_H

//#include <Arduino.h>
#include <stdio.h>
#include <string.h>

template <size_t N>
class ByteBuffer {
  public:

    ByteBuffer() : buffer_size(N) {}

    ByteBuffer(const char (&other)[N+1]) : buffer_size(N) {
      memcpy(buffer, other, N);
      buffer[N] = '\0';
      len = N;
    }

    ByteBuffer(const ByteBuffer<N>& other) : buffer_size(N) {
      memcpy(buffer, other.buffer, other.len);
      buffer[other.len] = '\0';
      len = other.len;
    }

    bool push(const ByteBuffer &piece) {
      if (piece.len + len > buffer_size) {
        return false;
      }

      memcpy(buffer+len, piece.buffer, piece.len);
      buffer[len + piece.len] = '\0';
      len += piece.len;
      return true;
    }

    bool push(char piece) {
      if (len + 1 > buffer_size) {
        return false;
      }

      buffer[len] = piece;
      buffer[len + 1] = '\0';
      len += 1;
      return true;
    }

    /*
    bool push(const String &piece) {
      if (len + piece.length() > buffer_size) {
        return false;
      }

      memcpy(buffer+len, piece.c_str(), piece.len());
      buffer[len + piece.len()] = '\0';
      len += piece.len();
      return true;
    }
    */

    template <size_t n>
    bool push(const char (&piece)[n]) {
      size_t plen = n - 1;
      if (len + plen > buffer_size) {
        printf("current length : %d, buffer size : %d, piece length %d\n", len, buffer_size, plen);
        return false;
      }

      memcpy(buffer+len, piece, plen);
      buffer[len + plen] = '\0';
      len += plen;
      return true;
    }

    size_t maxSize() {
      return buffer_size;
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

      return (strncmp(buffer+len-suffix.len, suffix.buffer, suffix.len) == 0);
    }

    bool concat(ByteBuffer &stuff) {
      if (push(stuff) == -1) {
        return false;
      }
      return true;
    }

    const char *c_str() {
      return (char*) buffer;
    }

  private:
    char buffer[N+1];
    const size_t buffer_size;
    unsigned int len = 0;
};

template <size_t n>
static ByteBuffer<n> makebb(ByteBuffer<n>& other) {
  return ByteBuffer<n> (other);
}

template <size_t n>
static ByteBuffer<n-1> makebb(const char (&other)[n]) {
  return ByteBuffer<n-1> (other);
}

#endif // ByteBuffer_H