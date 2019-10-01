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

void		ft_bzero(char *str, size_t len)
{
	size_t		i;

	i = 0;
	while (i < len)
		str[i++] = '\0';
}

void		*move_ptr(t_info *info, void *ptr, size_t to_add)
{
	if ((size_t)ptr + to_add >= (size_t)(info->file + info->file_size))
	{
		dprintf(2, "Error, file bad formated\n");
		return (NULL);
	}
	return ((void *)((size_t)ptr + to_add));
}
