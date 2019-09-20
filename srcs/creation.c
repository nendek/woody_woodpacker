#include "woody.h"

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

	program_header = (Elf64_Phdr *)(file + sizeof(Elf64_Ehdr));
	while (program_header->p_type != PT_LOAD)
		program_header++;
	while (program_header->p_type == PT_LOAD)
		program_header++;
	program_header--;
	return (program_header);
}

void		replace_jmploader(t_info *info, Elf64_Phdr  *program_header)
{
	// adresse d'arrivee : offset_loader + la moitie (en dur = 0x24)
	// adresse relative : adresse du jump - adresse courante
	// adresse de depart : program_header->p_vaddr + + program_header->p_filesz + WOODY_SIZE - 1

	size_t		depart;
	size_t		arrive;
	uint32_t	rel = 0;

	depart = program_header->p_vaddr + program_header->p_memsz + WOODY_SIZE;
	arrive = info->offset_loader + 0x26;
	rel = (uint32_t)(arrive - depart);

	ft_memcpy(jmploader + 1, &(rel), sizeof(uint32_t));

}

void		append_code(t_info *info, void *new_file)
{
	// append woody to the end of the .bss
	ft_memcpy(new_file + info->offset_woody, woody64, WOODY_SIZE);

	// edit jump shellcode
	replace_jmploader(info, new_file + info->segment_data_header);

	// append jump to loader to the end of the woody shellcode
	ft_memcpy(new_file + info->offset_woody + WOODY_SIZE, jmploader, JMPL_SIZE);

}


void		create_woody(t_info *info)
{
	void		*new_file;
	size_t		new_file_size;

	// get usefull infos : TODO move it in get_elf64_zone
	Elf64_Phdr	*header;
	header = get_last_load(info->file);
	info->offset_woody = header->p_offset + header->p_memsz + ALIGN_APPEND;
	info->woody_size = WOODY_SIZE + JMPL_SIZE;


	// create and init new_file
	new_file_size = info->file_size + info->bss_size + ALIGN_APPEND + info->woody_size;
	if (!(new_file = malloc(new_file_size)))
		return ;
	ft_bzero(new_file, new_file_size);

	// recopy headers and .text of the original file
	ft_memcpy(new_file, info->file, info->offset_loader);

	// add the loader code
	if ((info->funcs->inject_loader(info, new_file + info->offset_loader)) == 0)
		return ;
	// add the rest of the original file
	ft_memcpy(new_file + info->offset_loader + info->loader_size, info->file + info->offset_loader + info->loader_size, info->file_size - info->offset_loader - info->loader_size);

	// add the dechiffreur (woody) at the end of the code
	append_code(info, new_file);

	// replace headers to make them work with loader
	info->funcs->replace_headers(info, new_file);

	// write the file
	// 	print_woody(new_file, info->file_size, "woody");
	print_woody(new_file, new_file_size, "packer");
	free(new_file);
}
