CC = cc
CFLAGS = -std=c89 -Wall -Wextra -Werror -pedantic
OBJECTS = main.o game.o map.o display.o linkedList.o terminal.o color.o newSleep.o

.PHONY: all clean

all: dungeon

dungeon: $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $(OBJECTS)

main.o: main.c game.h map.h display.h terminal.h
game.o: game.c game.h map.h linkedList.h display.h newSleep.h
map.o: map.c map.h
display.o: display.c display.h game.h map.h color.h
linkedList.o: linkedList.c linkedList.h
terminal.o: terminal.c terminal.h
color.o: color.c color.h
newSleep.o: newSleep.c newSleep.h

clean:
	rm -f $(OBJECTS) dungeon
