CFLAGS = -g -std=c17 -Werror -Wall

main: ./build/main.o ./build/myheader.o ./build/builtins.o ./build/vars.o
	gcc $(CFLAGS) $^ -o $@
./build/main.o: ./source/main.c ./source/myheader.h
	gcc $(CFLAGS) -c ./source/main.c -o $@
./build/myheader.o: ./source/myheader.c ./source/myheader.h
	gcc $(CFLAGS) -c ./source/myheader.c -o $@
./build/builtins.o: ./source/builtins.c ./source/builtins.h
	gcc $(CFLAGS) -c ./source/builtins.c -o $@
./build/vars.o: ./source/vars.c ./source/vars.h
	gcc $(CFLAGS) -c ./source/vars.c -o $@
rm:
	rm -f ./build/*.o