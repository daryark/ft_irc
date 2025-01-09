GREEN = \033[0;32m
RE = \033[0m

SRC =  main.cpp Irc.cpp
HDR = Irc.hpp

SRC_F = src/
OBJ_F = obj/
HDR_F = incl/

HDR_PATHS = $(addprefix $(HDR_F), $(HDR))
VPATH = $(SRC_F) #*don't forget to use it, OR specify src folder for compiler then - $(OBJ_F)%.o: $(SRC_F)%.cpp ...
OBJ = $(addprefix $(OBJ_F), $(SRC:%.cpp=%.o))

CXX = c++
CXX_FLAGS = -std=c++98 -Wall -Wextra -Werror -O3 -g
NAME=ircserv

all:$(NAME)

$(NAME): $(OBJ)
	$(CXX) $(CXX_FLAGS) -o $@ $^
	@echo "$(GREEN)OK$(RE)"

$(OBJ_F)%.o: %.cpp $(HDR_F) Makefile
	mkdir -p $(@D)
	$(CXX) $(CXX_FLAGS) -c $< -o $@

clean:
	@rm -rf $(OBJ_F)

fclean:	clean
	@rm -f $(NAME)

re:	fclean all

.PHONY:	all clean fclean re