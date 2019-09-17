#include "woody.h"

void	print_woody(t_info *info, void *new_file)
{
	int		fd;

	if ((fd = open("woody", O_CREAT | O_TRUNC | O_WRONLY, 0755)) == -1)
		return ;
	write(fd, new_file, info->file_size); 
	close(fd);
}

void		create_woody(t_info *info)
{
	void	*new_file;

	if (!(new_file = malloc(info->file_size)))
		return ;
	ft_memcpy(new_file, info->file, info->offset_injection);
	if ((info->funcs->inject_code(info, new_file + info->offset_injection)) == 0)
		return ;
	ft_memcpy(new_file + info->offset_injection + info->injection_size, info->file + info->offset_injection + info->injection_size, info->file_size - info->offset_injection - info->injection_size);
	info->funcs->replace_headers(info, new_file);
	print_woody(info, new_file);
	free(new_file);
}
