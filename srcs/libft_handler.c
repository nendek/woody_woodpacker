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
