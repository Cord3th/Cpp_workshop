


main:
	clang++ *.cpp -o main -std=c++17 -pthread -Werror

run: main
	./main

clean:
	rm -f *.o main
