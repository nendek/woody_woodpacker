#include "woody.h"

void			create_Key(t_info *info)
{
	struct timeval	time;

	gettimeofday(&time, NULL);
	srand(time.tv_usec);

	info->Key = rand();
}

static uint32_t	key_timing(uint32_t current_key)
{
	return (current_key - TIMING);
}

uint32_t		get_last_key(uint32_t master_Key)
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
void			encryption(t_info *info, void *new_file)
{
	uint32_t	*text;
	size_t		i;
	int32_t		nb_laps;
	uint32_t	key;

	text = (uint32_t *)(new_file + info->base_entry);

	nb_laps = 0;
	key = info->Key;
	while (nb_laps < NB_TIMING)
	{
		i = 0;
		key = key_timing(key);
		while ((i + 1)* 4 < info->text_size)
		{
			text[i] ^= key;
			i++;
		}
		nb_laps++;
	}
}
