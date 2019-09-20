#include "woody.h"

# define LOADER_SIZE sizeof(loader64) - 1
static char loader64[] =
"\x57\x56\x50\x51\x52\x41\x50\x41\x51\x41\x52\x41\x53\x41\x54\xba"
"\x05\x00\x00\x00\xbe\x5d\x00\x00\x00\x48\x8d\x3d\x5c\x2e\x00\x00"
"\x48\x81\xe7\x00\xf0\xff\xff\xb8\x0a\x00\x00\x00\x0f\x05\xe9\x4a"
"\x2e\x00\x00\xba\x03\x00\x00\x00\xbe\x5d\x00\x00\x00\x48\x8d\x3d"
"\x38\x2e\x00\x00\x48\x81\xe7\x00\xf0\xff\xff\xb8\x0a\x00\x00\x00"
"\x0f\x05\x41\x5c\x41\x5b\x41\x5a\x41\x59\x41\x58\x5a\x59\x58\x5e\x5f";

# define JMP64_SIZE sizeof(jmp64) - 1
static char jmp64[] = "\xe9\xff\xff\xff\xff\xff\xff\xff\xff";

void				modify_loader(t_info *info)
{
	Elf64_Phdr	*header;
	uint32_t	new_rel;
	size_t		offset;

	header = get_last_load(info->file);
	offset = header->p_vaddr + header->p_memsz + 7 - info->offset_loader; //+ (info->offset_loader & 1);

	// change first load : offset 31 dans le shellcode
	new_rel = offset - 31;
	ft_memcpy(loader64 + 28, &new_rel, sizeof(uint32_t));

	// change last load: offset 67 dans le shellcode
	new_rel = offset - 67;
	ft_memcpy(loader64 + 64, &new_rel, sizeof(uint32_t));

	// change jmp addr: offset 50 dans le shellcode
	new_rel = offset - 50;
	ft_memcpy(loader64 + 47, &new_rel, sizeof(uint32_t));

	
}

static int32_t		inject_loader64(t_info *info, void *new_file)
{
	char		*inject;
	uint64_t	addr_to_jmp;

	if (!(inject = malloc(info->loader_size)))
		return (0);
	// calc old entry_point rel addr
	addr_to_jmp = (uint64_t)(info->base_entry - info->offset_loader - info->loader_size + 4);
	ft_memcpy(jmp64 + 1, &(addr_to_jmp), sizeof(uint64_t));

	// modify shellcode_size and rel addr in loader
	modify_loader(info);

	// end injection
	ft_memcpy(inject, loader64, LOADER_SIZE);
	ft_memcpy(inject + LOADER_SIZE, jmp64, JMP64_SIZE);
	ft_memcpy(new_file, inject, info->loader_size);
	return (1);
}

static void			replace_headers_loader64(t_info *info, void *new_file)
{
	Elf64_Ehdr *main_header;
	Elf64_Phdr *program_header;

	main_header = (Elf64_Ehdr *)new_file;
	program_header = (Elf64_Phdr *)(new_file + sizeof(Elf64_Ehdr));

	main_header->e_entry = info->offset_loader;
	program_header = (Elf64_Phdr *)(new_file + info->segment_text_header);
	program_header->p_filesz += info->loader_size;
	program_header->p_memsz += info->loader_size;
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
	if (start_next_segment - end_segment < info->loader_size)
		return (0);
	info->offset_loader = end_segment;
	info->segment_text_header = (size_t)((size_t)program_header - (size_t)(info->file));
	return (1);
}

int32_t				get_elf64_zone(t_info *info)
{
	Elf64_Ehdr *header;
	Elf64_Phdr *program_header;
	int			i;
	
	// set functions to 64 bit mode
	info->funcs->inject_loader = &inject_loader64;
	info->funcs->replace_headers_loader = &replace_headers_loader64;
	info->loader_size = LOADER_SIZE + JMP64_SIZE;

	
	header = (Elf64_Ehdr *)(info->file);
	program_header = (Elf64_Phdr *)(info->file + sizeof(Elf64_Ehdr));

	// save usefull infos
	info->nb_hp = header->e_phnum;
	info->base_entry = header->e_entry;
	
	// get zone to inject loader
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
