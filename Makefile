# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: vpildaer <vpildaer@student.42belgium.be    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2026/10/09 15:28:55 by vpildaer          #+#    #+#              #
#    Updated: 2026/10/21 15:45:25 by vpildaer         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

CC 		= gcc

CFLAGS 	= -Wall -Wextra -Werror -pthread

RM		= rm -f

NAME 	= codexion

SRCS	= ./codexion_dir/main.c ./codexion_dir/parser.c \
			./codexion_dir/heap.c ./codexion_dir/heap_utils.c \
			./codexion_dir/heap_utils2.c \
			./codexion_dir/init_threads.c ./codexion_dir/init_routine.c \
			./codexion_dir/routine.c ./codexion_dir/routine_utils.c  \
			./codexion_dir/routine_utils2.c

OBJS	= $(SRCS:.c=.o)

all: $(NAME)

$(NAME): $(OBJS) 
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

$(OBJS): codexion_dir/codexion.h

clean:
	$(RM) $(OBJS)

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re