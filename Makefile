# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: aelhadja <aelhadja@student.1337.ma>        +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2026/09/05 12:03:42 by aelhadja          #+#    #+#              #
#    Updated: 2026/09/05 12:03:45 by aelhadja         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

CC       := cc
CFLAGS   := -Wall -Wextra -Werror -pthread

NAME     := codexion
TARGET   := $(NAME)
SRCS     := src/main.c src/threads/creating_threads.c src/threads/initialisations.c src/helper.c \
			src/utils.c src/simulation/edf_handling.c src/simulation/fifo_handling.c \
			src/simulation/ordering_schedul.c src/simulation/release_dongles.c src/simulation/compile_debug_refactor.c

# SRCS :=
#     src/main.c
#     src/threads/creating_threads.c
#     src/threads/initilisation.c
#     src/threads/utils.c
#     src/simulation/edf_handling.c
#     src/simulation/fifo_handling.c
#     src/simulation/ordering_schedul.c

OBJS     := $(SRCS:.c=.o)

.PHONY: all clean fclean re

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@

%.o: %.c src/head.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all
