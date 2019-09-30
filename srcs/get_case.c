#include "woody.h"

// static size_t		get_padding_size(Elf64_Phdr *program_header) { return (program_header->p_align - (program_header->p_filesz % program_header->p_align)); }
static size_t       get_padding_size(t_info *info, Elf64_Phdr *program_header) 
{
    Elf64_Phdr  *crawler;
    int32_t     i;  
    size_t      next_offset;
    int32_t     found;

    next_offset = -1; 
    crawler = (Elf64_Phdr *)(info->file + sizeof(Elf64_Ehdr));
    i = 0;
    found = 0;
    while (i < info->nb_hp)
    {   
        if ((crawler != program_header) && (crawler->p_offset > program_header->p_offset + program_header->p_filesz))
        {   
            if (next_offset > crawler->p_offset)
            {   
                next_offset = crawler->p_offset;
                found = 1;
            }   
        }   
        i++;
        crawler++;
    }   
    if (found == 0)
        return (0);
    return (next_offset - (program_header->p_offset + program_header->p_filesz));
}

static int32_t		save_place_bss(t_info *info)
{
	Elf64_Phdr	*header;

	header = get_last_load64(info->file);
	info->segment_data_header = (size_t)header - (size_t)(info->file);
	info->offset_woody_file = header->p_offset + header->p_memsz;
	info->offset_woody_mem = header->p_vaddr + header->p_memsz;
	return (0);
}

static int32_t		save_place_padding(t_info *info, Elf64_Phdr *program_header, int32_t nb_segment)
{
	program_header++;
	nb_segment++;
	while (nb_segment < info->nb_hp)
	{
		if (program_header->p_type == PT_LOAD)
		{
			if (get_padding_size(info, program_header) > info->woody_size)
			{
				info->offset_woody_file = program_header->p_offset + program_header->p_filesz;
				info->offset_woody_mem = program_header->p_vaddr + program_header->p_filesz;
				info->segment_data_header = (size_t)((size_t)program_header - (size_t)(info->file));
				return (0);
			}
		}
		program_header++;
		nb_segment++;
	}
	return (1);
}

static int32_t		save_place_3(t_info *info, Elf64_Phdr *program_header)
{

	if (get_padding_size(info, program_header) < info->woody_size + (15 * 2)) // TODO : get define 15 = push all size
		return (1);
	if (program_header == get_last_load64(info->file))
		return (1);
	
	info->offset_woody_file = program_header->p_offset + program_header->p_filesz;
	info->offset_woody_mem = program_header->p_vaddr + program_header->p_filesz;
	info->segment_data_header = (size_t)((size_t)program_header - (size_t)(info->file));
	info->offset_loader_file = program_header->p_offset + program_header->p_filesz;
	info->offset_loader_mem = program_header->p_vaddr + program_header->p_filesz;
	return (0);
}

static int32_t		save_place_text(t_info *info, Elf64_Phdr *program_header)
{
	// check segment is Executable
	if ((program_header->p_flags & PF_X) != PF_X)
		return (1);
	if (get_padding_size(info, program_header) < info->loader_size)
		return (1);

	info->offset_loader_file = program_header->p_offset + program_header->p_filesz;
	info->offset_loader_mem = program_header->p_vaddr + program_header->p_filesz;
	return (0);
}

int32_t		get_case_1(t_info *info)
{
	Elf64_Phdr	*program_header;
	int32_t		i;

	i = 0;
	program_header = (Elf64_Phdr *)(info->file + sizeof(Elf64_Ehdr));
	while (i < info->nb_hp)
	{
		if (program_header->p_type == PT_LOAD)
			// enough in .text for loader + enough in padding for woody
			if ((save_place_text(info, program_header) == 0) && (save_place_padding(info, program_header, i) == 0))
				return (0);
		i++;
		program_header++;
	}
	return (1);
}

int32_t		get_case_2(t_info *info)
{
	Elf64_Phdr	*program_header;
	int32_t		i;

	i = 0;
	program_header = (Elf64_Phdr *)(info->file + sizeof(Elf64_Ehdr));
	while (i < info->nb_hp)
	{
		if (program_header->p_type == PT_LOAD)
			// enough in .text for loader + enough in bss for woody
			if ((save_place_text(info, program_header) == 0) && (save_place_bss(info) == 0))
				return (0);
		i++;
		program_header++;
	}
	return (1);
}

int32_t		get_case_3(t_info *info)
{
	Elf64_Phdr	*program_header;
	int32_t		i;
	
	i = 0;
	program_header = (Elf64_Phdr *)(info->file + sizeof(Elf64_Ehdr));
	while (i < info->nb_hp)
	{
		if (program_header->p_type == PT_LOAD)
			// enough in padding for woody_loader
			if (save_place_3(info, program_header) == 0)
				return (0);
		i++;
		program_header++;
	}
	return (1);
}

int32_t		get_case_4(t_info *info)
{
	Elf64_Phdr	*program_header;

	program_header = get_last_load64(info->file);

	info->offset_woody_file = program_header->p_offset + program_header->p_memsz;
	info->offset_woody_mem = program_header->p_vaddr + program_header->p_memsz;
	info->segment_data_header = (size_t)((size_t)program_header - (size_t)(info->file));
	info->offset_loader_file = program_header->p_offset + program_header->p_memsz;
	info->offset_loader_mem = program_header->p_vaddr + program_header->p_memsz;
	return (0);	
}
