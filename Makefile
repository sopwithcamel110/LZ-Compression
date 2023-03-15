CC = clang
CFLAGS = -Wall -Wextra -Werror -Wpedantic
EXEC = encode decode
OBJS = io.o word.o trie.o
CLEAN = encode.o decode.o

all: $(EXEC)

encode: encode.o $(OBJS)
	$(CC) -o encode encode.o $(OBJS)

decode: decode.o $(OBJS)
	$(CC) -o decode decode.o $(OBJS)

$.o : %.c
	$(CC) $(CFLAGS) -c $<

clean: 
	rm -f $(EXEC) $(CLEAN) $(OBJS)

format:
	clang-format -i -style=file *.[ch]
