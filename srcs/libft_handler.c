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
