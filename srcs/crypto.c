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
	//size_t		i;
	//int32_t		nb_laps;
	uint32_t	key;

	header = (Elf64_Phdr *)(info->file + info->segment_text_header);
	text = (uint32_t *)(new_file + info->base_entry - (header->p_vaddr - header->p_offset));

	//nb_laps = 0;
	key = info->Key;
	dprintf(1, "key: %d | text_size: %ld | text: %p\n", key, info->text_size, text);
	//while (nb_laps < NB_TIMING)
	//{
	//	i = 0;
	//	key = key_timing(key);
	//	while ((i + 1)* 4 < info->text_size)
	//	{
	//		text[i] ^= key;
	//		i++;
	//	}
	//	nb_laps++;
	//}

	__asm__ (
			"movq %0, %%r8\t\n"	//timing
			"movq %1, %%r9\t\n"	//nb_timing
			"movl %2, %%edx\t\n"	//key
			"movq %3, %%r10\t\n"	//text size RCX
			"movq %4, %%r11\t\n"	//text RDI
			"loop2:\t\n"
			"movq %%r10, %%rcx\t\n"
			"movq %%r11, %%rdi\t\n"	//text
			"loop1:\t\n"
			"movl (%%rdi), %%eax\t\n"
			"xor %%rdx, %%rax\t\n"
			"stos	%%eax,%%es:(%%rdi)\t\n"
			"sub    $4, %%ecx\t\n"
			"cmp    $0, %%ecx\t\n"
			"jg     loop1\t\n"
			"sub    %%r8d, %%edx\t\n"
			"dec    %%r9\t\n"
			"test   %%r9, %%r9\t\n"
			"jne    loop2\t\n"
			:
			:"g" (TIMING), "g" (NB_TIMING), "g" (key), "g" (info->text_size), "g" (text)
			://"r11", "r10", "rdx", "r8", "rcx", "rdi", "rdx", "rax"
	       );
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
