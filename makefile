# === Alias === #
all: saelma
run: saelma
	./saelma 0
debug: saelma
	./saelma 1

# === Compilation rules === #
CC = clang
CFLAGS = -Werror -Wextra -Wall #-O3

%: %.o
	$(CC) $(CFLAGS) $^ -o $@
%.o: %.c
	$(CC) $(CFLAGS) $< -c

# === Links === #
saelma: saelma.o shell.o

# === Utilities === #
clean:
	rm *.o

clear:
	rm saelma