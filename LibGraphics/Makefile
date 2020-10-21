.PHONY: buildLib cleanLib makeDirLib postBuildCommands
.DEFAULT_GOAL := buildLib

include Config
include Config_Files

rm_logLevel := 5	#Set logging level
include ../CommonRoutines

C_COMPILER ?= gcc
CPP_COMPILER ?= g++
CPP_FLAGS ?= -g -Wall -std=c++17

COMPILER_INCLUDE_PATH ?= \
-I ./vendor/spdlog/include/ \
-I ./vendor/glm/ \
-I ./vendor/GLFW/include/ \
-I ./vendor/stb_image/ \
-I ./vendor/FreeType/ \
-I ./src/ \

#calculate the object file dependecies
lib_Objects := $(patsubst %.cpp, %.o, $(lib_Sources))
lib_Objects := $(addprefix $(lib_Dir_Obj), $(lib_Objects))
#$(info rishi lib_objects $(lib_Objects))

#calculate the subdirectories to create under bin-int
lib_Objects_subDir := $(foreach file, $(lib_Sources), $(shell dirname $(file)))
lib_Objects_subDir := $(addprefix $(lib_Dir_Obj), $(lib_Objects_subDir))

#calculate the subdirectories to create under bin/Include
lib_Out_Include_CopyFiles_subDir := $(foreach file, $(lib_Out_Include_CopyFiles), $(shell dirname $(file)))
lib_Out_Include_CopyFiles_subDir := $(addprefix $(lib_Dir_Out)$(lib_Dir_Out_Include), $(lib_Out_Include_CopyFiles_subDir))
#$(info rishi lib_Out_Include_CopyFiles_subDir $(lib_Out_Include_CopyFiles_subDir))

buildLib: makeDirLib $(lib_Dir_Out)$(lib_Name_Out) postBuildCommands

cleanLib:
	$(call log, 4, "Cleaning $(lib_Dir_Obj)")
	@[ -d $(lib_Dir_Obj) ] && rm -r $(lib_Dir_Obj) || true;
	$(call log, 4, "Cleaning $(lib_Dir_Out)")
	@[ -d $(lib_Dir_Out) ] && rm -r $(lib_Dir_Out) || true;


postBuildCommands: $(lib_Dir_Out)$(lib_Dir_Out_Include).rm_update


#this is a temp rule
.PHONY: invalidate
invalidate:
	@$(call rmDeleteFile, $(lib_Dir_Out)$(lib_Dir_Out_Include).rm_update)
	@$(call rmDeleteFile, $(lib_Dir_Out).rm_update)

#This is a hack so that copying does not repeatedly happen...
$(lib_Dir_Out)$(lib_Dir_Out_Include).rm_update: \
$(addprefix $(lib_Dir_Source), $(lib_Out_Include_CopyFiles))

	$(call log, 3, "Starting PostBuild commands:")
	$(call rmCopy, 5, vendor/, $(lib_Dir_Out)$(lib_Dir_Out_Include)vendor/)
#copy the postbuild include files
	
	@rm_src=$(lib_Dir_Source); \
	rm_include=$(lib_Dir_Out)$(lib_Dir_Out_Include); \
	for file in $(lib_Out_Include_CopyFiles); \
	do \
	[ ! -f $${rm_src}$${file} ] || cp $${rm_src}$${file} $${rm_include}$$(dirname $${file}) ; \
	echo "Copying: $${rm_src}$${file} to $${rm_include}$$(dirname $${file})"; \
	done

	@touch $(lib_Dir_Out)$(lib_Dir_Out_Include).rm_update 

#Add directories to create here
makeDirLib: $(lib_Dir_Out).rm_update

$(lib_Dir_Out).rm_update:
#$(call logcon, 4, [ ! -d $(lib_Dir_Out) ], "Creating lib directories...")
	$(call log, 4, "Creating lib directories...")
	$(call rm_CreateFolder, $(lib_Dir_Out))
	$(call rm_CreateFolder, $(lib_Dir_Obj))
#create folder for the include inside bin
	$(call rm_CreateFolder, $(lib_Dir_Out)$(lib_Dir_Out_Include))

#Creates nested sub folders inside bin-int and inside bin/Include/RPGraphics
	@for dir in $(lib_Objects_subDir) $(lib_Out_Include_CopyFiles_subDir); \
	do \
	[ -d $$dir ] || mkdir -p $$dir; \
	done
	
#extract the glfw and the other static library
	$(call rm_CreateFolder, $(lib_Dir_Obj)GLFW)
	@rm_curDir=$$(pwd); \
	cd $(lib_Dir_Obj)GLFW; \
	ar x $${rm_curDir}/vendor/GLFW/lib/libglfw3.a;

	$(call rm_CreateFolder, $(lib_Dir_Obj)FreeType)
	@rm_curDir=$$(pwd); \
	cd $(lib_Dir_Obj)FreeType; \
	ar x $${rm_curDir}/vendor/FreeType/lib/libfreetype.a;

	$(call rm_CreateFolder, $(lib_Dir_Obj)GLEW)
	@rm_curDir=$$(pwd); \
	cd $(lib_Dir_Obj)GLEW; \
	ar x $${rm_curDir}/vendor/GLEW/lib/libGLEW.a;
	
	@touch $(lib_Dir_Out).rm_update;

#Craete the static library...
$(lib_Dir_Out)$(lib_Name_Out): $(lib_Objects)
	$(call rmDeleteFile, $(lib_Dir_Out)$(lib_Name_Out))
	$(call log, 4, "Building library of: $^")
	
	@ar rs $(lib_Dir_Out)$(lib_Name_Out) $^ \
	$(lib_Dir_Obj)GLFW/*.o $(lib_Dir_Obj)FreeType/*.o \
	$(lib_Dir_Obj)GLEW/*.o \
	|| (echo "Failed to create static library $<" && false);


#lib_CompileFileCpp (objRelativePathName, cppRelPathName, headerfileDependecies)
define lib_CompileFileCpp
lib_res_objName = $(patsubst %.cpp, %.o, $(strip $(1)))
$(lib_Dir_Obj)$$(lib_res_objName) : $(lib_Dir_Source)$(strip $(1)) $(2)

	$$(call log, 5, "Compiling $$<")
	@$$(CPP_COMPILER) $$(CPP_FLAGS) $$(COMPILER_INCLUDE_PATH) -o $$@ -c $$< \
	|| (echo "Failed to compile $$<" && false);
lib_res_objName :=
endef

#lib_Get_Header_Dependecies is to be used by the foreach function
lib_Get_Header_Dependencies = $(addprefix $(lib_Dir_Source), $($(file)_hdep))
$(foreach file, $(lib_Sources), \
$(eval $(call lib_CompileFileCpp, $(file), $(lib_Get_Header_Dependencies))))