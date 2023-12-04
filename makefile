CC = g++
FLAGS = -g -Wall 
LIBS = -lncurses -lcurl
SRCDIR = src
INCDIR = include
OBJDIR = obj
SRC = $(wildcard $(SRCDIR)/*.cpp) $(wildcard $(SRCDIR)/GUI/*.cpp) $(wildcard $(SRCDIR)/WorkItems/*.cpp)
OBJ = $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(SRC))
INC = -I$(INCDIR) -I$(INCDIR)/GUI -I$(INCDIR)/WorkItems

pm: $(OBJ)
	$(CC) $(FLAGS) -o pm $(OBJ) $(LIBS)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(@D)
	$(CC) $(FLAGS) $(INC) -c -o $@ $<

.PHONY: clean
clean:
	rm -rf pm $(OBJ)

install:
	cp pm /usr/bin
