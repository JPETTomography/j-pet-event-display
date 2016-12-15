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

CO = g++
CFLAGS += -Wall
LDFLAGS += -Wall

###################
%.o: %.cpp %.h
	$(CO) $(CFLAGS)  -c $<
%.o: %.cpp
	$(CO) $(CFLAGS) -c $<
###################
all: $(OBJECTS)  
	$(CO)  $(OBJECTS) $(LDFLAGS) -o $(EXEC)
clean:
	rm -rf *.o *.exe
