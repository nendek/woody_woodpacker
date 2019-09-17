#include "woody.h"

char shellcode64[] =
"\x50\x51\x52\x41\x50\x41\x51\x41\x52\x41\x53\x50\x48\x31\xc0\x48\x31\xdb\x48\x31\xd2\x48\x83\xec\x10\xc7\x04\x24\x2e\x2e\x2e\x2e\xc7\x44\x24\x04\x57\x4f\x4f\x44\xc7\x44\x24\x08\x59\x2e\x2e\x2e\xc7\x44\x24\x0c\x2e\x0a\x00\x00\xba\x0e\x00\x00\x00\xb8\x01\x00\x00\x00\xbf\x01\x00\x00\x00\x48\x8d\x34\x24\x0f\x05\x48\x83\xc4\x10\x58\x41\x5b\x41\x5a\x41\x59\x41\x58\x5a\x59\x58";
char jmp64[] = "\xe9\xff\xff\xff\xff\xff\xff\xff\xff";

static int32_t		inject_code64(t_info *info, void *new_file)
{
	char		*inject;
	uint64_t	addr_to_jmp;

	if (!(inject = malloc(info->injection_size)))
		return (0);
	addr_to_jmp = (uint64_t)(info->base_entry - info->offset_injection - info->injection_size + 4);
	ft_memcpy(jmp64 + 1, &(addr_to_jmp), sizeof(uint64_t));
	ft_memcpy(inject, shellcode64, sizeof(shellcode64) - 1);
	ft_memcpy(inject + sizeof(shellcode64) - 1, jmp64, sizeof(jmp64) -1);
	ft_memcpy(new_file, inject, info->injection_size);
	return (1);
}

static void			replace_headers64(t_info *info, void *new_file)
{
	Elf64_Ehdr *main_header;
	Elf64_Phdr *program_header;

	main_header = (Elf64_Ehdr *)new_file;
	program_header = (Elf64_Phdr *)(new_file + sizeof(Elf64_Ehdr));

	main_header->e_entry = info->offset_injection;
	program_header = (Elf64_Phdr *)(new_file + info->segment_injection_offset);
	program_header->p_filesz += info->injection_size;
	program_header->p_memsz += info->injection_size;
}

static int32_t		save_place_to_inject(t_info *info, Elf64_Phdr *program_header, int32_t nb_segment)
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

int32_t				get_elf64_zone(t_info *info)
{
	Elf64_Ehdr *header;
	Elf64_Phdr *program_header;
	int			i;
	
	info->funcs->inject_code = &inject_code64;
	info->funcs->replace_headers = &replace_headers64;
	info->injection_size = sizeof(shellcode64) + sizeof(jmp64) - 2;

	header = (Elf64_Ehdr *)(info->file);
	program_header = (Elf64_Phdr *)(info->file + sizeof(Elf64_Ehdr));

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
