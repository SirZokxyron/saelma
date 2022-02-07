# === Alias === #

all: saelma
run: saelma
	./saelma 0
debug: saelma
	./saelma 1

# === Compilation rules === #

CC = clang
CFLAGS = -Werror -Wextra -Wall -g # -O3

%: src/%.o
	$(CC) $(CFLAGS) $^ -o $@
%.o: src/%.c
	$(CC) $(CFLAGS) $< -c

# === Links === #

saelma: src/saelma.o src/shell.o

# === Utilities === #

clean:
	rm src/*.o

clear:
	rm saelma
