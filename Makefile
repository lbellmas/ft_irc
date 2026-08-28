NAME	= ircserv
CXX		= c++
CFLAGS	= -Wextra -Werror -Wall -std=c++98

# Todos los .cpp en el directorio actual
SRC		= main.cpp Server.cpp Client.cpp utils.cpp Channel.cpp
HPP		= Server.hpp Client.hpp utils.hpp Channel.hpp
# Genera los .o correspondientes
OBJ		= $(SRC:.cpp=.o)

all: $(NAME)

$(NAME): $(OBJ) 
	$(CXX) $(CFLAGS) $(OBJ) -o $(NAME)

# Regla genérica para compilar .cpp -> .o
%.o: %.cpp $(HPP) Makefile
	$(CXX) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re