#include "woody.h"

char shellcode32[] = 
// "\x60\xeb\x19\x59\xb8\x04\x00\x00\x00\xbb\x01\x00\x00\x00\xba\x0e\x00\x00\x00\xcd\x80\xb8\x01\x00\x00\x00\xcd\x80\xe8\xe2\xff\xff\xff\x2e\x2e\x2e\x2e\x57\x4f\x4f\x44\x59\x2e\x2e\x2e\x2e\x0a\x61";



"\x60\x31\xc0\x31\xdb\x31\xd2\x68\x72\x6c\x64\x21\xc6\x44\x24\x03\x0a\x68\x6f\x20\x77"
"\x6f\x68\x48\x65\x6c\x6c\x89\xe1\xb2\x0c\xb0\x04\xb3\x01\xcd\x80\xb2\x0c\x01\xd4\x61";



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

static int32_t		inject_code32(t_info *info, void *new_file)
{
	char		*inject;
	uint32_t	addr_to_jmp;

	if (!(inject = malloc(info->injection_size)))
		return (0);
	addr_to_jmp = (uint32_t)(info->base_entry - info->offset_injection - info->injection_size);
	ft_memcpy(jmp32 + 1, &(addr_to_jmp), sizeof(uint32_t));
	ft_memcpy(inject, shellcode32, sizeof(shellcode32) - 1);
	ft_memcpy(inject + (sizeof(shellcode32) - 1), jmp32, sizeof(jmp32) - 1);
	ft_memcpy(new_file, inject, info->injection_size);
	return (1);
}

static void			replace_headers32(t_info *info, void *new_file)
{
	Elf32_Ehdr *main_header;
	Elf32_Phdr *program_header;

	main_header = (Elf32_Ehdr *)new_file;
	program_header = (Elf32_Phdr *)(new_file + sizeof(Elf32_Ehdr));

	main_header->e_entry = info->offset_injection;
	program_header = (Elf32_Phdr *)(new_file + info->segment_injection_offset);
	program_header->p_filesz += info->injection_size;
	program_header->p_memsz += info->injection_size;
}

static int32_t		save_place_to_inject(t_info *info, Elf32_Phdr *program_header, int32_t nb_segment)
{
	size_t		end_segment;
	size_t		start_next_segment;

	if (nb_segment >= info->nb_hp - 1)
	{
		dprintf(2, "ERROR during space searching : segment PT_LOAD is last segment\n");
		return (0);
	}
	if ((program_header->p_flags & PF_X) != PF_X)
		return (0);

	end_segment = program_header->p_offset + program_header->p_filesz;
	start_next_segment = (program_header + 1)->p_offset;
	if (start_next_segment - end_segment < info->injection_size)
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
	
	info->funcs->inject_code = &inject_code32;
	info->funcs->replace_headers = &replace_headers32;
	info->injection_size = sizeof(shellcode32) + sizeof(jmp32) - 2;

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
