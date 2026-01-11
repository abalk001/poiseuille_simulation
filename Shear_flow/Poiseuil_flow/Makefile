NAME = poiseuille

CC = cc
CFLAGS = -Wall -Wextra -Werror -g
LDFLAGS = -lm

SRCDIR = src
INCDIR = include
OBJDIR = obj

SRCS = $(SRCDIR)/main.c \
       $(SRCDIR)/physics.c \
       $(SRCDIR)/simulation.c \
       $(SRCDIR)/validation.c \
       $(SRCDIR)/utils.c \
       $(SRCDIR)/output.c \
       $(SRCDIR)/config.c

OBJS = $(SRCS:$(SRCDIR)/%.c=$(OBJDIR)/%.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME) $(LDFLAGS)
	@echo "Build complete: $(NAME)"

$(OBJDIR)/%.o: $(SRCDIR)/%.c $(INCDIR)/poiseuille.h
	@mkdir -p $(OBJDIR)
	$(CC) $(CFLAGS) -I$(INCDIR) -c $< -o $@

clean:
	rm -rf $(OBJDIR)
	@echo "Object files removed"

fclean: clean
	rm -f $(NAME)
	rm -f poiseuille_trajectory.csv
	rm -f poiseuille_validation.csv
	@echo "Executable and data files removed"

re: fclean all

run: $(NAME)
	./$(NAME)

run-config: $(NAME)
	./$(NAME) configs/default.conf

compare: $(NAME)
	@echo "Running Poiseuille flow simulation..."
	./$(NAME)
	@echo "\nComparing with simple shear flow data..."
	@if [ -f ../simple_shear_flow/simple_shear_trajectory.csv ]; then \
		echo "Simple shear data found."; \
	else \
		echo "Run simple_shear first to generate comparison data."; \
	fi

.PHONY: all clean fclean re run run-config compare
