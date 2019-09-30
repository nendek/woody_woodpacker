#include "woody.h"

static void	print_woody(void *file, size_t size, char *name)
{
	int		fd;

	if ((fd = open(name, O_CREAT | O_TRUNC | O_WRONLY, 0755)) == -1)
		return ;
	write(fd, file, size); 
	close(fd);
}

void		handle_case1(t_info *info)
{
	void		*new_file;
	size_t		new_file_size;

	// create and init new_file
	new_file_size = info->file_size;
	if (!(new_file = malloc(new_file_size)))
		return ;
	ft_bzero(new_file, new_file_size);

	// recopy headers and .text of the original file
	ft_memcpy(new_file, info->file, info->offset_loader_file);

	// add the loader code
	if ((info->funcs->inject_loader(info, new_file + info->offset_loader_file)) == 0)
		return ;

	// recopy file until woody_injection
	ft_memcpy(new_file + info->offset_loader_file + info->loader_size, info->file + info->offset_loader_file + info->loader_size, info->offset_woody_file - (info->offset_loader_file + info->loader_size));

	// inject woody
	// TODO : make it pointer to funcs
	inject_woody(info, new_file + info->offset_woody_file);

	// complete the file
	ft_memcpy(new_file + info->offset_woody_file + info->woody_size, info->file + info->offset_woody_file + info->woody_size, info->file_size - (info->offset_woody_file + info->woody_size));

	// replace headers to make them work with loader
	info->funcs->replace_headers(info, new_file);

	// encrypt .text section
	encryption(info, new_file);

	// write the file
	print_woody(new_file, new_file_size, "woody");
	free(new_file);
}

void		handle_case2(t_info *info)
{
	void		*new_file;
	size_t		new_file_size;

	// create and init new_file
	new_file_size = info->file_size + info->bss_size + info->woody_size;
	if (!(new_file = malloc(new_file_size)))
		return ;
	ft_bzero(new_file, new_file_size);

	// recopy headers and .text of the original file
	ft_memcpy(new_file, info->file, info->offset_loader_file);

	// add the loader code
	if ((info->funcs->inject_loader(info, new_file + info->offset_loader_file)) == 0)
		return ;

	// add the rest of the file until end .data
	ft_memcpy(new_file + info->offset_loader_file + info->loader_size, info->file + info->offset_loader_file + info->loader_size, info->end_data_seg - (info->offset_loader_file + info->loader_size));

	// add the .bss and the dechiffreur
	info->funcs->append_woody(info, new_file);

	// complete the file
	ft_memcpy(new_file + info->offset_woody_file + info->woody_size, info->file + info->end_data_seg, info->file_size - info->end_data_seg);

	// replace headers to make them work with loader
	info->funcs->replace_headers(info, new_file);

	// encrypt .text section
	encryption(info, new_file);

	// write the file
	print_woody(new_file, new_file_size, "woody");
	free(new_file);
}

void		handle_case3(t_info *info)
{
	void		*new_file;
	size_t		new_file_size;

	// create and init new_file
	new_file_size = info->file_size;
	if (!(new_file = malloc(new_file_size)))
		return ;
	ft_bzero(new_file, new_file_size);

	// recopy headers and .text of the original file
	ft_memcpy(new_file, info->file, info->offset_woody_file);

	// inject woody_loader
	append_woody_loader(info, new_file);

	// complete the file
	ft_memcpy(new_file + info->offset_woody_file + info->woody_size + (15 * 2), info->file + info->offset_woody_file + info->woody_size + (15 * 2), new_file_size - (info->offset_woody_file + info->woody_size + (15 * 2)));

	// replace headers to make them work with loader
	info->funcs->replace_headers(info, new_file);

	// encrypt .text section
	encryption(info, new_file);

	// write the file
	print_woody(new_file, new_file_size, "woody");
	free(new_file);
}

void		handle_case4(t_info *info)
{
	void		*new_file;
	size_t		new_file_size;

	// create and init new_file
	new_file_size = info->file_size + info->bss_size + info->woody_size + (15 * 2);
	if (!(new_file = malloc(new_file_size)))
		return ;
	ft_bzero(new_file, new_file_size);

	// recopy everything until end data seg
	ft_memcpy(new_file, info->file, info->end_data_seg);

	// inject woody_loader
	append_woody_loader(info, new_file);

	// complete the file
	ft_memcpy(new_file + info->offset_woody_file + info->woody_size + (15 * 2), info->file + info->end_data_seg, new_file_size - (info->offset_woody_file + info->woody_size + (15 * 2)));

	// replace headers to make them work with loader
	info->funcs->replace_headers(info, new_file);

	// encrypt .text section
	encryption(info, new_file);

	// write the file
	print_woody(new_file, new_file_size, "woody");
	free(new_file);
}

void		create_woody(t_info *info)
{
// 	dprintf(1, "mode : %#x\n", info->injection_mode);

	if (info->injection_mode == WOODY_PADDING)
		handle_case1(info);
	else if (info->injection_mode == WOODY_BSS)
		handle_case2(info);
	else if (info->injection_mode == DOUBLE_PADDING)
		handle_case3(info);
	else if (info->injection_mode == DOUBLE_BSS)
		handle_case4(info);
}
