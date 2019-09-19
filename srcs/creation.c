#include "woody.h"

# define ALIGN_APPEND 8

# define WOODY_SIZE sizeof(woody64) - 1
static char woody64[] = 
"\x48\x31\xc0\x48\x31\xdb\x48\x31\xd2\x48\x83\xec\x10\xc7\x04\x24\x2e\x2e\x2e\x2e\xc7\x44\x24\x04\x57\x4f\x4f\x44\xc7\x44\x24\x08\x59\x2e\x2e\x2e\xc7\x44\x24\x0c\x2e\x0a\x00\x00\xba\x0e\x00\x00\x00\xb8\x01\x00\x00\x00\xbf\x01\x00\x00\x00\x48\x8d\x34\x24\x0f\x05\x48\x83\xc4\x10";

# define JMPL_SIZE sizeof(jmploader) -1
static char jmploader[] = 
"\xe9\xff\xff\xff\xff";

void	print_woody(void *file, size_t size, char *name)
{
	int		fd;

	if ((fd = open(name, O_CREAT | O_TRUNC | O_WRONLY, 0755)) == -1)
		return ;
	write(fd, file, size); 
	close(fd);
}

Elf64_Phdr *get_last_load(void *file)
{
	Elf64_Phdr	*program_header;
	int			i = 0;

	program_header = (Elf64_Phdr *)(file + sizeof(Elf64_Ehdr));
	while (program_header->p_type != PT_LOAD)
	{
// 		dprintf(1, "%#x\n", program_header->p_type);
		program_header++;
		i++;
	}
	while (program_header->p_type == PT_LOAD)
		program_header++;
	program_header--;
	return (program_header);
}

void		replace_jmploader(t_info *info, Elf64_Phdr  *program_header)
{
	// adresse d'arrivee : offset_injection + la moitie (en dur = 0x24)
	// adresse relative : adresse du jump - adresse courante
	// adresse de depart : program_header->p_vaddr + + program_header->p_filesz + WOODY_SIZE - 1

	size_t		depart;
	size_t		arrive;
	uint32_t	rel = 0;

	depart = program_header->p_vaddr + program_header->p_memsz + WOODY_SIZE;
	arrive = info->offset_injection + 0x26;
	rel = (uint32_t)(arrive - depart);

	ft_memcpy(jmploader + 1, &(rel), sizeof(uint32_t));

}

void		append_code(t_info *info, void *new_file)
{
	Elf64_Phdr	*program_header;
	size_t		offset_append;

	program_header = get_last_load(new_file);

	offset_append = program_header->p_offset + program_header->p_memsz + ALIGN_APPEND;
	info->offset_append = offset_append;
	ft_memcpy(new_file + offset_append, woody64, WOODY_SIZE);
	replace_jmploader(info, program_header);
	ft_memcpy(new_file + offset_append + WOODY_SIZE, jmploader, JMPL_SIZE);

	program_header->p_filesz += info->bss_size + WOODY_SIZE + JMPL_SIZE + ALIGN_APPEND;
	program_header->p_memsz = program_header->p_filesz;
	dprintf(1, "offset append : %#lx || p_memsz : %#lx\n", offset_append, program_header->p_memsz);
}


void		create_woody(t_info *info)
{
	void		*new_file;
	Elf64_Phdr	*header;
	size_t		bss_size;

	header = get_last_load(info->file);
	bss_size = header->p_memsz - header->p_filesz;
	info->bss_size = bss_size;
	if (!(new_file = malloc(info->file_size + WOODY_SIZE + JMPL_SIZE + ALIGN_APPEND + bss_size)))
		return ;
	ft_bzero(new_file, info->file_size + WOODY_SIZE + JMPL_SIZE + ALIGN_APPEND + bss_size);

	ft_memcpy(new_file, info->file, info->offset_injection);
	if ((info->funcs->inject_code(info, new_file + info->offset_injection)) == 0)
		return ;

	ft_memcpy(new_file + info->offset_injection + info->injection_size, info->file + info->offset_injection + info->injection_size, info->file_size - info->offset_injection - info->injection_size);

	info->funcs->replace_headers(info, new_file);
	append_code(info, new_file);
	print_woody(new_file, info->file_size, "woody");
	print_woody(new_file, info->file_size + WOODY_SIZE + JMPL_SIZE + ALIGN_APPEND + info->bss_size, "packer");
	free(new_file);
}
