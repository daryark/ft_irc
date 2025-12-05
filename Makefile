GREEN = \033[0;32m
RE = \033[0m

SRC =	main.cpp Server.cpp Client.cpp Command.cpp Channel.cpp \
		Join.cpp Privmsg.cpp Registration.cpp Kick.cpp Part.cpp Mode.cpp\
		server_helpers.cpp helpers.cpp debug_file.cpp
HDR = Server.hpp Client.hpp CommandFactory.hpp Command.hpp Channel.hpp ErrorReply.hpp

SRC_F = src/
OBJ_F = obj/
HDR_F = incl/

HDR_PATHS = $(addprefix $(HDR_F), $(HDR))
VPATH = $(SRC_F)
OBJ = $(addprefix $(OBJ_F), $(SRC:%.cpp=%.o))

CXX = c++
CXX_FLAGS = -std=c++11 -Wall -Wextra -Werror -O3 -g -MMD -MP
NAME=ircserv

all:$(NAME)

$(NAME): $(OBJ)
	$(CXX) $(CXX_FLAGS) -o $@ $^
	@echo "$(GREEN)OK$(RE)"

$(OBJ_F)%.o: %.cpp Makefile
	@mkdir -p $(@D)
	$(CXX) $(CXX_FLAGS) -I$(HDR_F) -c $< -o $@

clean:
	@rm -rf $(OBJ_F)

fclean:	clean
	@rm -f $(NAME)

re:	fclean all

-include $(OBJ:.o=.d)

.PHONY:	all clean fclean re