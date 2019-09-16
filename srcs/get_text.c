#include "woody.h"
# define INJECTION32_SIZE (sizeof(shellcode32)-1 + sizeof(jmp32)-1 + sizeof(pusha)-1 + sizeof(popa)-1)

const char shellcode32[] = 
"\x31\xc0\x31\xdb\x31\xd2\x68\x72\x6c\x64\x21\xc6\x44\x24\x03\x0a\x68\x6f\x20\x77"
"\x6f\x68\x48\x65\x6c\x6c\x89\xe1\xb2\x0c\xb0\x04\xb3\x01\xcd\x80\xb2\x0c\x01\xd4";
// "\x31\xc0\x31\xdb\x31\xc9\x31\xd2"
// "\xeb\x32\x5b\xb0\x05\x31\xc9\xcd"
// "\x80\x89\xc6\xeb\x06\xb0\x01\x31"
// "\xdb\xcd\x80\x89\xf3\xb0\x03\x83"
// "\xec\x01\x8d\x0c\x24\xb2\x01\xcd"
// "\x80\x31\xdb\x39\xc3\x74\xe6\xb0"
// "\x04\xb3\x01\xb2\x01\xcd\x80\x83"
// "\xc4\x01\xeb\xdf\xe8\xc9\xff\xff"
// "\xff"
// "~/.vimrc";

char jmp32[] = "\xe9\xff\xff\xff\xff";
char pusha[] = "\x60";
char popa[] = "\x61";

// static int32_t ft_strcmp(char *s1, char *s2)
// {
// 	while (*s1 == *s2 && *s1 && *s2)
// 	{
// 		s1++;
// 		s2++;
// 	}
// 	return (*s2 - *s1);
// }
// 
int32_t			get_elf64_zone(t_info *info)
{
	(void)info;
// 	Elf64_Ehdr *header;
// 	Elf64_Shdr *sections;
// 	void		*str_table;
// 	
// 
// 	header = (Elf64_Ehdr *)(info->file);
// 	sections = (Elf64_Shdr *)((size_t)(info->file) + (size_t)(header->e_shoff));
// 	info->nb_hs = header->e_shnum;
// 	str_table = sections + header->e_shstrndx;
// 	str_table = (void *)((size_t)(info->file) + (size_t)(((Elf64_Shdr *)(str_table))->sh_offset));
// 	(void)str_table;
// 	
// 	for (int32_t i = 0; i < info->nb_hs; i++)
// 	{
// 		if (ft_strcmp((char *)(str_table + sections->sh_name), ".text") == 0)
// 		{
// 			info->text_section = (void *)sections;
// 			info->origin_size = sections->sh_size;
// 			info->text_offset = sections->sh_offset;
// 			info->text_section_index = i;
// 			return (1);
// 		}
// 		sections++;
// 	}
// 	return (0);
	return (0);
}

size_t				get_injection32_size()
{
	return (INJECTION32_SIZE);
}

int32_t				save_place_to_inject(t_info *info, Elf32_Phdr *program_header, int32_t nb_segment)
{
	size_t		end_segment;
	size_t		start_next_segment;

	if (nb_segment >= info->nb_hp - 1)
	{
		dprintf(2, "ERROR during space searching : segment PT_LOAD is last segment\n");
		return (0);
	}
	if ((program_header->p_flags & PF_X) != PF_X)
	{
// 		dprintf(2, "Segment nb %d is not executable\n", nb_segment);
		return (0);
	}
	end_segment = program_header->p_offset + program_header->p_filesz;
	
	start_next_segment = (program_header + 1)->p_offset;
	if (start_next_segment - end_segment < INJECTION32_SIZE)
		return (0);
	info->offset_injection = end_segment;
	info->segment_injection_offset = (size_t)((size_t)program_header - (size_t)(info->file));
	return (1);
}

int32_t				get_elf32_zone(t_info *info)
{
	Elf32_Ehdr *header;
	Elf32_Phdr *program_header;
	int			i;
	
	header = (Elf32_Ehdr *)(info->file);
	program_header = (Elf32_Phdr *)(info->file + sizeof(Elf32_Ehdr));

	info->nb_hp = header->e_phnum;
	info->base_entry = header->e_entry;
	
	i = 0;
	while (i < info->nb_hp)
	{
		if (program_header->p_type == PT_LOAD)
		{
			if (save_place_to_inject(info, program_header, i) == 1)
				return (1);
		}
		i++;
		program_header++;
	}
	return (0);
}
