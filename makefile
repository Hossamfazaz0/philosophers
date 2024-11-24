NAME = philo

CC = cc 


SRC = main.c utils.c init.c philos_routine_func.c routine_functions.c

OBJ = $(SRC:.c=.o)

$(NAME): $(OBJ)
	$(CC) -pthread $(OBJ) -o $(NAME)
%.o: %.c
	$(CC)  -c $< -o $@

all: $(NAME)

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re