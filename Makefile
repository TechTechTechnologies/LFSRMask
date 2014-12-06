
ifeq ($(OS),Windows_NT)
  SFML  = -lsfml-audio-s    \
          -lsfml-graphics-s \
          -lsfml-window-s   \
          -lsfml-system-s   \
          -lopengl32        \
          -lglu32
else
  SFML  = -lsfml-audio    \
          -lsfml-graphics \
          -lsfml-window   \
          -lsfml-system   \
          -lGL            \
          -lGLU
endif
SRC   = list.cpp
MAIN  =	main.cpp
OUT   =	lfsr.exe

all:
	echo "\n"
	g++ $(SRC) $(MAIN) -o $(OUT) $(LIB) $(SFML) -D SFML_STATIC

run: all
	./$(OUT)

debug:
	echo "\n"
	g++ $(SRC) $(MAIN) -g -o $(OUT) $(LIB) $(SFML) -D SFML_STATIC
	gdb $(OUT)

clean:
	rm *.o
