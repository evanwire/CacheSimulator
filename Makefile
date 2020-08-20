all: cache

cache:
	gcc -g -Wall -lm -Werror -fsanitize=address -std=c11 cache.c -o cache

clean:
	rm -rf cache
