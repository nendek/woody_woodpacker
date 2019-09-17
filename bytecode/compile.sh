#!/bin/sh
nasm -f elf32 bytecode32.s
nasm -f elf64 bytecode64.s
ld -s -m elf_i386 -o exe32 bytecode32.o
ld -s -o exe64 bytecode64.o
rm bytecode32.o
rm bytecode64.o
printf "\"" > bytecode64
printf "\"" > bytecode32
(for i in $(objdump -d exe64 |grep "^ " |cut -f2); do echo -n 'x'$i; done;) >> bytecode64
(for i in $(objdump -d exe32 |grep "^ " |cut -f2); do echo -n 'x'$i; done;) >> bytecode32
printf "\";\n" >> bytecode64
printf "\";\n" >> bytecode32
rm exe32
rm exe64
sed -i 's/x/\\x/g' bytecode64
sed -i 's/x/\\x/g' bytecode32
