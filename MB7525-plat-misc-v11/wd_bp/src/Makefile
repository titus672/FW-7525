
#Lanner Software Button Makefile

EXT=
RM=rm -rf
CP=cp -f
BIN_PATH=../bin/

ifneq ($(DJGPP),)

EXT=.exe
RM=del
CP=copy
BIN_PATH=..\bin\

endif 

TARGET=wd_tst$(EXT)

all: $(TARGET)

$(TARGET): wd_tst.o ioaccess.o
	-$(CC) -o $(TARGET) wd_tst.o ioaccess.o
	-$(CP) $(TARGET) $(BIN_PATH) 

wd_tst.o: wd_tst.c
	-$(CC) -c $^

ioaccess.o: ioaccess.c
	-$(CC) -c $^

clean:
	-$(RM) $(TARGET)
	-$(RM) wd_tst.o
	-$(RM) ioaccess.o
