#Configuracao: testes com multiplicacao de matrizes
#Disciplina: Ambientes de Programacao Paralela (PPGI-2013/3)
#-------------------------------------------------------------

VERSION = 1.0

SRC_MULTMAT0 = multmat0.c
SRC_MULTMAT1 = multmat1.c

EXE_MULTMAT0 = multmat0
EXE_MULTMAT1 = multmat1

#-------------------------------------------------------------
# the rest of the file should not change
CC = gcc -O1 -Wall
CFLAGS =  
RM = rm

# make all
all:  matriz.o $(EXE_MULTMAT0) $(EXE_MULTMAT1) 

matriz.o: matriz.c matriz.h
	$(CC) -c $(CFLAGS) -o $@ matriz.c

$(EXE_MULTMAT0): matriz.o $(SRC_MULTMAT0)
	$(CC) $(CFLAGS) -o $@ matriz.o $(SRC_MULTMAT0) 

$(EXE_MULTMAT1): matriz.o $(SRC_MULTMAT1)
	$(CC) $(CFLAGS) -o $@ matriz.o $(SRC_MULTMAT1)
# version
version: 
	@echo $(VERSION) "(Draft Version!)" 
# clean
clean:
	$(RM) -rf *.o  $(EXE_MULTMAT0) $(EXE_MULTMAT1) 
