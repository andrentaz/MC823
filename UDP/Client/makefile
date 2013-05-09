all: build

build:
	gcc Client.c CliFunctions.c -o Client
rmv:
	rm Client

redo:	rmv build

debug:
	gcc Client.c CliFunctions.c -o Client -g
run:	build
	./Client
r:	build run
