#ifndef WOODY_H
# define WOODY_H

# include <sys/mman.h>
# include <elf.h>
# include <fcntl.h>
# include <sys/stat.h>
# include <unistd.h>
# include <stdlib.h>
# include <stdio.h>

# define ELF_32 1
# define ELF_64 2

extern const char shellcode32[];
extern char jmp32[];
extern char pusha[];
extern char popa[];

typedef struct			s_MEgA_StRuCtuRrRE_HAXOR
{
	void			*file;
	size_t			file_size;

	int32_t			nb_hp;
	size_t			segment_injection_offset;
	size_t			base_entry;
	size_t			offset_injection;

}						t_info;

int32_t		get_elf64_zone(t_info *info);
int32_t		get_elf32_zone(t_info *info);
size_t      get_injection32_size();
void		create_woody(t_info *info);

#endif
