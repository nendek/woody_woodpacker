#include "woody.h"

void	print_woody(void *file, size_t size, char *name)
{
	int		fd;

	if ((fd = open(name, O_CREAT | O_TRUNC | O_WRONLY, 0755)) == -1)
		return ;
	write(fd, file, size); 
	close(fd);
}


void		create_woody(t_info *info)
{
	void		*new_file;
	size_t		new_file_size;


	// create and init new_file
	new_file_size = info->file_size + info->bss_size + ALIGN_APPEND + info->woody_size;
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
// 	dprintf(1, "total_size = %#lx\n", new_file_size);
// 	dprintf(1, "dep new_file : %#lx || dep old_file : %#lx || size : %#lx\n", info->offset_woody + info->woody_size, end_data_seg, info->file_size - end_data_seg);
	ft_memcpy(new_file + info->offset_woody + info->woody_size, info->file + info->end_data_seg, info->file_size - info->end_data_seg);

	// replace headers to make them work with loader
	info->funcs->replace_headers(info, new_file);

	// write the file
	// 	print_woody(new_file, info->file_size, "woody");
	print_woody(new_file, new_file_size, "packer");
	free(new_file);
}
