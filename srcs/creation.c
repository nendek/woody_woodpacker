#include "woody.h"

// char shellcode[]=
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

// static void	modify_woody(t_info *info, int fd)
// {
// 	size_t		offset;
// 	size_t		val;
// 
// 	offset = (size_t)((size_t)(info->text_section) - (size_t)(info->file)) + SHSIZE_OFFSET_64;
// 	if (lseek(fd, offset, SEEK_SET) == -1)
// 		return ;
// 	val = ((Elf64_Shdr *)(info->text_section))->sh_size;
// 	val += LEN_SHELLCODE;
// 	write(fd, &val, 8);
// }

void	replace_headers(t_info *info, void *new_file, size_t nb_add)
{
	int32_t		i;
	Elf64_Ehdr *main_header;
	Elf64_Shdr *text_header;
	Elf64_Phdr *program_header;
	
	// modify e_shoff
	main_header = (Elf64_Ehdr *)new_file;
	main_header->e_shoff += nb_add;

	// modify program_header TODO : c'est en dur pour l'instant
	program_header = (Elf64_Phdr *)(new_file + sizeof(Elf64_Ehdr));
	program_header += 3;
	program_header->p_filesz += nb_add;
	program_header->p_memsz += nb_add;
	program_header++;
	int j = 4;
	while (j < main_header->e_phnum)
	{
		if (program_header->p_offset > info->text_offset)
		{
			dprintf(1, "%d || %#lx\n", j, program_header->p_offset);
			program_header->p_offset += nb_add;
			program_header->p_vaddr += nb_add;
		}
		j++;
		program_header++;
	}

	//modify e_entry
// 	main_header->e_entry += nb_add;

	// modify offset des sections headers situes apres le .text header
	text_header = (Elf64_Shdr *)(new_file + (size_t)(info->text_section - (size_t)(info->file)));
	text_header->sh_size += nb_add;
	i = info->text_section_index;
	while (i < info->nb_hs - 1)
	{
		text_header++;
		text_header->sh_offset += nb_add;
		i++;
	}
}

void	ft_memcpy(void *dest, void *src, size_t size)
{
	size_t i = 0;

	while (i < size)
	{
		((char *)dest)[i] = ((char *)src)[i];
		i++;
	}
}

void	print_woody(t_info *info, void *new_file, size_t nb_add)
{
	int		fd;

	if ((fd = open("woody", O_CREAT | O_TRUNC | O_WRONLY, 0755)) == -1)
		return ;
	write(fd, new_file, info->file_size + nb_add); 
	close(fd);
}

size_t	inject_code(t_info *info, void *new_file, size_t nb_add)
{
	(void)info;
	ft_memcpy(new_file, shellcode, nb_add);
	return (73);
}

void	create_woody(t_info *info)
{
	void	*new_file;
	size_t	nb_add;

	nb_add = LEN_SHELLCODE; // TODO remplacer par une fonction eventuellement;
	if (!(new_file = malloc(info->file_size + nb_add)))
		return ;
	ft_memcpy(new_file, info->file, info->text_offset);
	inject_code(info, new_file + info->text_offset, nb_add);
	ft_memcpy(new_file + info->text_offset + nb_add, info->file + info->text_offset, info->file_size - info->text_offset);
	replace_headers(info, new_file, nb_add);

	print_woody(info, new_file, nb_add);
	free(new_file);
}
