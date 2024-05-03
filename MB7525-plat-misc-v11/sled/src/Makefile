
#Lanner Status LED code

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

TARGET=sled_tst$(EXT)

all: $(TARGET)

$(TARGET): sled_tst.o ioaccess.o
	-$(CC) -o $(TARGET) sled_tst.o ioaccess.o
	-$(CP) $(TARGET) $(BIN_PATH) 

sled_tst.o: sled_tst.c
	-$(CC) -c $^

ioaccess.o: ioaccess.c
	-$(CC) -c $^

clean:
	-$(RM) $(TARGET)
	-$(RM) sled_tst.o
	-$(RM) ioaccess.o
