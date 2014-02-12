#
# Copyright 2005-2014  Hayaki Saito <user@zuse.jp>
#
# ##### BEGIN GPL LICENSE BLOCK #####
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
# ##### END GPL LICENSE BLOCK #####


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
	$(CP) *.c *.h *.rc *.def *.ico *.bmp *.mk Makefile GNUmakefile src/
	$(RM) Haisyo.zip
	$(ZIP) Haisyo.zip COPYING.txt README.md src/* bin/*

clean:
	$(RM) *.obj *.dll *.res *.RES *.exp *.lib *.log *.zip *.o *.exe
	$(RM) -r src bin
