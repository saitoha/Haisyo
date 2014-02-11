
CC=i686-w64-mingw32-gcc
RC=i686-w64-mingw32-windres
ZIP=zip
CP=cp -f
RM=rm -f

TARGET=haisyo
LIBS=-luser32 -lgdi32 -lole32 -lshell32 -luuid

all: $(TARGET).exe hook.dll

run: all

c.o:
	$(CC) -c $< -o $@

hook.dll: hook.o hook.def
	$(CC) -shared $< -d hook.def -o $@ $(LIBS)

$(TARGET).res.o: $(TARGET).rc
	$(RC) $< $@

$(TARGET).exe: main.o hook.dll $(TARGET).res.o
	$(CC) -o $@ $^ $(LIBS) 

dist: $(TARGET).exe
	$(RM) -r bin
	mkdir bin
	$(CP) $(TARGET).exe hook.dll bin/
	$(RM) -r src
	mkdir src
	$(CP) *.c *.h *.rc *.def *.ico *.bmp *.mk src/
	$(RM) Haisyo.zip
	$(ZIP) Haisyo.zip COPYING.txt README.md src/* bin/*

clean:
	$(RM) *.obj *.dll *.res *.RES *.exp *.lib *.log *.zip *.o *.exe
	$(RM) -r src bin
