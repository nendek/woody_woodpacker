# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: arobion <marvin@42.fr>                     +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2019/06/11 17:36:04 by arobion           #+#    #+#              #
#    Updated: 2019/07/10 17:56:49 by arobion          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

CC = gcc
CFLAGS = -Wall -Wextra -Werror

NAME = woody_woodpacker
FILES = woody.c creation.c crypto.c libft_handler.c \
		get_case64.c handle_64.c \
		get_case32.c handle_32.c 

SRCS_DIR = srcs
SRCS = $(addprefix $(SRCS_DIR)/,$(FILES))

INCS_DIR = includes
INCS = -I $(INCS_DIR)

OBJS_DIR = objs
OBJS = $(addprefix $(OBJS_DIR)/,$(FILES:%.c=%.o))

all: $(NAME)

$(NAME): $(OBJS_DIR) $(OBJS)
		$(CC) $(CFLAGS) $(INCS) -o $(NAME) $(OBJS)

$(OBJS_DIR):
		mkdir -p $(OBJS_DIR)

$(OBJS_DIR)/%.o: $(SRCS_DIR)/%.c
		$(CC) $(CFLAGS) $(INCS) -c -o $@ $<

clean:
		rm -rf objs

fclean: clean
		rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
