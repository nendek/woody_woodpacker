#include "woody.h"

# define LOADER32_SIZE sizeof(loader32) - 1
static char loader32[] =
"\x60\xba\x05\x00\x00\x00\xb9\x5d\x10\x00\x00\x8d\x9f\x71\x2e\x00"
"\x00\x81\xe3\x00\xf0\xff\xff\xb8\x7d\x00\x00\x00\xcd\x80\xe9\x5a"
"\x2e\x00\x00\xba\x03\x00\x00\x00\xb9\x5d\x10\x00\x00\x8d\x9f\x4f"
"\x2e\x00\x00\x81\xe3\x00\xf0\xff\xff\xb8\x7d\x00\x00\x00\xcd\x80\x61";

# define JMP32_SIZE sizeof(jmp32) - 1
static char jmp32[] =
"\xe9\xff\xff\xff\xff";

# define WOODY_SIZE sizeof(woody32) - 1
static char woody32[] = 
"\x31\xc0\x31\xdb\x31\xd2\x83\xec\x10\xc7\x04\x24\x2e\x2e\x2e\x2e\xc7\x44\x24\x04\x57\x4f\x4f\x44\xc7\x44\x24\x08\x59\x2e\x2e\x2e\xc7\x44\x24\x0c\x2e\x0a\x00\x00\xba\x0e\x00\x00\x00\x8d\x0c\x24\xbb\x01\x00\x00\x00\xb8\x04\x00\x00\x00\xcd\x80\x89\x7c\x24\x0c\x89\xfe\x81\xc6\x78\x56\x34\x12\xc7\x44\x24\x04\x42\x00\x00\x00\xc7\x04\x24\xff\xff\xff\xff\xba\x03\x00\x00\x00\x8b\x0c\x24\x81\xc1\x00\x10\x00\x00\x89\xf3\x81\xe3\x00\xf0\xff\xff\xb8\x7d\x00\x00\x00\xcd\x80\xc7\x44\x24\x08\xdd\xd3\xb4\x37\xbb\x08\x00\x00\x00\x8b\x54\x24\x04\x8b\x0c\x24\x89\xf7\x8b\x07\x31\xd0\xab\x83\xe9\x04\x83\xf9\x00\x7f\xf3\x03\x54\x24\x08\x4b\x85\xdb\x75\xe5\xba\x05\x00\x00\x00\x8b\x0c\x24\x81\xc1\x00\x10\x00\x00\x89\xf3\x81\xe3\x00\xf0\xff\xff\xb8\x7d\x00\x00\x00\xcd\x80\x8b\x7c\x24\x0c\x83\xc4\x10";

# define JMPEW_SIZE sizeof(jmp_end_woody) - 1
static char jmp_end_woody[] =
"\xe9\xff\xff\xff\xff";


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
	arrive = info->offset_loader + 0x1e;
	rel = (uint32_t)(arrive - depart);

	ft_memcpy(jmp_end_woody + 1, &(rel), sizeof(uint32_t));
}

static void		modify_woody(t_info *info, void *new_file)
{
// 	Elf32_Phdr	*program_header;
// 	size_t		offset;
	size_t		val;
	(void)new_file;

// 	program_header = (Elf32_Phdr *)(new_file + info->segment_data_header);
// 	offset = ((Elf32_Phdr *)(program_header))->p_vaddr + ((Elf32_Phdr *)(program_header))->p_memsz - info->base_entry;

	// put .text addr into EDI
	val = info->base_entry - info->offset_loader;
	ft_memcpy(woody32 + 68, &(val), sizeof(uint32_t));

	// put Key value
	val = get_last_key(info->Key);
	ft_memcpy(woody32 + 76, &(val), sizeof(uint32_t));

	// put .text size
	val = info->offset_loader - info->base_entry - 4;
	ft_memcpy(woody32 + 83, &(val), sizeof(uint32_t));

	// put .text begin addr again again
//  	val = - 189 - offset;
// 	ft_memcpy(woody32 + 185, &(val), sizeof(uint32_t));

	// put decryption key
//  	val = get_last_key(info->Key);
// 	val = 0x42;
// 	ft_memcpy(woody32 + 69, &(val), sizeof(uint32_t));

}

static void		append_code32(t_info *info, void *new_file)
{
	// add .bss section to the physical file : DO nothing, just jump the section

	// edit shellcode with infos
	modify_woody(info, new_file);

	// append woody to the end of the .bss
	ft_memcpy(new_file + info->offset_woody, woody32, WOODY_SIZE);

	// edit jump shellcode
	replace_jmploader32(info, new_file + info->segment_data_header);

	// append jump to loader to the end of the woody shellcode
	ft_memcpy(new_file + info->offset_woody + WOODY_SIZE, jmp_end_woody, JMP32_SIZE);

}

static void		modify_loader(t_info *info)
{
	Elf32_Phdr	*header;
	uint32_t	new_rel;
	size_t		offset;

	header = get_last_load32(info->file);
	offset = header->p_vaddr + header->p_memsz - 35 - info->offset_loader; //+ (info->offset_loader & 1);

	// change first load : offset 13 dans le shellcode
	new_rel = offset;
	ft_memcpy(loader32 + 13, &new_rel, sizeof(uint32_t));

	// change last load: offset 47 dans le shellcode
	new_rel = offset;
	ft_memcpy(loader32 + 47, &new_rel, sizeof(uint32_t));

	// change jmp addr: offset 31 dans le shellcode
	new_rel = offset;
	ft_memcpy(loader32 + 31, &new_rel, sizeof(uint32_t));

	
}

static int32_t		inject_loader32(t_info *info, void *new_file)
{
	char		*inject;
	uint32_t	addr_to_jmp;

	if (!(inject = malloc(info->loader_size)))
		return (0);

	// calc old entry_point rel addr
	addr_to_jmp = (uint32_t)(info->base_entry - info->offset_loader - info->loader_size);
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
	info->funcs->replace_jmp_end_woody = &replace_jmploader32;
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
