#include "woody.h"

static int32_t ft_strcmp(char *s1, char *s2)
{
	while (*s1 == *s2 && *s1 && *s2)
	{
		s1++;
		s2++;
	}
	return (*s2 - *s1);
}

int32_t			get_elf64_text(t_info *info)
{
	Elf64_Ehdr *header;
	Elf64_Shdr *sections;
	void		*str_table;
	

	header = (Elf64_Ehdr *)(info->file);
	sections = (Elf64_Shdr *)((size_t)(info->file) + (size_t)(header->e_shoff));
	info->nb_hs = header->e_shnum;
	str_table = sections + header->e_shstrndx;
	str_table = (void *)((size_t)(info->file) + (size_t)(((Elf64_Shdr *)(str_table))->sh_offset));
	(void)str_table;
	
	for (int32_t i = 0; i < info->nb_hs; i++)
	{
		if (ft_strcmp((char *)(str_table + sections->sh_name), ".text") == 0)
		{
			info->text_section = (void *)sections;
			info->origin_size = sections->sh_size;
			info->text_offset = sections->sh_offset;
			info->text_section_index = i;
			return (0);
		}
		sections++;
	}
	return (1);
}
