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

typedef struct			s_MEgA_StRuCtuRrRE_HAXOR t_info;

typedef struct			s_funcs
{
	int32_t		(*inject_code)(t_info *, void *);
	void		(*replace_headers)(t_info *, void *);
// 	size_t		(*get_code_size)();
}						t_funcs;

struct				s_MEgA_StRuCtuRrRE_HAXOR
{
	void			*file;
	size_t			file_size;

	int32_t			nb_hp;
	size_t			segment_injection_offset;
	size_t			base_entry;
	size_t			offset_injection;
	size_t			offset_append;
	size_t			injection_size;
	t_funcs			*funcs;

};

int32_t		get_elf64_zone(t_info *info);

int32_t		get_elf32_zone(t_info *info);

void		create_woody(t_info *info);

void		create_packer(t_info *info, void *new_file);

		/* LIBFT_HANDLER */
void    ft_memcpy(void *dest, void *src, size_t size);
void    ft_bzero(char *str, size_t len);

#endif
