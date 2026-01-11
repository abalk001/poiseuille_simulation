NAME = simple_shear

CC = cc
CFLAGS = -Wall -Wextra -Werror -g -fsanitize=address -fsanitize=undefined
LDFLAGS = -lm -fsanitize=address -fsanitize=undefined

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

$(OBJDIR)/%.o: $(SRCDIR)/%.c $(INCDIR)/shear.h
	@mkdir -p $(OBJDIR)
	$(CC) $(CFLAGS) -I$(INCDIR) -c $< -o $@

clean:
	rm -rf $(OBJDIR)
	@echo "Object files removed"

fclean: clean
	rm -f $(NAME)
	rm -f simple_shear_trajectory.csv
	rm -f validation_input.csv
	@echo "Executable and data files removed"

re: fclean all

run: $(NAME)
	./$(NAME)

test-configs: $(NAME)
	@echo "Testing default configuration..."
	./$(NAME) configs/default.conf
	@echo "\nTesting high deflation..."
	./$(NAME) configs/high_deflation.conf
	@echo "\nTesting viscosity contrast..."
	./$(NAME) configs/viscosity_contrast.conf

help:
	@echo "Usage: ./$(NAME) [config_file]"
	@echo "Available configurations:"
	@echo "  configs/default.conf          - Default parameters"
	@echo "  configs/high_deflation.conf   - Strongly deflated vesicle"
	@echo "  configs/viscosity_contrast.conf - High viscosity ratio"
	@echo "  configs/low_shear.conf        - Low shear rate"
	@echo "  configs/paper_fig2.conf       - Match paper Figure 2"

.PHONY: all clean fclean re run test-configs help
