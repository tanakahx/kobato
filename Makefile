main: kobato.h main.cc
	g++ -std=c++17 main.cc -o main

clean:
	-rm main