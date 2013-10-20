#Configuracao: testes com multiplicacao de matrizes
#Disciplina: Ambientes de Programacao Paralela (PPGI-2013/3)
#-------------------------------------------------------------

VERSION = 1.0

SRC_FFT = fft.c

EXE_FFT = fft


#-------------------------------------------------------------
# the rest of the file should not change
CC = gcc -O1 -Wall
CFLAGS = -lm
RM = rm

# make all
all:  
	$(CC)  $(CFLAGS)  -o  $(EXE_FFT)  $(SRC_FFT)

# version
version: 
	@echo $(VERSION) "(Draft Version!)" 
# clean
clean:
	$(RM) -rf *.o  $(EXE_FFT)
