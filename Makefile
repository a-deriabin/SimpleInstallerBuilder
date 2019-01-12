CFLAGS=-c -std=c11 -pedantic -Wall
OFLAGS=-o

all:
	#TODO



FileAppendUtil: FileAppendUtil.o
	gcc $(OFLAGS) bin/FileAppendUtil.exe obj/FileAppendUtil/*.o obj/Shared/*.o

FileAppendUtil.o: Shared.o
	mkdir -p obj/FileAppendUtil
	gcc $(CFLAGS) src/FileAppendUtil/main.c -o obj/FileAppendUtil/main.o



UninstallerProgram: UninstallerProgram.o
	gcc $(OFLAGS) bin/UninstallerProgram.exe obj/UninstallerProgram/*.o

UninstallerProgram.o:
	mkdir -p obj/UninstallerProgram
	gcc $(CFLAGS) src/UninstallerProgram/main.c -o obj/UninstallerProgram/main.o


Shared.o:
	mkdir -p obj/Shared
	gcc $(CFLAGS) src/Shared/FileToExeWriter.c -o obj/Shared/FileToExeWriter.o



clean:
	rm -rf ./obj/*
	rm -rf ./bin/*