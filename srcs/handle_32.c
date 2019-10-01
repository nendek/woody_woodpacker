#include "woody.h"

# define LOADER32_SIZE sizeof(loader32) - 1
static char loader32[] =
// "\x60\xba\x07\x00\x00\x00\xb9\x5d\x10\x00\x00\x8d\x9f\x71\x2e\x00"
// "\x00\x81\xe3\x00\xf0\xff\xff\xb8\x7d\x00\x00\x00\xcd\x80\xe9\x5a"
// "\x2e\x00\x00\xba\x07\x00\x00\x00\xb9\x5d\x10\x00\x00\x8d\x9f\x4f"
// "\x2e\x00\x00\x81\xe3\x00\xf0\xff\xff\xb8\x7d\x00\x00\x00\xcd\x80\x61";
"\x60\xe8\x42\x00\x00\x00\xba\x07\x00\x00\x00\xb9\x5d\x10\x00\x00\x8d\x9f\x11\x11\x11\x11\x81\xe3\x00\xf0\xff\xff\xb8\x7d\x00\x00\x00\xcd\x80\xe9\x55\x2e\x00\x00\xba\x07\x00\x00\x00\xb9\x5d\x10\x00\x00\x8d\x9f\x11\x11\x11\x11\x81\xe3\x00\xf0\xff\xff\xb8\x7d\x00\x00\x00\xcd\x80\x61\xeb\x04\x8b\x3c\x24\xc3";

# define JMP32_SIZE sizeof(jmp32) - 1
static char jmp32[] =
"\xe9\xff\xff\xff\xff";

# define WOODY_SIZE sizeof(woody32) - 1
static char woody32[] = 
"\x31\xc0\x31\xdb\x31\xd2\x83\xec\x10\xc7\x04\x24\x2e\x2e\x2e\x2e\xc7\x44\x24\x04\x57\x4f\x4f\x44\xc7\x44\x24\x08\x59\x2e\x2e\x2e\xc7\x44\x24\x0c\x2e\x0a\x00\x00\xba\x0e\x00\x00\x00\x8d\x0c\x24\xbb\x01\x00\x00\x00\xb8\x04\x00\x00\x00\xcd\x80\x89\x7c\x24\x0c\xe8\x86\x00\x00\x00\x89\xfe\x81\xc6\x78\x56\x34\x12\xc7\x44\x24\x04\x42\x00\x00\x00\xc7\x04\x24\xff\xff\xff\xff\xba\x07\x00\x00\x00\x8b\x0c\x24\x81\xc1\x00\x10\x00\x00\x89\xf3\x81\xe3\x00\xf0\xff\xff\xb8\x7d\x00\x00\x00\xcd\x80\xc7\x44\x24\x08\xdd\xd3\xb4\x37\xbb\x08\x00\x00\x00\x8b\x54\x24\x04\x8b\x0c\x24\x89\xf7\x8b\x07\x31\xd0\xab\x83\xe9\x04\x83\xf9\x00\x7f\xf3\x03\x54\x24\x08\x4b\x85\xdb\x75\xe5\xba\x07\x00\x00\x00\x8b\x0c\x24\x81\xc1\x00\x10\x00\x00\x89\xf3\x81\xe3\x00\xf0\xff\xff\xb8\x7d\x00\x00\x00\xcd\x80\x8b\x7c\x24\x0c\x83\xc4\x10\xeb\x04\x8b\x3c\x24\xc3";

# define JMPEW_SIZE sizeof(jmp_end_woody) - 1
static char jmp_end_woody[] =
"\xe9\xff\xff\xff\xff";

# define PUSHALL_SIZE sizeof(push_all) - 1
static char push_all[] = 
"\x60";

# define POPALL_SIZE sizeof(pop_all) - 1
static char pop_all[] = 
"\x61";


Elf32_Phdr	*get_last_load32(t_info *info, void *file)
{
	Elf32_Phdr	*program_header;

	if (!(program_header = move_ptr(info, file, sizeof(Elf32_Ehdr))))
		return (0);
	while (program_header->p_type != PT_LOAD)
	{
		if (!(program_header = move_ptr(info, program_header, sizeof(Elf32_Phdr))))
			return (0);
	}
	while (program_header->p_type == PT_LOAD)
	{
		if (!(program_header = move_ptr(info, program_header, sizeof(Elf32_Phdr))))
			return (0);
	}
	program_header--;
	return (program_header);
}

static void		replace_jmp_end_woody32(t_info *info)
{
	// adresse d'arrivee : offset_loader + la moitie (en dur = 0x24)
	// adresse relative : adresse du jump - adresse courante
	// adresse de depart : program_header->p_vaddr + + program_header->p_filesz + WOODY_SIZE - 1

	size_t		depart;
	size_t		arrive;
	uint32_t	rel = 0;

	depart = info->offset_woody_mem + WOODY_SIZE;
	if (info->injection_mode == WOODY_PADDING || info->injection_mode == WOODY_BSS)
		arrive = info->offset_loader_mem + 0x23;
	else
		arrive = info->base_entry;
	rel = (uint32_t)(arrive - depart);

	ft_memcpy(jmp_end_woody + 1, &(rel), sizeof(uint32_t));
}

