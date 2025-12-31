NAME = poiseuille_simulation
SRCS = poiseuille_simulation.c sim.c tools.c
OBJS = $(SRCS:.c=.o)
CC = gcc
LDFLAGS = -lm

all: $(NAME)
$(NAME): $(OBJS)
	$(CC) $(CFLAGS) -o $(NAME) $(OBJS) $(LDFLAGS)
%.o: %.c header.h
	$(CC) $(CFLAGS) -c $< -o $@
clean:
	rm -f $(OBJS)
fclean: clean
	rm -f $(NAME)
re: fclean all
.PHONY: all clean fclean re