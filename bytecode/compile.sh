#!/bin/sh
nasm -f elf32 bytecode32.s
nasm -f elf64 bytecode64.s
nasm -f elf64 loader.s
as -o test.o test.s
ld -s -m elf_i386 -o exe32 bytecode32.o
ld -s -o exe64 bytecode64.o
ld -s -o exe64 loader.o
ld -s -o test test.o
printf "\"" > test
printf "\"" > loader
printf "\"" > bytecode64
printf "\"" > bytecode32
(for i in $(objdump -d test.o |grep "^ " |cut -f2); do echo -n 'x'$i; done;) >> test
(for i in $(objdump -d loader.o |grep "^ " |cut -f2); do echo -n 'x'$i; done;) >> loader
(for i in $(objdump -d bytecode64.o |grep "^ " |cut -f2); do echo -n 'x'$i; done;) >> bytecode64
(for i in $(objdump -d bytecode32.o |grep "^ " |cut -f2); do echo -n 'x'$i; done;) >> bytecode32
printf "\";\n" >> test
printf "\";\n" >> loader
printf "\";\n" >> bytecode64
printf "\";\n" >> bytecode32
rm bytecode32.o
rm bytecode64.o
rm loader.o
rm test.o
sed -i 's/x/\\x/g' test
sed -i 's/x/\\x/g' loader
sed -i 's/x/\\x/g' bytecode64
sed -i 's/x/\\x/g' bytecode32
