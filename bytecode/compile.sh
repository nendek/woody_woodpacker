#!/bin/sh
nasm -f elf32 woody32.s
#ld -s -o exe32 woody32.o
printf "\"" > woody32
(for i in $(objdump -d woody32.o |grep "^ " |cut -f2); do echo -n 'x'$i; done;) >> woody32
printf "\";\n" >> woody32
rm woody32.o
sed -i 's/x/\\x/g' woody32

nasm -f elf32 loader32.s
#ld -s -m elf_i386 -o exe32 loader32.o
printf "\"" > loader32
(for i in $(objdump -d loader32.o |grep "^ " |cut -f2); do echo -n 'x'$i; done;) >> loader32
printf "\";\n" >> loader32
rm loader32.o
sed -i 's/x/\\x/g' loader32

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
#rm loader64.o
sed -i 's/x/\\x/g' loader64

#as -o test.o test.s
##ld -s -o test test.o
#printf "\"" > test
#(for i in $(objdump -d test.o |grep "^ " |cut -f2); do echo -n 'x'$i; done;) >> test
#printf "\";\n" >> test
#rm test.o
#sed -i 's/x/\\x/g' test
