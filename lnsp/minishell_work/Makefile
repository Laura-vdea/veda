# 컴파일러
CC = gcc
CFLAGS = -W -Wall -Werror
TARGET = minishell
SRCS = main.c is_bultin.c parsing.c run_command.c
HEADERS = minishell.h
OBJS = $(SRCS:.c=.o)
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(TARGET)

re: fclean all

.PHONY: all clean fclean re
