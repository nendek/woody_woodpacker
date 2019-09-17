#include "woody.h"
# define INJECTION64_SIZE (sizeof(shellcode64)-1 + sizeof(jmp64)-1)

const char shellcode64[] =
"\x60\x90\x90\x90\x90\x61";

char jmp64[] = "\xe9\xff\xff\xff\xff\xff\xff\xff\xff";

static size_t		get_injection64_size() { return (INJECTION64_SIZE); }

static int32_t		inject_code64(t_info *info, void *new_file)
{
	char		*inject;
	uint64_t	addr_to_jmp;

	if (!(inject = malloc(get_injection64_size())))
		return (0);
	addr_to_jmp = (uint64_t)(info->base_entry - info->offset_injection - get_injection64_size());
	ft_memcpy(jmp64 + 1, &(addr_to_jmp), sizeof(uint64_t));
	ft_strcat(inject, shellcode64);
	ft_strcat(inject, jmp64);
	ft_memcpy(new_file, inject, get_injection64_size());
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
	program_header->p_filesz += get_injection64_size();
	program_header->p_memsz += get_injection64_size();
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
	if (start_next_segment - end_segment < INJECTION64_SIZE)
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
	info->funcs->get_code_size = &get_injection64_size;

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
