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


typedef struct			s_MEgA_StRuCtuRrRE_HAXOR
{
	void			*file;
	size_t			file_size;

	void			*text_section;
	int32_t			text_section_index;
	int32_t			nb_hp;
	int32_t			nb_hs;
	int32_t			origin_size;
	int32_t			text_offset;

}						t_info;

int32_t		get_elf64_text(t_info *info);
void		create_woody(t_info *info);

#endif
