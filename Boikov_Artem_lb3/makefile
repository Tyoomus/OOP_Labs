# Простейший Makefile для проекта
all:
	g++ -std=c++17 -o game *.cpp

clean:
	rm -f game

run:
	g++ -std=c++17 -o game *.cpp && ./game

build:
	g++ -std=c++17 -Wall -Wextra -o game \
		main.cpp \
		Cell.cpp \
		Enemy.cpp \
		GameField.cpp \
		GameManager.cpp \
		Player.cpp \
		SpellHand.cpp \
		DirectDamageSpell.cpp \
		AreaDamageSpell.cpp \
		SpellBook.cpp

test: clean build run

.PHONY: all clean run build test