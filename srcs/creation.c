#include "woody.h"

char sht[] = 
"\x50\x51\x52\x41\x50\x41\x51\x41\x52\x41\x53\x50\x48\x31\xc0\x48\x31\xdb\x48\x31\xd2\x48\x83\xec\x10\xc7\x04\x24\x2e\x2e\x2e\x2e\xc7\x44\x24\x04\x57\x4f\x4f\x44\xc7\x44\x24\x08\x59\x2e\x2e\x2e\xc7\x44\x24\x0c\x2e\x0a\x00\x00\xba\x0e\x00\x00\x00\xb8\x01\x00\x00\x00\xbf\x01\x00\x00\x00\x48\x8d\x34\x24\x0f\x05\x48\x83\xc4\x10\x58\x41\x5b\x41\x5a\x41\x59\x41\x58\x5a\x59\x58";

void	print_woody(void *file, size_t size, char *name)
{
	int		fd;

	if ((fd = open(name, O_CREAT | O_TRUNC | O_WRONLY, 0755)) == -1)
		return ;
	write(fd, file, size); 
	close(fd);
}

// void		create_woody(t_info *info)
// {
// 	void	*new_file;
// 
// 	if (!(new_file = malloc(info->file_size)))
// 		return ;
// 	ft_memcpy(new_file, info->file, info->offset_injection);
// 	if ((info->funcs->inject_code(info, new_file + info->offset_injection)) == 0)
// 		return ;
// 	ft_memcpy(new_file + info->offset_injection + info->injection_size, info->file + info->offset_injection + info->injection_size, info->file_size - info->offset_injection - info->injection_size);
// 	info->funcs->replace_headers(info, new_file);
// 	create_packer(info, new_file);
// 	print_woody(info, new_file);
// 	free(new_file);
// }

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

void		append_code(t_info *info, void *new_file)
{
	Elf64_Phdr	*program_header;
	size_t		offset_append;

	program_header = get_last_load(new_file);

	offset_append = program_header->p_offset + program_header->p_filesz;
	info->offset_append = offset_append;
	ft_memcpy(new_file + offset_append, sht, sizeof(sht) -1);

	program_header->p_filesz += sizeof(sht) - 1;
	program_header->p_memsz += sizeof(sht) - 1;
}


void		create_woody(t_info *info)
{
	void	*new_file;

	if (!(new_file = malloc(info->file_size + sizeof(sht) - 1)))
		return ;
	ft_memcpy(new_file, info->file, info->offset_injection);

	if ((info->funcs->inject_code(info, new_file + info->offset_injection)) == 0)
		return ;

	ft_memcpy(new_file + info->offset_injection + info->injection_size, info->file + info->offset_injection + info->injection_size, info->file_size - info->offset_injection - info->injection_size);

	info->funcs->replace_headers(info, new_file);
	append_code(info, new_file);
// 	create_packer(info, new_file);
	print_woody(new_file, info->file_size, "woody");
	print_woody(new_file, info->file_size + sizeof(sht) - 1, "packer");
	free(new_file);
}
