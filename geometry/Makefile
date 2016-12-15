########################
## geometry libraries necessary for visualisation classes
LDFLAGS_VISUAL = -lGeom -lGeomPainter  
#-lGuiHtml
########################
########################
EXEC = Geom.exe
OBJECTS = createGeometryPET.o
CFLAGS =  `root-config --cflags`  
LDFLAGS = `root-config --cflags --glibs`
LDFLAGS += $(LDFLAGS_VISUAL) 

ifeq ($(OSTYPE),aix) 
	CO = xlc++
	LDFLAGS += -bh:5
else
	CO = g++
	CFLAGS += -Wall
	LDFLAGS += -Wall
endif


###################
%.o: %.cc %.h
	$(CO) $(CFLAGS)  -c $<
%.o: %.cc
	$(CO) $(CFLAGS) -c $<
###################
all: $(OBJECTS)  
	$(CO)  $(OBJECTS) $(LDFLAGS) -o $(EXEC)
clean:
	rm -rf *.o *.exe

