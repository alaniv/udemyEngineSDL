CFLAGS=-std=c++14 -Wfatal-errors -Wall -Werror -Wextra -pedantic -g
LDIR=-L./lib/LUA -L./lib/SDL2
IDIR=-I./include -I./src -I./include/LUA
LIBS=-llua53 -lmingw32 -lSDL2main -lSDL2_image -lSDL2_ttf -lSDL2_mixer -lSDL2
OBJS=./obj/Game.o ./obj/Entity.o ./obj/EntityManager.o ./obj/Main.o ./obj/Map.o ./obj/AssetManager.o ./obj/TextureManager.o ./obj/Animation.o ./obj/Collision.o ./obj/FontManager.o
.PHONY: build clean run
build: $(OBJS)
	g++ $(CFLAGS) $^ -o ./bin/Main.exe $(IDIR) $(LDIR) $(LIBS)

clean:
	rm -f ./bin/Main.exe && rm -f ./obj/*.o
  
./obj/Game.o: ./src/Game.cpp ./src/Components/TransformComponent.h ./src/Components/SpriteComponent.h ./src/Components/KeyboardControlComponent.h ./src/Components/TextLabelComponent.h ./src/Components/ProjectileEmitterComponent.h
	g++ -c $(CFLAGS) ./src/Game.cpp -o ./obj/Game.o $(IDIR) $(LDIR) $(LIBS)

./obj/Map.o: ./src/Map.cpp ./src/Components/TileComponent.h
	g++ -c $(CFLAGS) ./src/Map.cpp -o ./obj/Map.o $(IDIR) $(LDIR) $(LIBS)

./obj/%.o: ./src/%.cpp
	g++ -c $(CFLAGS) $< -o $@ $(IDIR) $(LDIR) $(LIBS)

run:
	cd bin && ./Main.exe