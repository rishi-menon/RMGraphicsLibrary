
.PHONY: buildApp cleanApp runApp
include Config

C_COMPILER ?= gcc
CPP_COMPILER :=g++
CPP_FLAGS := -Wall -std=c++11

# -I ""
# vendor: lib/Include/RMGraphics/vendor/
COMPILER_INCLUDE_PATH := \
-I ./$(app_Dir_Libs)Include/ \
-I ./$(app_Dir_Libs)Include/RMGraphics/ \
-I ./$(app_Dir_Libs)Include/RMGraphics/vendor/spdlog/include/ \
-I ./$(app_Dir_Libs)Include/RMGraphics/vendor/glm/


LINKER_OPTIONS := \
-L ./$(app_Dir_Libs) \
-l $(app_NameStrip_LibGraphics)


rm_logLevel := 5	#Set logging level
include ../CommonRoutines

buildApp: | appCreateDir $(app_Dir_Out)$(app_Name_Out)

cleanApp:
	$(call log, 4, "Cleaning $(app_Dir_Libs)")
	@[ -d $(app_Dir_Libs) ] && rm -r $(app_Dir_Libs) || true;
	$(call log, 4, "Cleaning $(app_Dir_Obj)")
	@[ -d $(app_Dir_Obj) ] && rm -r $(app_Dir_Obj) || true;
	$(call log, 4, "Cleaning $(app_Dir_Out)")
	@[ -d $(app_Dir_Out) ] && rm -r $(app_Dir_Out) || true;

runApp:
	$(call log, 3, "")
	@./$(app_Dir_Out)$(app_Name_Out)


appCreateDir:
#create the library folder
	$(call logcon, 4, [ ! -d $(app_Dir_Libs) ], "Creating app directories...")
	$(call rm_CreateFolder, $(app_Dir_Libs))
#create bin and bin-int
	$(call rm_CreateFolder, $(app_Dir_Out))
	$(call rm_CreateFolder, $(app_Dir_Obj))	
#create other sub bin-int directories here

#Add object dependencies here
$(app_Dir_Out)$(app_Name_Out): \
$(app_Dir_Obj)main.o \
$(app_Dir_Obj)temp.o

	$(call log, 4, "Building application: $(CPP_COMPILER) $(CPP_FLAGS) $(LINKER_OPTIONS) -o $@ $^")
	$(call log, 5, "Object files in application: $^")
	@$(CPP_COMPILER) $(CPP_FLAGS) $(LINKER_OPTIONS) -o $@ $^ || (echo "Failed to link Application" && false);

#Add object compiling here
$(app_Dir_Obj)main.o : $(app_Dir_Source)main.cpp
	$(call log, 5, "Compiling $<")
	@$(CPP_COMPILER) $(CPP_FLAGS) $(COMPILER_INCLUDE_PATH) -o $@ -c $< || (echo "Failed to compile $<" && false);

$(app_Dir_Obj)temp.o : $(app_Dir_Source)temp.cpp
	$(call log, 5, "Compiling $<")
	@$(CPP_COMPILER) $(CPP_FLAGS) $(COMPILER_INCLUDE_PATH) -o $@ -c $< || (echo "Failed to compile $<" && false);