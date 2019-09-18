#include "woody.h"

char	jmpD[] = //"\xe9\xff\xff\xff\xff\xff\xff\xff\xff";
"\x50\x51\x52\x41\x50\x41\x51\x41\x52\x41\x53\x50\x48\x31\xc0\x48\x31\xdb\x48\x31\xd2\x48\x83\xec\x10\xc7\x04\x24\x2e\x2e\x2e\x2e\xc7\x44\x24\x04\x57\x4f\x4f\x44\xc7\x44\x24\x08\x59\x2e\x2e\x2e\xc7\x44\x24\x0c\x2e\x0a\x00\x00\xba\x0e\x00\x00\x00\xb8\x01\x00\x00\x00\xbf\x01\x00\x00\x00\x48\x8d\x34\x24\x0f\x05\x48\x83\xc4\x10\x58\x41\x5b\x41\x5a\x41\x59\x41\x58\x5a\x59\x58";

void	print_woody(t_info *info, void *new_file)
{
	int		fd;

	if ((fd = open("woody", O_CREAT | O_TRUNC | O_WRONLY, 0755)) == -1)
		return ;
	write(fd, new_file, info->file_size); 
	close(fd);
}

void	print_packer(void *file, size_t size)
{
	int		fd;

	if ((fd = open("packer", O_CREAT | O_TRUNC | O_WRONLY, 0755)) == -1)
		return ;
	write(fd, file, size); 
	close(fd);
}

size_t		get_size_packed(t_info *info)
{
	// size = new_file + main header + programs header + dechiffreur
	size_t	ret;

	ret = info->file_size;
	ret += sizeof(Elf64_Ehdr);
	ret += sizeof(Elf64_Phdr) * 3;
	ret += sizeof(jmpD) - 1;
	return (ret);
}

void		set_program_header(Elf64_Phdr *ptr, Elf64_Word type, Elf64_Word flags, Elf64_Off offset, Elf64_Addr vaddr, Elf64_Addr paddr, Elf64_Xword filesz, Elf64_Xword memsz, Elf64_Xword align)
{
	ptr->p_type = type;
	ptr->p_flags = flags;
	ptr->p_offset = offset;
	ptr->p_vaddr = vaddr;
	ptr->p_paddr = paddr;
	ptr->p_filesz = filesz;
	ptr->p_memsz = memsz;
	ptr->p_align = align;
}

void		create_packer(t_info *info, void *original)
{
	Elf64_Ehdr	*old_header;
	Elf64_Ehdr	*new_header;
	Elf64_Phdr	*program_header;
	void		*packed;


	if (!(packed = malloc(get_size_packed(info))))
		return ;
	ft_bzero(packed, get_size_packed(info));
	new_header = (Elf64_Ehdr *)packed;
	old_header = (Elf64_Ehdr *)original;


// 	new_header->e_ident = old_header->e_ident;
	ft_memcpy(new_header->e_ident, old_header->e_ident, EI_NIDENT);
	new_header->e_type = old_header->e_type;
	new_header->e_machine = old_header->e_machine;
	new_header->e_version = old_header->e_version;
	new_header->e_entry = 0x1000;
	new_header->e_phoff = old_header->e_phoff;
	new_header->e_shoff = 0;
	new_header->e_flags = old_header->e_flags;
	new_header->e_ehsize = old_header->e_ehsize;
	new_header->e_phentsize = old_header->e_phentsize;
	new_header->e_phnum = 3;
	new_header->e_shentsize = 0;
	new_header->e_shnum = 0;
	new_header->e_shstrndx = 0;

	program_header = (Elf64_Phdr *)(packed + sizeof(Elf64_Ehdr));
	
	set_program_header(program_header, PT_PHDR, PF_R, 0x40, 0x40, 0x40, 0xA8, 0xA8, 0x8);
	program_header++;
	set_program_header(program_header, PT_LOAD, PF_R | PF_X, 0x1000, 0x1000, 0x1000, sizeof(jmpD) - 1, sizeof(jmpD) - 1, 0x1000);
	program_header++;
	set_program_header(program_header, PT_LOAD, PF_R, 0x2000, 0x2000, 0x2000, info->file_size, info->file_size, 0x1000);

	ft_memcpy(packed + new_header->e_entry, jmpD, sizeof(jmpD) - 1);
	ft_memcpy(packed + program_header->p_offset, original, info->file_size);
	print_packer(packed, get_size_packed(info));
}

void		create_woody(t_info *info)
{
	void	*new_file;

	if (!(new_file = malloc(info->file_size)))
		return ;
	ft_memcpy(new_file, info->file, info->offset_injection);
	if ((info->funcs->inject_code(info, new_file + info->offset_injection)) == 0)
		return ;
	ft_memcpy(new_file + info->offset_injection + info->injection_size, info->file + info->offset_injection + info->injection_size, info->file_size - info->offset_injection - info->injection_size);
	info->funcs->replace_headers(info, new_file);

	create_packer(info, new_file);
	print_woody(info, new_file);
	free(new_file);
}
