.PHONY: build clean run buildLib cleanLib cleanApp
.DEFAULT_GOAL := build

LibraryMakefilePath := LibGraphics/
AppMakefilePath := Application/

include $(LibraryMakefilePath)Config
include $(AppMakefilePath)Config

rm_logLevel := 4
include CommonRoutines

build: buildLib
#build directories and copy the created library file
	$(call log, 3, "Building Application...")
	@make -C "$(AppMakefilePath)" appCreateDir
	@cp -r $(LibraryMakefilePath)$(lib_Dir_Out) $(AppMakefilePath)$(app_Dir_Libs)
	@mv $(AppMakefilePath)$(app_Dir_Libs)$(lib_Name_Out) $(AppMakefilePath)$(app_Dir_Libs)$(app_Name_LibGraphics)

	@make -C "$(AppMakefilePath)" buildApp
	$(call log, 3, "Success...")

buildLib:
	$(call log, 3, "Building Library...")
	@make -C "$(LibraryMakefilePath)" buildLib

clean: cleanLib	cleanApp
cleanLib: 
	@make -C "$(LibraryMakefilePath)" cleanLib
cleanApp:
	@make -C "$(AppMakefilePath)" cleanApp

run: build
	@make -C "$(AppMakefilePath)" runApp

#temp rule
.PHONY: invalidate
invalidate:
	@make -C "$(LibraryMakefilePath)" invalidate
	@make -C "$(AppMakefilePath)" invalidate
