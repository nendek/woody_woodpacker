#include "woody.h"

# define LOADER32_SIZE sizeof(loader32) - 1
static char loader32[] =
"\x57\x56\x50\x51\x52\x41\x50\x41\x51\x41\x52\x41\x53\x41\x54\xba"
"\x05\x00\x00\x00\xbe\x5d\x10\x00\x00\x48\x8d\x3d\x5c\x2e\x00\x00"
"\x48\x81\xe7\x00\xf0\xff\xff\xb8\x0a\x00\x00\x00\x0f\x05\xe9\x4a"
"\x2e\x00\x00\xba\x03\x00\x00\x00\xbe\x5d\x10\x00\x00\x48\x8d\x3d"
"\x38\x2e\x00\x00\x48\x81\xe7\x00\xf0\xff\xff\xb8\x0a\x00\x00\x00"
"\x0f\x05\x41\x5c\x41\x5b\x41\x5a\x41\x59\x41\x58\x5a\x59\x58\x5e\x5f";

# define JMP32_SIZE sizeof(jmp32) - 1
static char jmp32[] =
"\xe9\xff\xff\xff\xff";

# define WOODY_SIZE sizeof(woody32) - 1
static char woody32[] = 
"\x48\x31\xc0\x48\x31\xdb\x48\x31\xd2\x48\x83\xec\x10\xc7\x04\x24\x2e\x2e\x2e\x2e\xc7\x44\x24\x04\x57\x4f\x4f\x44\xc7\x44\x24\x08\x59\x2e\x2e\x2e\xc7\x44\x24\x0c\x2e\x0a\x00\x00\xba\x0e\x00\x00\x00\xb8\x01\x00\x00\x00\xbf\x01\x00\x00\x00\x48\x8d\x34\x24\x0f\x05\x48\x83\xc4\x10";

static Elf32_Phdr	*get_last_load32(void *file)
{
	Elf32_Phdr	*program_header;

	program_header = (Elf32_Phdr *)(file + sizeof(Elf32_Ehdr));
	while (program_header->p_type != PT_LOAD)
		program_header++;
	while (program_header->p_type == PT_LOAD)
		program_header++;
	program_header--;
	return (program_header);
}

static void		replace_jmploader32(t_info *info, void *program_header)
{
	// adresse d'arrivee : offset_loader + la moitie (en dur = 0x24)
	// adresse relative : adresse du jump - adresse courante
	// adresse de depart : program_header->p_vaddr + + program_header->p_filesz + WOODY_SIZE - 1

	size_t		depart;
	size_t		arrive;
	uint32_t	rel = 0;

	depart = ((Elf32_Phdr *)(program_header))->p_vaddr + ((Elf32_Phdr *)(program_header))->p_memsz + WOODY_SIZE;
	//TODO: 0x2E make change
	arrive = info->offset_loader + 0x2e;
	rel = (uint32_t)(arrive - depart);

	ft_memcpy(jmp32 + 1, &(rel), sizeof(uint32_t));
}

static void		append_code32(t_info *info, void *new_file)
{
	// add .bss section to the physical file : DO nothing, just jump the section

	// append woody to the end of the .bss
	ft_memcpy(new_file + info->offset_woody, woody32, WOODY_SIZE);

	// edit jump shellcode
	replace_jmploader32(info, new_file + info->segment_data_header);

	// append jump to loader to the end of the woody shellcode
	ft_memcpy(new_file + info->offset_woody + WOODY_SIZE, jmp32, JMP32_SIZE);

}

static void		modify_loader(t_info *info)
{
	Elf32_Phdr	*header;
	uint32_t	new_rel;
	size_t		offset;

	header = get_last_load32(info->file);
	offset = header->p_vaddr + header->p_memsz - 1 - info->offset_loader; //+ (info->offset_loader & 1);

	// change first load : offset 31 dans le shellcode
	new_rel = offset - 31;
	ft_memcpy(loader32 + 28, &new_rel, sizeof(uint32_t));

	// change last load: offset 67 dans le shellcode
	new_rel = offset - 67;
	ft_memcpy(loader32 + 64, &new_rel, sizeof(uint32_t));

	// change jmp addr: offset 50 dans le shellcode
	new_rel = offset - 50;
	ft_memcpy(loader32 + 47, &new_rel, sizeof(uint32_t));

	
}

static int32_t		inject_loader32(t_info *info, void *new_file)
{
	char		*inject;
	uint32_t	addr_to_jmp;

	if (!(inject = malloc(info->loader_size)))
		return (0);

	// calc old entry_point rel addr
	addr_to_jmp = (uint32_t)(info->base_entry - info->offset_loader - info->loader_size + 4);
	ft_memcpy(jmp32 + 1, &(addr_to_jmp), sizeof(uint32_t));

	// modify shellcode_size and rel addr in loader
	modify_loader(info);

	// end injection
	ft_memcpy(inject, loader32, LOADER32_SIZE);
	ft_memcpy(inject + LOADER32_SIZE, jmp32, JMP32_SIZE);
	ft_memcpy(new_file, inject, info->loader_size);
	return (1);
}

static void			replace_headers32(t_info *info, void *new_file)
{
	Elf32_Ehdr *main_header;
	Elf32_Phdr *program_header;

	// first part : replace_headers for loader
	main_header = (Elf32_Ehdr *)new_file;
	program_header = (Elf32_Phdr *)(new_file + sizeof(Elf32_Ehdr));

	main_header->e_entry = info->offset_loader;
	program_header = (Elf32_Phdr *)(new_file + info->segment_text_header);
	program_header->p_filesz += info->loader_size;
	program_header->p_memsz += info->loader_size;

	// second part : replace_headers for woody
	program_header = (Elf32_Phdr *)(new_file + info->segment_data_header);
	program_header->p_filesz += info->bss_size + info->woody_size;
	program_header->p_memsz = program_header->p_filesz;
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
	if (start_next_segment - end_segment < info->loader_size)
		return (0);
	info->offset_loader = end_segment;
	info->segment_text_header = (size_t)((size_t)program_header - (size_t)(info->file));
	return (1);
}

int32_t				get_elf32_zone(t_info *info)
{
	Elf32_Ehdr	*header;
	Elf32_Phdr	*program_header;
	int		i;
	
	// set functions to 32 bit mode
	info->funcs->inject_loader = &inject_loader32;
	info->funcs->replace_headers = &replace_headers32;
	info->funcs->replace_jmploader = &replace_jmploader32;
	info->funcs->append_code = &append_code32;

	program_header = get_last_load32(info->file);
	info->offset_woody = program_header->p_offset + program_header->p_memsz;
	info->woody_size = WOODY_SIZE + JMP32_SIZE;
	info->end_data_seg = program_header->p_offset + program_header->p_filesz;

	header = (Elf32_Ehdr *)(info->file);

	// save usefull infos
	info->nb_hp = header->e_phnum;
	info->base_entry = header->e_entry;
	info->bss_size = program_header->p_memsz - program_header->p_filesz;
	info->segment_data_header = (size_t)((size_t)program_header - (size_t)(info->file));
	info->loader_size = LOADER32_SIZE + JMP32_SIZE;

	// get zone to inject loader and save offset_loader and segment_text_header
	program_header = (Elf32_Phdr *)(info->file + sizeof(Elf32_Ehdr));
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
