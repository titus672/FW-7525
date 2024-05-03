
#Lanner Digital IO code

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

TARGET=dio_tst$(EXT)

all: $(TARGET)

$(TARGET): dio_tst.o ioaccess.o
	-$(CC) -o $(TARGET) dio_tst.o ioaccess.o
	-$(CP) $(TARGET) $(BIN_PATH) 

dio_tst.o: dio_tst.c
	-$(CC) -c $^

ioaccess.o: ioaccess.c
	-$(CC) -c $^

clean:
	-$(RM) $(TARGET)
	-$(RM) dio_tst.o
	-$(RM) ioaccess.o
