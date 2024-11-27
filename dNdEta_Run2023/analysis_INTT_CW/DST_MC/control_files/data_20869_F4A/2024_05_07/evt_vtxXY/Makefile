CC = g++
CFLAGS = $(shell root-config --cflags)
LIBS = $(shell root-config --libs)

SRCDIR = ../../../../evtXY_data_check
OBJDIR = obj
BINDIR = .

SRC = make_evtXY_plot.cpp \
      $(SRCDIR)/CheckEvtXY.cpp \
      $(SRCDIR)/sPhenixStyle.C \
      $(SRCDIR)/ReadEvtXY_Chain.C \
      $(SRCDIR)/gaus_func.cpp

OBJ = $(SRC:%.cpp=$(OBJDIR)/%.o)

TARGET = $(BINDIR)/run_make_plot

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJ) \
    $(CC) $(CFLAGS) -o $@ $^ $(LIBS)

$(OBJDIR)/%.o: %.cpp \
    @mkdir -p $(@D) \
    $(CC) $(CFLAGS) -c -o $@ $<

clean: \
    @rm -rf $(OBJDIR) $(TARGET)
