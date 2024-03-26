CC=gcc
CFLAGS=-Wall -Wextra -Werror
TARGET=my_ls

all: $(TARGET)

$(TARGET): $(TARGET).c
	$(CC) $(CFLAGS) -o $(TARGET) $(TARGET).c

clean:
	rm -f $(TARGET)

fclean: clean
	rm -f *.o

.PHONY: all clean fclean
