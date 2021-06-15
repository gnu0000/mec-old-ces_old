!include "..\makeopts"

# Set target to your module name
TARGET=stditem
RPT_1=rstdit

all: 
   premake /AddDot $(TARGET).pre $(TARGET)
   $(MAKE) -nologo -f $(TARGET)
   premake /AddDot $(RPT_1).pre $(RPT_1)
   $(MAKE) -nologo -f $(RPT_1)
