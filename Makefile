main: main.cpp
	g++ main.cpp src/glad.c -o main -Iinclude -lglfw -ldl -lGL
	echo "Run ./main to start the program!"
clean:
	rm main
	echo "Removed main binary"
