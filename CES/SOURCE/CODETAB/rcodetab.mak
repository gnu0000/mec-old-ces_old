!include "..\makeopts"

TARGET=rcodetab

all: $(EXEDIR)$(TARGET).dll $(LIBDIR)itiprt.lib  $(LIBDIR)itirptut.lib

$(EXEDIR)$(TARGET).dll: *.c  *.h  *.pre  ..\include\itiprt.h  ..\include\itirptut.h
   premake $(TARGET).pre $(TARGET)
   $(MAKE) -nologo -f $(TARGET)


