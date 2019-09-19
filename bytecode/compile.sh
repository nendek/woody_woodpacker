#!/bin/sh
nasm -f elf32 bytecode32.s
#ld -s -m elf_i386 -o exe32 bytecode32.o
printf "\"" > bytecode32
(for i in $(objdump -d bytecode32.o |grep "^ " |cut -f2); do echo -n 'x'$i; done;) >> bytecode32
printf "\";\n" >> bytecode32
rm bytecode32.o
sed -i 's/x/\\x/g' bytecode32

nasm -f elf64 woody64.s
#ld -s -o exe64 woody64.o
printf "\"" > woody64
(for i in $(objdump -d woody64.o |grep "^ " |cut -f2); do echo -n 'x'$i; done;) >> woody64
printf "\";\n" >> woody64
rm woody64.o
sed -i 's/x/\\x/g' woody64

nasm -f elf64 loader64.s
#ld -s -o exe64 loader64.o
printf "\"" > loader64
(for i in $(objdump -d loader64.o |grep "^ " |cut -f2); do echo -n 'x'$i; done;) >> loader64
printf "\";\n" >> loader64
rm loader64.o
sed -i 's/x/\\x/g' loader64

#as -o test.o test.s
##ld -s -o test test.o
#printf "\"" > test
#(for i in $(objdump -d test.o |grep "^ " |cut -f2); do echo -n 'x'$i; done;) >> test
#printf "\";\n" >> test
#rm test.o
#sed -i 's/x/\\x/g' test
