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

    ByteBuffer<N>& operator=(const ByteBuffer<N>& other) {
      memcpy(buffer, other.buffer, other.len);
      buffer[other.len] = '\0';
      len = other.len;
      return *this;
    }

    template <size_t n>
    bool push(const ByteBuffer<n> &piece) {
      if (piece.length() + len > buffer_size) {
        return false;
      }

      memcpy(buffer+len, piece.c_str(), piece.length());
      buffer[len + piece.length()] = '\0';
      len += piece.length();
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
        return false;
      }

      memcpy(buffer+len, piece, plen);
      buffer[len + plen] = '\0';
      len += plen;
      return true;
    }

    size_t maxSize() const {
      return buffer_size;
    }

    size_t length() const {
      return len;
    }

    template <typename T>
    bool operator==(const T &other) {
      return equals(other);
    }

    template <typename T>
    bool operator!=(const T &other) {
      return !equals(other);
    }

    bool equals(const ByteBuffer &other) {
      return (strcmp(buffer, other.buffer) == 0);
    }

    template <size_t n>
    bool equals(const char (&other)[n]) {
      return (strcmp(buffer, other) == 0);
    }

    template <size_t n>
    bool startsWith(const char (&prefix)[n]) const {
      if (n-1 > len) {
        return false;
      }

      return (strncmp(buffer, prefix, n-1) == 0);
    }

    bool startsWith(const ByteBuffer &prefix) const {
      if (prefix.len > len) {
        return false;
      }

      return (strncmp(buffer, prefix.buffer, prefix.len) == 0);
    }

    bool endsWith(const ByteBuffer &suffix) const {
      if (suffix.len > len) {
        return false;
      }

      return (strncmp(buffer+len-suffix.len, suffix.buffer, suffix.len) == 0);
    }

    bool concat(const ByteBuffer &stuff) {
      if (push(stuff) == -1) {
        return false;
      }
      return true;
    }

    const char *c_str() const {
      return (char*) buffer;
    }

    void remove(size_t index = 0) {
      if (index >= len) {
        return;
      }

      buffer[index] = '\0';
      len = index;
    }

    size_t indexOf(char c, size_t from = 0) {
      char* pos = strchr(buffer + from, c);
      if (pos == NULL) {
        return -1;
      }
      return (size_t)(pos - buffer);
    }

    ByteBuffer<N> substring(size_t left, size_t right=N) const {
      if (left > right) {
        auto temp = right;
        right = left;
        left = temp;
      }

      if (left >= len) {
        return ByteBuffer<N>();
      }

      if (right > len) {
        right = len;
      }

      ByteBuffer<N> sub;
      memcpy(sub.buffer, buffer+left, right-left);
      sub.buffer[right-left] = '\0';
      sub.len = right-left;
      return sub;
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
