# Objetos necesarios
TOOL_ROOTS := JumpsStatsSimple JumpsStatsByTypes JumpsStatsWithPredictor
TOOL_SOURCES := JumpsStatsWithPredictor.cpp Jump_Predictor_Sim.cpp
TOOL_OBJECTS := $(TOOL_SOURCES:.cpp=$(OBJ_SUFFIX))

# Regla de enlace
$(OBJDIR)JumpsStatsWithPredictor$(PINTOOL_SUFFIX): \
    $(OBJDIR)Jump_Predictor_Sim$(OBJ_SUFFIX) \
    $(OBJDIR)JumpsStatsWithPredictor$(OBJ_SUFFIX)

	$(LINKER) $(TOOL_LDFLAGS_NOOPT) $(LINK_EXE)$@ \
	$(OBJDIR)Jump_Predictor_Sim$(OBJ_SUFFIX) \
	$(OBJDIR)JumpsStatsWithPredictor$(OBJ_SUFFIX) \
	$(TOOL_LPATHS) $(TOOL_LIBS)
