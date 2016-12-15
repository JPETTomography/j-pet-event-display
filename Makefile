########################
## geometry libraries necessary for visualisation classes
LDFLAGS_VISUAL = -lGeom -lGeomPainter -lTreePlayer
#-lGuiHt
########################
HTML_DIR = html
LATEX_DIR = latex
########################
TEST_DIR = ../tests
########################
EXEC = EventDisplay.exe
MODULES = EDGeometryVisualizator.cc EDLogger.cc EDCommonTools.cc Run.cc 
OBJS = $(MODULES:.cc=.o)
########################
LOG = EventDisplay.log
########################
#INCFLAGS = -I$(shell root-config --incdir)
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
	$(CO) $(CFLAGS) $(INCFLAGS) -c $<
%.o: %.cc
	$(CO) $(CFLAGS) $(INCFLAGS) -c $<
###################
all:  $(OBJS)  
	$(CO) $(OBJS) $(LDFLAGS) -o $(EXEC)
run: all
	./EventDisplay.exe
################
tests: $(OBJS)
	cd $(TEST_DIR); $(MAKE)
tests_run: tests
	cd $(TEST_DIR); ./run_tests.pl 
################
documentation: 
	doxygen Doxyfile
clean:
	rm -rf *.o $(EXEC) $(LOG)  $(DICT) $(DICT_HEAD) $(LATEX_DIR) $(HTML_DIR)

