#include "woody.h"

void	print_woody(void *file, size_t size, char *name)
{
	int		fd;

	if ((fd = open(name, O_CREAT | O_TRUNC | O_WRONLY, 0755)) == -1)
		return ;
	write(fd, file, size); 
	close(fd);
}

void		create_Key(t_info *info)
{
	struct timeval	time;

	gettimeofday(&time, NULL);
	srand(time.tv_usec);

	info->Key = rand();
}


uint32_t	key_timing(uint32_t current_key)
{
	return (current_key - TIMING);
}

uint32_t	get_last_key(uint32_t master_Key)
{
	int			i;

	i = 0;
	while (i < NB_TIMING)
	{
		master_Key = key_timing(master_Key);
		i++;
	}
	return (master_Key);
}
void		encryption(t_info *info, void *new_file)
{
	uint32_t	*text;
	size_t		i;
	int32_t		nb_laps;
	uint32_t	key;
	size_t		size_text;

	size_text = info->offset_loader - info->base_entry;
	text = (uint32_t *)(new_file + info->base_entry);

	nb_laps = 0;
	key = info->Key;
	while (nb_laps < NB_TIMING)
	{
		i = 0;
		key = key_timing(key);
// 		key = 0x40c433ba;
		while ((i + 1)* 4 < size_text)
		{
			text[i] ^= key;
			i++;
		}
		nb_laps++;
	}
	dprintf(1, "key : %#x || last_key : %#x\n", key, get_last_key(info->Key));
}

void		create_woody(t_info *info)
{
	void		*new_file;
	size_t		new_file_size;


	// create and init new_file
	new_file_size = info->file_size + info->bss_size + info->woody_size;
	if (!(new_file = malloc(new_file_size)))
		return ;
	ft_bzero(new_file, new_file_size);

	// recopy headers and .text of the original file
	ft_memcpy(new_file, info->file, info->offset_loader);

	// add the loader code
	if ((info->funcs->inject_loader(info, new_file + info->offset_loader)) == 0)
		return ;

	// add the rest of the file until end .data
	ft_memcpy(new_file + info->offset_loader + info->loader_size, info->file + info->offset_loader + info->loader_size, info->end_data_seg - (info->offset_loader + info->loader_size));

	// add the .bss and the dechiffreur
	info->funcs->append_code(info, new_file);

	// complete the file
	ft_memcpy(new_file + info->offset_woody + info->woody_size, info->file + info->end_data_seg, info->file_size - info->end_data_seg);

	// replace headers to make them work with loader
	info->funcs->replace_headers(info, new_file);

	// encrypt .text section
	encryption(info, new_file);

	// write the file
	print_woody(new_file, new_file_size, "woody");
	free(new_file);
}
