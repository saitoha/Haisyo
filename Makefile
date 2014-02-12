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

 
CC=cl.exe /nologo /EHsc
RC=rc.exe /r
RM=del /f
LIBS=User32.lib Gdi32.lib Ole32.lib Shell32.lib

TARGET=haisyo

all: $(TARGET).exe

hook.dll: hook.lib

hook.lib: hook.c hook.def
	$(CC) /LD hook.c /link $(LIBS) /DEF:hook.def /OUT:hook.dll

haisyo.RES: haisyo.rc
	$(RC) $?

haisyo.exe: hook.dll haisyo.RES
	$(CC) main.c /link $(LIBS) /MACHINE:x86 haisyo.RES hook.lib /OUT:$@

clean:
	$(RM) *.obj *.dll *.res *.RES *.exp *.lib *.log *.zip *.o
	$(RM) src bin