static void		modify_woody(t_info *info)
{
	size_t		val;
	(void)val;

	// put .text addr into EDI
	val = info->base_entry - info->offset_woody_mem - 69;
	ft_memcpy(woody32 + 73, &(val), sizeof(uint32_t));

	// put Key value
	val = get_last_key(info->Key);
	ft_memcpy(woody32 + 81, &(val), sizeof(uint32_t));

	// put .text size
	val = info->text_size - 4;
	ft_memcpy(woody32 + 88, &(val), sizeof(uint32_t));

}

static void		append_woody32(t_info *info, void *new_file)
{
	// add .bss section to the physical file : DO nothing, just jump the section

	// edit shellcode with infos
	modify_woody(info);

	// append woody to the end of the .bss
	ft_memcpy(new_file + info->offset_woody_file, woody32, WOODY_SIZE);

	// edit jump shellcode
	replace_jmp_end_woody32(info);

	// append jump to loader to the end of the woody shellcode
	ft_memcpy(new_file + info->offset_woody_file + WOODY_SIZE, jmp_end_woody, JMPEW_SIZE);
}

void			inject_woody32(t_info *info, void *new_file)
{
	// edit shellcode with infos
	modify_woody(info);

	// copy it into file
	ft_memcpy(new_file, woody32, WOODY_SIZE);

	// edit jump shellcode
	replace_jmp_end_woody32(info);

	// append jump to loader to the end of the woody shellcode
	ft_memcpy(new_file + WOODY_SIZE, jmp_end_woody, JMPEW_SIZE);
}

void			append_woody_loader32(t_info *info, void *new_file)
{
	info->offset_woody_mem += PUSHALL_SIZE;
	modify_woody(info);
	info->offset_woody_mem -= PUSHALL_SIZE;

// 	info->offset_woody_file += PUSHALL_SIZE;
	ft_memcpy(new_file + info->offset_woody_file, push_all, PUSHALL_SIZE);
	ft_memcpy(new_file + info->offset_woody_file + PUSHALL_SIZE, woody32, WOODY_SIZE);
	ft_memcpy(new_file + info->offset_woody_file + PUSHALL_SIZE + WOODY_SIZE, pop_all, POPALL_SIZE);
// 	info->offset_woody_file -= PUSHALL_SIZE;

	// wrap func
	info->offset_woody_mem += PUSHALL_SIZE + POPALL_SIZE + JMPEW_SIZE;
	replace_jmp_end_woody32(info);
	info->offset_woody_mem -= PUSHALL_SIZE + POPALL_SIZE + JMPEW_SIZE;

// 	info->offset_woody_file += PUSHALL_SIZE + POPALL_SIZE + JMPEW_SIZE;
	ft_memcpy(new_file + info->offset_woody_file + PUSHALL_SIZE + WOODY_SIZE + POPALL_SIZE, jmp_end_woody, JMPEW_SIZE);
// 	info->offset_woody_file -= PUSHALL_SIZE + POPALL_SIZE + JMPEW_SIZE;
}

static void		modify_loader(t_info *info)
{
	uint32_t	new_rel;
	size_t		offset;

	(void)new_rel;
// 	offset = header->p_vaddr + header->p_memsz - info->offset_loader_mem - 35; //+ (info->offset_loader & 1);
	offset = info->offset_woody_mem - info->offset_loader_mem - 6;

	// change first load : offset 18 dans le shellcode
	new_rel = offset;
	ft_memcpy(loader32 + 18, &new_rel, sizeof(uint32_t));

	// change last load: offset 47 dans le shellcode
	new_rel = offset;
	ft_memcpy(loader32 + 52, &new_rel, sizeof(uint32_t));

	// change jmp addr: offset 31 dans le shellcode
	new_rel = offset - 34;
	ft_memcpy(loader32 + 36, &new_rel, sizeof(uint32_t));

	
}

static int32_t		inject_loader32(t_info *info, void *new_file)
{
	char		*inject;
	uint32_t	addr_to_jmp;

	if (!(inject = malloc(info->loader_size)))
		return (0);

	// calc old entry_point rel addr
	addr_to_jmp = (uint32_t)(info->base_entry - info->offset_loader_mem - info->loader_size);
	ft_memcpy(jmp32 + 1, &(addr_to_jmp), sizeof(uint32_t));

	// modify shellcode_size and rel addr in loader
	modify_loader(info);

	// end injection
	ft_memcpy(inject, loader32, LOADER32_SIZE);
	ft_memcpy(inject + LOADER32_SIZE, jmp32, JMP32_SIZE);
	ft_memcpy(new_file, inject, info->loader_size);
	free(inject);
	return (1);
}

