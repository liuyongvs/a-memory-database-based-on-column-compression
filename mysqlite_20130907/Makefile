CC = gcc
TARGET = Demo.bin
OBJ = Demo.o csv.o CF.o DF.o BF.o Mem.o
$(TARGET):$(OBJ) 
	$(CC) -o $(TARGET) $^  -g
Demo.o:Demo.c
	$(CC) -c Demo.c -g
csv.o:csv.c  csv.h
	$(CC) -c csv.c -g
CF.o:CF.c CF.h 
	$(CC) -c CF.c -g
DF.o:DF.c DF.h
	$(CC) -c DF.c -g
BF.o:BF.c BF.h
	$(CC) -c BF.c -g
Mem.o:Mem.c Mem.h
	$(CC) -c Mem.c Mem.h

.PHONY: clean 
clean:
	rm *.bin *.~ *.out *.o mem *.rar *.*.swp *.*.swo ./out/* 
