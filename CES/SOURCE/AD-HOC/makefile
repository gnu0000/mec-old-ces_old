!include "..\makeopts"

# Set target to your module name
TARGET=ad-hoc

all: $(EXEDIR)$(TARGET).dll $(LIBDIR)$(TARGET).lib

$(EXEDIR)$(TARGET).dll: *.c *.h ..\include\*.h *.pre *.met
   premake /AddDot $(TARGET).pre $(TARGET)
   $(MAKE) -nologo -f $(TARGET)

$(LIBDIR)$(TARGET).lib: $(TARGET).def
   premake  /AddDot $(TARGET).pre $(TARGET)
   $(MAKE) -nologo -f $(TARGET) $*.lib
