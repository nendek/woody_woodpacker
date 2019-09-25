#ifndef WOODY_H
# define WOODY_H

# include <sys/mman.h>
# include <sys/time.h>
# include <elf.h>
# include <fcntl.h>
# include <sys/stat.h>
# include <unistd.h>
# include <stdlib.h>
# include <stdio.h>
# include <time.h>

# define ELF_32 1
# define ELF_64 2
# define NB_TIMING 8
# define TIMING 0x37B4D3DD

typedef struct			s_MEgA_StRuCtuRrRE_HAXOR t_info;

typedef struct			s_funcs
{
	int32_t		(*inject_loader)(t_info *, void *);
	void		(*replace_headers)(t_info *, void *);
	void		(*append_code)(t_info *, void *);
	void		(*replace_jmploader)(t_info *, void *);
}						t_funcs;

struct				s_MEgA_StRuCtuRrRE_HAXOR
{
	void			*file;
	size_t			file_size;

	// base file info
	int32_t			nb_hp;
	size_t			base_entry;
	size_t			bss_size;
	size_t			end_data_seg;
	size_t			segment_text_header;
	size_t			segment_data_header;

	// packer file info
	size_t			offset_loader;
	size_t			loader_size;
	size_t			offset_woody;
	size_t			woody_size;
	uint32_t		Key;
	t_funcs			*funcs;

};

int32_t		get_elf64_zone(t_info *info);

int32_t		get_elf32_zone(t_info *info);

void		create_Key(t_info *info);
uint32_t	get_last_key(uint32_t master_Key);
void		create_woody(t_info *info);

void		create_packer(t_info *info, void *new_file);

		/* LIBFT_HANDLER */
void    ft_memcpy(void *dest, void *src, size_t size);
void    ft_bzero(char *str, size_t len);

#endif
