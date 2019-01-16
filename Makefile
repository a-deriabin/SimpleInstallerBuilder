CFLAGS=-c -std=c11 -pedantic -Wall
OFLAGS=-o

all: FileAppendUtil InstallerBuilder InstallerProgram UninstallerProgram
	./bin/FileAppendUtil "./bin/InstallerBuilder.exe" \
	 "./bin/InstallerProgram.exe" "./bin/UninstallerProgram.exe"
	rm -rf ./bin/FileAppendUtil.exe ./bin/InstallerProgram.exe \
	 ./bin/UninstallerProgram.exe


FileAppendUtil: FileAppendUtil.o
	gcc $(OFLAGS) bin/FileAppendUtil.exe obj/FileAppendUtil/*.o obj/Shared/*.o

FileAppendUtil.o: Shared.o
	mkdir -p obj/FileAppendUtil
	gcc $(CFLAGS) src/FileAppendUtil/main.c -o obj/FileAppendUtil/main.o


InstallerBuilder: InstallerBuilder.o
	gcc $(OFLAGS) bin/InstallerBuilder.exe obj/InstallerBuilder/*.o obj/Shared/*.o

InstallerBuilder.o: Shared.o
	mkdir -p obj/InstallerBuilder
	gcc $(CFLAGS) src/InstallerBuilder/main.c -o obj/InstallerBuilder/main.o


InstallerProgram: InstallerProgram.o
	gcc $(OFLAGS) bin/InstallerProgram.exe obj/InstallerProgram/*.o obj/Shared/*.o

InstallerProgram.o: Shared.o
	mkdir -p obj/InstallerProgram
	gcc $(CFLAGS) src/InstallerProgram/main.c -o obj/InstallerProgram/main.o


UninstallerProgram: UninstallerProgram.o
	gcc $(OFLAGS) bin/UninstallerProgram.exe obj/UninstallerProgram/*.o

UninstallerProgram.o:
	mkdir -p obj/UninstallerProgram
	gcc $(CFLAGS) src/UninstallerProgram/main.c -o obj/UninstallerProgram/main.o


Shared.o: src/Shared/*.c
	mkdir -p obj/Shared
	gcc $(CFLAGS) src/Shared/FileToExeWriter.c -o obj/Shared/FileToExeWriter.o
	gcc $(CFLAGS) src/Shared/FileExtracter.c -o obj/Shared/FileExtracter.o
	gcc $(CFLAGS) src/Shared/ArrayList.c -o obj/Shared/ArrayList.o
	gcc $(CFLAGS) src/Shared/FileUtil.c -o obj/Shared/FileUtil.o
	gcc $(CFLAGS) src/Shared/StringUtil.c -o obj/Shared/StringUtil.o


clean:
	rm -rf ./obj/*
	rm -rf ./bin/*.exe