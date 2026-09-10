CFLAGS = -g -std=c17 -Werror -Wall

main: ./build/main.o ./build/myheader.o
	gcc $(CFLAGS) $^ -o $@
./build/main.o: ./source/main.c ./source/myheader.h
	gcc $(CFLAGS) -c ./source/main.c -o $@
./build/myheader.o: ./source/myheader.c ./source/myheader.h
	gcc $(CFLAGS) -c ./source/myheader.c -o $@
rm:
	rm -f ./build/main.o; rm -f ./build/myheader.o;