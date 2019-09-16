#include "woody.h"

void	ft_memcpy(void *dest, void *src, size_t size)
{
	size_t i = 0;

	while (i < size)
	{
		((char *)dest)[i] = ((char *)src)[i];
		i++;
	}
}

char	*ft_strcat(char *dest, const char *src)
{
	int		i;
	int		j;

	i = 0;
	j = 0;
	while (dest[i] != '\0')
		i++;
	while (src[j] != '\0')
	{
		dest[i] = src[j];
		i++;
		j++;
	}
	if (src[j] == '\0')
		dest[i] = src[j];
	return (dest);	
}

void		ft_bzero(char *str, size_t len)
{
	size_t		i;

	i = 0;
	while (i < len)
		str[i++] = '\0';
}

int32_t		inject_code(t_info *info, void *new_file)
{
	char		*inject;
	uint32_t	addr_to_jmp;

	if (!(inject = malloc(get_injection32_size())))
		return (0);
	addr_to_jmp = (uint32_t)(info->base_entry - info->offset_injection - get_injection32_size());
	ft_memcpy(jmp32 + 1, &(addr_to_jmp), sizeof(uint32_t));
	ft_strcat(inject, pusha);
	ft_strcat(inject, shellcode32);
	ft_strcat(inject, popa);
	ft_strcat(inject, jmp32);
	ft_memcpy(new_file, inject, get_injection32_size());
	return (1);
}

void	print_woody(t_info *info, void *new_file)
{
	int		fd;

	if ((fd = open("woody", O_CREAT | O_TRUNC | O_WRONLY, 0755)) == -1)
		return ;
	write(fd, new_file, info->file_size); 
	close(fd);
}

void		replace_headers(t_info *info, void *new_file)
{
	Elf32_Ehdr *main_header;
	Elf32_Phdr *program_header;

	main_header = (Elf32_Ehdr *)new_file;
	program_header = (Elf32_Phdr *)(new_file + sizeof(Elf32_Ehdr));

	main_header->e_entry = info->offset_injection;
	program_header = (Elf32_Phdr *)(new_file + info->segment_injection_offset);
	program_header->p_filesz += get_injection32_size();
	program_header->p_memsz += get_injection32_size();
}

void		create_woody(t_info *info)
{
	void	*new_file;

	if (!(new_file = malloc(info->file_size)))
		return ;
	ft_memcpy(new_file, info->file, info->offset_injection);
	if ((inject_code(info, new_file + info->offset_injection)) == 0)
		return ;
	ft_memcpy(new_file + info->offset_injection + get_injection32_size(), info->file + info->offset_injection + get_injection32_size(), info->file_size - info->offset_injection - get_injection32_size());
	replace_headers(info, new_file);
	print_woody(info, new_file);
	free(new_file);
}
