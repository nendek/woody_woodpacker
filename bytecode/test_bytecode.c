/*
 *compile: gcc -fno-stack-protector -z execstack -o test_bytecode test_bytecode.c
 */

char bytecode[] = "\xeb\x1e\x5e\xb8\x01\x00\x00\x00\xbf\x01\x00\x00\x00\xba\x0e\x00\x00\x00\x0f\x05\xb8\x3c\x00\x00\x00\xbf\x00\x00\x00\x00\x0f\x05\xe8\xdd\xff\xff\xff\x2e\x2e\x2e\x2e\x57\x4f\x4f\x44\x59\x2e\x2e\x2e\x2e\x0a";
int main(int argc, char **argv)
{
  int (*func)();
  func = (int (*)()) bytecode;
  (int)(*func)();
}
