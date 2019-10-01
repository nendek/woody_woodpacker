#!/bin/sh
nasm -f elf32 woody32.s
printf "\"" > woody32
(for i in $(objdump -d woody32.o |grep "^ " |cut -f2); do echo -n 'x'$i; done;) >> woody32
printf "\";\n" >> woody32
rm woody32.o
sed -i 's/x/\\x/g' woody32

nasm -f elf32 loader32.s
printf "\"" > loader32
(for i in $(objdump -d loader32.o |grep "^ " |cut -f2); do echo -n 'x'$i; done;) >> loader32
printf "\";\n" >> loader32
rm loader32.o
sed -i 's/x/\\x/g' loader32

nasm -f elf64 woody64.s
printf "\"" > woody64
(for i in $(objdump -d woody64.o |grep "^ " |cut -f2); do echo -n 'x'$i; done;) >> woody64
printf "\";\n" >> woody64
rm woody64.o
sed -i 's/x/\\x/g' woody64

nasm -f elf64 loader64.s
printf "\"" > loader64
(for i in $(objdump -d loader64.o |grep "^ " |cut -f2); do echo -n 'x'$i; done;) >> loader64
printf "\";\n" >> loader64
rm loader64.o
sed -i 's/x/\\x/g' loader64
