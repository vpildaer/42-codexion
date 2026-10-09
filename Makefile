# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: vpildaer <vpildaer@student.42belgium.be    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2026/10/09 15:28:55 by vpildaer          #+#    #+#              #
#    Updated: 2026/10/09 18:15:42 by vpildaer         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

CC 		= gcc

CFLAGS 	= -Wall -Wextra -Werror -pthread

RM		= rm -f

NAME 	= codexion_exec

SRCS	= ./codexion/main.c ./codexion/parser.c \
			./codexion/heap.c ./codexion/heap_utils.c ./codexion/heap_utils2.c \
			./codexion/init_threads.c ./codexion/init_routine.c \
			./codexion/routine.c ./codexion/routine_utils.c  \
			./codexion/routine_utils2.c

OBJS	= $(SRCS:.c=.o)

all: $(NAME)

$(NAME): $(OBJS) 
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

$(OBJS): codexion/codexion.h

clean:
	$(RM) $(OBJS)

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re