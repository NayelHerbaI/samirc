# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: nherbal <nherbal@student.42.fr>            +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/03/21 20:28:19 by naherbal          #+#    #+#              #
#    Updated: 2024/12/30 06:27:08 by nherbal          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

SRC	=		src/main.cpp				\
			src/setup_socket.cpp		\
			src/client_handling.cpp		\
			src/handleCommand.cpp		\
			src/parseCommand.cpp		\
			src/sendMessage.cpp			\
			src/signalHandling.cpp		\
			src/debug.cpp				\
			src/commands/pass.cpp		\
			src/commands/user.cpp		\
			src/commands/join.cpp		\
			src/commands/ping.cpp		\
			src/commands/privmsg.cpp	\
			src/commands/part.cpp		\
			src/commands/oper.cpp		\
			src/commands/whois.cpp		\
			src/commands/kick.cpp		\
			src/commands/invite.cpp		\
			src/commands/mode.cpp		\
			src/commands/topic.cpp		\

OBJ		=	$(SRC:.c=.o)

NAME	=	ircserv	

CFLAGS	=	-Wall -Wextra -Werror -std=c++98

RM		=	rm -rf

all:	$(NAME)

$(NAME):
		c++ $(FLAGS) $(SRC) -o $(NAME)

fclean:
		$(RM) $(NAME)

re:		fclean all

.PHONY:	all clean fclean re
