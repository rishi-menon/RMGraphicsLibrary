
.PHONY: buildApp cleanApp runApp appPostBuild invalidate

include Config
include Config_Files

rm_logLevel := 5	#Set logging level
include ../CommonRoutines

#calculate the object file dependecies
app_Objects := $(patsubst %.cpp, %.o, $(app_Sources))
app_Objects := $(addprefix $(app_Dir_Obj), $(app_Objects))
#$(info rishi app_objects $(app_Objects))

#calculate the subdirectories to create under bin-int
app_Objects_subDir := $(foreach file, $(app_Sources), $(shell dirname $(file)))
app_Objects_subDir := $(addprefix $(app_Dir_Obj), $(app_Objects_subDir))
#$(info rishi app_objects_dir $(lib_Objects_subDir))

buildApp: | appCreateDir $(app_Dir_Out)$(app_Name_Out) appPostBuild

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

invalidate:
	@[ -f $(app_Dir_Out).rm_update ] && rm $(app_Dir_Out).rm_update || true;


appPostBuild: $(app_Dir_Out).rm_update
$(app_Dir_Out).rm_update:
	@echo "Copying Assets..."
	@cp -r Assets $(app_Dir_Out)
	@touch $(app_Dir_Out).rm_update

appCreateDir:
#create the library folder
	$(call logcon, 4, [ ! -d $(app_Dir_Libs) ], "Creating app directories...")
	$(call rm_CreateFolder, $(app_Dir_Libs))
#create bin and bin-int
	$(call rm_CreateFolder, $(app_Dir_Out))
	$(call rm_CreateFolder, $(app_Dir_Obj))	
#create other sub bin-int directories here

#Add object dependencies here
$(app_Dir_Out)$(app_Name_Out): $(app_Dir_Libs)$(app_Name_LibGraphics) $(app_Objects)

	$(call log, 4, "Building application: $(CPP_COMPILER) $(CPP_FLAGS) $(LINKER_OPTIONS) -o $@ $(app_Objects)")
	$(call log, 5, "Object files in application: $(app_Objects)")
	@$(CPP_COMPILER) $(CPP_FLAGS) $(LINKER_OPTIONS) -o $@ $(app_Objects) || (echo "Failed to link Application" && false);

#Add object compiling here
#p_Dir_Obj)main.o : $(app_Dir_Source)main.cpp
#$(call log, 5, "Compiling $<")
#@$(CPP_COMPILER) $(CPP_FLAGS) $(COMPILER_INCLUDE_PATH) -o $@ -c $< || (echo "Failed to compile $<" && #false);
#
#p_Dir_Obj)temp.o : $(app_Dir_Source)temp.cpp
#$(call log, 5, "Compiling $<")
#@$(CPP_COMPILER) $(CPP_FLAGS) $(COMPILER_INCLUDE_PATH) -o $@ -c $< || (echo "Failed to compile $<" && #false);

#app_CompileFileCpp (cppRelPathName, headerfileDependecies)
define app_CompileFileCpp
app_res_objName = $(patsubst %.cpp, %.o, $(strip $(1)))
$(app_Dir_Obj)$$(app_res_objName) : $(app_Dir_Source)$(strip $(1)) $(2)
	$$(call log, 5, "Compiling $$<")
	@$$(CPP_COMPILER) $$(CPP_FLAGS) $$(COMPILER_OPTIONS) $$(COMPILER_INCLUDE_PATH) -o $$@ -c $$< \
	|| (echo "Failed to compile $$<" && false);
app_res_objName :=
endef

#app_Get_Header_Dependecies is to be used by the foreach function
app_Get_Header_Dependencies = $(addprefix $(app_Dir_Source), $($(file)_hdep))
$(foreach file, $(app_Sources), \
$(eval $(call app_CompileFileCpp, $(file), $(lib_Get_Header_Dependencies))))