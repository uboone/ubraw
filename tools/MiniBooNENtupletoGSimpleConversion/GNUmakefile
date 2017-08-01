TARGET = ConvertNtuple

OBJS_CXX = ConvertNtuple.o BooNEtoNuMI.o BooNEtoGSimple.o

ROOTLIBS = $(shell $(ROOTSYS)/bin/root-config --evelibs) -lXMLParser -lEGPythia6
GENIELIBS = $(shell $(GENIE)/bin/genie-config --libs)
LOG4CPPLIBS = -L$(LOG4CPP_LIB) -llog4cpp

CPPFLAGS   +=  -I$(ROOTSYS)/include
CPPFLAGS   +=  -I$(GENIE_INC)/GENIE -std=c++11

$(TARGET) : $(OBJS_CXX)
	g++ $(OBJS_CXX) $(ROOTLIBS) $(GENIELIBS) $(LOG4CPPLIBS) $(EXTRALIBS) $(CPPFLAGS) -o $(TARGET)

BooNEtoNuMI.o : BooNEtoNuMI.cxx BooNEtoNuMI.h BooNENtuple.h NuMINtuple.h
	g++ -c BooNEtoNuMI.cxx $(CPPFLAGS)

BooNEtoGSimple.o : BooNEtoGSimple.cxx BooNEtoGSimple.h BooNENtuple.h BeamNtuple.h
	g++ -c BooNEtoGSimple.cxx $(CPPFLAGS) 

ConvertNtuple.o : ConvertNtuple.cxx
	g++ -c ConvertNtuple.cxx $(CPPFLAGS)

.PHONY : clean

clean :
	-rm -f $(OBJS_CXX) $(TARGET)