static void			replace_headers32(t_info *info, void *new_file)
{
	Elf32_Ehdr *main_header;
	Elf32_Phdr *program_header;

	// first part : replace_headers for loader
	main_header = (Elf32_Ehdr *)new_file;
	program_header = (Elf32_Phdr *)(new_file + sizeof(Elf32_Ehdr));

	main_header->e_entry = info->offset_loader_mem;
	program_header = (Elf32_Phdr *)(new_file + info->segment_text_header);
	if (info->injection_mode == WOODY_PADDING || info->injection_mode == WOODY_BSS)
	{
		program_header->p_filesz += info->loader_size;
		program_header->p_memsz += info->loader_size;
	}

	// second part : replace_headers for woody
	program_header = (Elf32_Phdr *)(new_file + info->segment_data_header);
	if (info->injection_mode == WOODY_BSS)
	{
		program_header->p_filesz += info->bss_size + info->woody_size;
		program_header->p_memsz = program_header->p_filesz;
	}
	else if (info->injection_mode == WOODY_PADDING)
	{
		program_header->p_filesz += info->woody_size;
		program_header->p_memsz += info->woody_size;
	}
	else if (info->injection_mode == DOUBLE_PADDING)
	{
		program_header->p_filesz += info->woody_size + PUSHALL_SIZE + POPALL_SIZE;
		program_header->p_memsz += info->woody_size + PUSHALL_SIZE + POPALL_SIZE;
		program_header->p_flags |= PF_X;
	}
	else
	{
		main_header->e_entry = program_header->p_vaddr + program_header->p_memsz;
		program_header->p_filesz += info->bss_size + info->woody_size + PUSHALL_SIZE + POPALL_SIZE;
		program_header->p_memsz = program_header->p_filesz;
		program_header->p_flags |= PF_X;
	}
}

static size_t				get_text_size(t_info *info)
{
    Elf32_Phdr  *program_header;

	if (!(program_header = (Elf32_Phdr *)(move_ptr(info, info->file, info->segment_text_header))))
		return (1);
    return (program_header->p_vaddr + program_header->p_filesz - info->base_entry);
}

static size_t				get_text_segment(t_info *info)
{
	Elf32_Phdr	*header;
	int32_t		i;

	i = 0;
	if (!(header = (Elf32_Phdr *)(move_ptr(info, info->file, sizeof(Elf32_Ehdr)))))
		return (-1);
	while (i < info->nb_hp)
	{
		if ((header->p_type == PT_LOAD) && (info->base_entry > header->p_vaddr) && (info->base_entry < header->p_vaddr + header->p_memsz))
			return ((size_t)header - (size_t)(info->file));

		if (!(header = move_ptr(info, header, sizeof(Elf32_Phdr))))
			return (-1);
		i++;
	}
	dprintf(2, "Error no .text section detected");
	return (-1);
}

int32_t				get_elf32_zone(t_info *info)
{
	Elf32_Ehdr	*header;
	Elf32_Phdr	*program_header;
	
	header = (Elf32_Ehdr *)(info->file);
	// set functions to 32 bit mode
	info->funcs->inject_loader = &inject_loader32;
	info->funcs->inject_woody = &inject_woody32;
	info->funcs->replace_headers = &replace_headers32;
	info->funcs->replace_jmp_end_woody = &replace_jmp_end_woody32;
	info->funcs->append_woody = &append_woody32;
	info->funcs->append_woody_loader = &append_woody_loader32;
	info->funcs->encryption = &encryption32;

	if (!(program_header = get_last_load32(info, info->file)))
		return (1);
	// save usefull infos
	info->nb_hp = header->e_phnum;
	info->base_entry = header->e_entry;
	if ((info->segment_text_header = get_text_segment(info)) == (size_t)(-1))
		return (1);
	if ((info->text_size = get_text_size(info)) == (size_t)(-1))
		return (1);
	info->bss_size = program_header->p_memsz - program_header->p_filesz;
	info->end_data_seg = program_header->p_offset + program_header->p_filesz;
	info->loader_size = LOADER32_SIZE + JMP32_SIZE;
	info->woody_size = WOODY_SIZE + JMPEW_SIZE;
	info->push_size = PUSHALL_SIZE;

	if (get_case32_1(info) == 0)
	{
		info->injection_mode = WOODY_PADDING;
		return (0);
	}
	if (get_case32_2(info) == 0)
	{
		info->injection_mode = WOODY_BSS;
		return (0);
	}
	if (get_case32_3(info) == 0)
	{
		info->injection_mode = DOUBLE_PADDING;
		return (0);
	}
	if (get_case32_4(info) == 0)
	{
		info->injection_mode = DOUBLE_BSS;
		return (0);
	}
	return (1);
}
