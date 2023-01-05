server: server.c
	gcc -o server server.c -ljansson
clean:
	@rm -f server *.o
