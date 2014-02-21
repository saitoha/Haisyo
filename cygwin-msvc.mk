
VCVERSION := 9.0
SDKVERSION := 6.0A

CC=cl.exe /nologo
RC=rc.exe

TARGET=haisyo
VSDIR=C:\Program Files\Microsoft Visual Studio $(VCVERSION)\VC
MSSDK=C:\Program Files\Microsoft SDKs\Windows\v$(SDKVERSION)
CFLAGS=/EHsc /I"$(VSDIR)\include" /I"$(MSSDK)\Include"
LDFLAGS=/LIBPATH:"$(VSDIR)\Lib" /LIBPATH:"$(MSSDK)\Lib" /MACHINE:x86
LIBS=User32.lib Gdi32.lib Shell32.lib Ole32.lib
FILTER=iconv -f SHIFT_JIS-2004 -t UTF-8 | tee build.log
CP=cp -f
RM=rm -f

VCBIN :=/cygdrive/c/Program Files/Microsoft Visual Studio $(VCVERSION)/VC/bin
SDKBIN :=/cygdrive/c/Program Files/Microsoft SDKs/Windows/v$(SDKVERSION)/bin
IDEPATH :=/cygdrive/c/Program Files/Microsoft Visual Studio $(VCVERSION)/Common7/IDE
PATH := $(VCBIN):$(SDKBIN):$(IDEPATH):$(PATH)

all: $(TARGET).exe

haisyo.dll: haisyo.c
	$(CC) $(CFLAGS) \
		/LD haisyo.c \
		/link $(LDFLAGS) $(LIBS) \
		/DEF:haisyo.def \
		/OUT:$@
	

$(TARGET).res: $(TARGET).rc
	$(RC) $<

$(TARGET).exe: main.c haisyo.dll $(TARGET).res
	$(CC) $(CFLAGS) main.c \
		/link $(LDFLAGS) $(LIBS) $(TARGET).res haisyo.lib \
		/OUT:$@

dist: $(TARGET).exe
	$(RM) -r bin
	mkdir bin
	$(CP) $(TARGET).exe haisyo.dll bin/
	$(RM) -r src
	mkdir src
	$(CP) *.c *.h *.rc *.def *.ico *.bmp Makefile src/
	$(RM) Haisyo.zip
	zip Haisyo.zip COPYING.txt README.md src/* bin/*

clean:
	$(RM) *.obj *.dll *.res *.RES *.exp *.lib *.log *.zip *.o
	$(RM) -r src bin

