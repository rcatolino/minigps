#include "ByteBuffer.h"
#include <assert.h>
#include <stdio.h>

void test_push_byte_buffer() {
  ByteBuffer<10> a;
  auto b = makebb("test");
  assert(a.push(b));
  assert(a.push(b));
  assert(!a.push(b));
  assert(a.length() == 8);
  assert(a.c_str()[8] == '\0');
  assert(strcmp(a.c_str(), "testtest") == 0);
  auto c = makebb("te");
  assert(a.push(c));
  assert(a.length() == 10);
  assert(a.c_str()[10] == '\0');
  assert(strcmp(a.c_str(), "testtestte") == 0);
}

void test_push_char() {
  ByteBuffer<3> a;
  const char *buff = a.c_str();
  assert(a.push('a'));
  assert(a.length() == 1);
  assert(buff[0] == 'a');
  assert(buff[1] == '\0');
  assert(a.push('b'));
  assert(a.push('c'));
  assert(!a.push('d'));
  assert(a.length() == 3);
  assert(strcmp(buff, "abc") == 0);
  assert(buff[3] == '\0');
  printf("test_push_char ok\n");
}

void test_push_char_star() {
  ByteBuffer<10> a;
  assert(a.push("test"));
  assert(a.length() == 4);
  const char *buff = a.c_str();
  assert(strcmp(buff, "test") == 0);
  assert(buff[4] == '\0');
  assert(a.push("data"));
  assert(a.length() == 8);
  assert(strcmp(buff, "testdata") == 0);
  assert(buff[8] == '\0');
  assert(!a.push("aaa"));
  assert(a.push("aa"));
  assert(a.length() == 10);
  assert(strcmp(buff, "testdataaa") == 0);
  assert(buff[10] == '\0');
  printf("test_push_char_start ok\n");
}

void test_char_array_constructor() {
  auto a = makebb("test");
  assert(a.length() == 4);
  assert(a.maxSize() == 4);
  assert(strcmp(a.c_str(), "test") == 0);
  printf("test_char_array_constructor ok\n");
}

void test_copy_constructor() {
  auto a = makebb("testa");
  auto b = makebb(a);
  assert(b.length() == 5);
  assert(b.maxSize() == 5);
  assert(strcmp(b.c_str(), "testa") == 0);
  printf("test_copy_constructor ok\n");
}

void test_equality() {
  auto a = makebb("testa");
  auto b = makebb("testa");
  auto c = makebb("testc");
  assert(a == b);
  assert(!(a == c));
  assert(a == "testa");
  assert(!(a == "testb"));
  assert(!(a == "atest"));
  assert(a != "testb");
  assert(a != "atest");
  assert(a != c);
  assert(!(a != b));
  printf("test_equality ok\n");
}

void test_startswith() {
  auto a = makebb("testatest");
  assert(a.startsWith("test"));
  assert(a.startsWith("testa"));
  assert(a.startsWith("testatest"));
  assert(!a.startsWith("nottest"));
  assert(!a.startsWith("testb"));
  assert(!a.startsWith("testatestb"));
  printf("test_startswith ok\n");
}

void test_endswith() {
  auto a = makebb("testatest");
  assert(a.endsWith("test"));
  assert(a.endsWith("atest"));
  assert(a.endsWith("testatest"));
  assert(!a.endsWith("nottest"));
  assert(!a.endsWith("btest"));
  assert(!a.endsWith("btestatest"));
  assert(!a.endsWith("testatestb"));
  printf("test_endswith ok\n");
}

void test_remove() {
  auto a = makebb("test");
  a.remove(5);
  assert(a == "test");
  a.remove(4);
  assert(a == "test");
  a.remove(3);
  assert(a == "tes");
  a.remove(0);
  assert(a == "");
  printf("test_remove ok\n");
}

void test_substring() {
  auto a = makebb("testtest");
  assert(a.substring(0) == "testtest");
  assert(a.substring(0, 20) == "testtest");
  assert(a.substring(0, 8) == "testtest");
  assert(a.substring(0, 7) == "testtes");
  assert(a.substring(7, 0) == "testtes");
  assert(a.substring(2) == "sttest");
  assert(a.substring(2,2) == "");
  assert(a.substring(2,4) == "st");
  assert(a.substring(2,20) == "sttest");
  printf("test_substring ok\n");
}

int test_main(int,  char *[]) {
  test_char_array_constructor();
  test_copy_constructor();
  test_push_char();
  test_push_char_star();
  test_push_byte_buffer();
  test_equality();
  test_remove();
  test_substring();
  test_startswith();
  test_endswith();
  printf("All test OK\n");
  return 0;
}
