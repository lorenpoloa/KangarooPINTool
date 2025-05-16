# Ruta a tu instalación de Pin
PIN_HOME ?= /ruta/a/pin    # <--- CAMBIA esto a tu ruta real

CXX = g++
CXXFLAGS = -Wall -Werror -std=c++98 -fPIC -O3
INCLUDES = -I$(PIN_HOME)/source/include/pin -I$(PIN_HOME)/source/include/pin/gen
LDFLAGS = -shared

TOOL_NAME = JumpStatsWithPredictor
OBJDIR = obj-intel64
TOOL_SRC = JumpsStatsWithPredictor.cpp Jump_Predictor_Sim.cpp
OBJS = $(TOOL_SRC:.cpp=.o)
OUT = $(OBJDIR)/$(TOOL_NAME).so

all: $(OUT)

$(OUT): $(OBJS) | $(OBJDIR)
	$(CXX) $(LDFLAGS) -o $@ $(OBJS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(OBJDIR):
	mkdir -p $(OBJDIR)

clean:
	rm -f *.o
	rm -rf $(OBJDIR)

