INSTDIR=/usr/devopt/nscldaq/main


#
#  Add your own compilation/link flags here:

USERCXXFLAGS=-std=c++11 -g -I. -I$(INSTDIR)/include
USERCCFLAGS=$(USERCCFLAGS)
USERLDFLAGS=-L$(INSTDIR)/lib -lDataFlow -ldataformat -lException -Wl,-rpath=$(INSTDIR)/lib

#
#  Add the names of objects you need here if you modified the name of the driver file, 
#  this should also reflect thtat.
#
OBJECTS = main.o CRawVMUSBtoRing.o 

#
#  Modify the line below to provide the name of the library you are trying to build
#  it must be of the form libsomethingorother.so
#

USERFILTER = crate2ring

%.o : %.cpp
	$(CXX) $(USERCXXFLAGS) -c -o $@ $<


$(USERFILTER): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $@ $(USERLDFLAGS) $(LDFLAGS)






