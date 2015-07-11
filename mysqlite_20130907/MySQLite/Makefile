CC = gcc 
TARGET = MySqlite
OBJ = dbtime.o main.o dbmem.o
DIR =./SRC
OUT =./OUT

TARGET:$(OBJ)
	$(CC) -o $(TARGET) $^ -g
	cp $(TARGET) $(OUT) 
main.o:main.c
	$(CC) -c main.c -g
dbtime.o:dbtime.c dbtime.h
	$(CC) -c dbtime.c dbtime.h -g
dbmem.o:dbmem.c dbmem.h
	$(CC) -c dbmem.c dbmem.h -g

.PHONY:clean
clean:
	@rm -f $(TARGET) *.bin *.~ *.out *.o *.*.swp *.*.swo *.*.gch;clear 
