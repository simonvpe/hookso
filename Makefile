DEBUG=-DDEBUG
OPTIMIZE=-O3
HOOK_ARGS=${DEBUG} ${OPTIMIZE} -std=c++11 -shared -fPIC -Wl,--no-as-needed -ldl
MAIN_ARGS=-lc
all:
	g++ ${HOOK_ARGS} -o hook.so hook.cpp
	gcc ${MAIN_ARGS} main.c

assembly:
	g++ ${HOOK_ARGS} -S -o hook.asm hook.cpp
	gcc ${MAIN_ARGS} -S -o main.asm main.c

run:
	LD_PRELOAD=${PWD}/hook.so ./a.out
