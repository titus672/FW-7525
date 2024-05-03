
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

TARGET=btn_tst$(EXT)

all: $(TARGET)

$(TARGET): btn_tst.o ioaccess.o
	-$(CC) -o $(TARGET) btn_tst.o ioaccess.o
	-$(CP) $(TARGET) $(BIN_PATH) 

btn_tst.o: btn_tst.c
	-$(CC) -c $^

ioaccess.o: ioaccess.c
	-$(CC) -c $^

clean:
	-$(RM) $(TARGET)
	-$(RM) btn_tst.o
	-$(RM) ioaccess.o
