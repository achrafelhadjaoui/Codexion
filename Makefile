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
SRCS     := src/main.c src/threads/creating_threads.c src/threads/initialisations.c src/helper.c src/threads/threads_helper.c \
			src/threads/monitor.c \
			src/threads/destroying.c src/utils.c src/simulation/logic_handling.c \
			src/simulation/ordering_schedul.c src/simulation/release_dongles.c src/simulation/compile_debug_refactor.c \
			src/simulation/activity_helper.c src/simulation/scheduel_ordering_helper.c


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
