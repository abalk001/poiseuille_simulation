NAME = simple_shear
SRCS = main.c tools.c simulation.c
OBJS = $(SRCS:.c=.o)
CC = cc
CFLAGS = -Wall -Wextra -Werror 
LDFLAGS = -lm

all: $(NAME)
$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME) $(LDFLAGS)
	@echo "Build complete: $(NAME)"
%.o: %.c
	$(CC) $(CFLAGS) -c  $< -fsanitize=address -fsanitize=undefined -g -o $@
clean:
	rm -f $(OBJS)
	@echo "Object files removed"
fclean: clean
	rm -f $(NAME)
	rm -f simple_flow_trajectory.csv
	@echo "Executable and data files removed"
re: fclean all

.PHONY: all clean fclean re