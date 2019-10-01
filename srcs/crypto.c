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

void			encryption64(t_info *info, void *new_file)
{
	Elf64_Phdr	*header;
	uint32_t	*text;
	size_t		i;
	int32_t		nb_laps;
	uint32_t	key;

	header = (Elf64_Phdr *)(info->file + info->segment_text_header);
	text = (uint32_t *)(new_file + info->base_entry - (header->p_vaddr - header->p_offset));

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

	//__asm__(
	//		"movq %1, %%r8\t\n"
	//		"movq %2, %%r9\t\n"
	//		"loop2:\t\n"
	//		//"lea %5, %%rdi\t\n"
	//		"loop1:\t\n"
	//		"movl (%%rdi), %%eax\t\n"
	//		"xor %%rdx, %%rax\t\n"
	//		"stos	%%eax,%%es:(%%rdi)\t\n"
	//		"sub    $0x4, %%ecx\t\n"
	//		"cmp    0x0, %%ecx\t\n"
	//		"jg     loop1\t\n"
	//		"add    %%r8d, %%edx\t\n"
	//		"dec    %%r9\t\n"
	//		"test   %%r9, %%r9\t\n"
	//		"jne    loop2\t\n"
	//		: "=d" (key), "=c" (info->text_size), "=D" (text)
	//		: "g" (TIMING), "g" (NB_TIMING) 			://"%rdx", "%r8", "%r9", "%rcx", "%rdi", "%rax"
	//       );
}

void			encryption32(t_info *info, void *new_file)
{
	Elf32_Phdr	*header;
	uint32_t	*text;
	size_t		i;
	int32_t		nb_laps;
	uint32_t	key;

	header = (Elf32_Phdr *)(info->file + info->segment_text_header);
	text = (uint32_t *)(new_file + info->base_entry - (header->p_vaddr - header->p_offset));

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
