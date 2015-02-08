all:
	$(MAKE) -f dist/main.mk $(MAKECMDGOALS)

%:
	$(MAKE) -f dist/main.mk $(MAKECMDGOALS)

checkout:
	git submodule init
	git submodule update
	$(MAKE) -f dist/main.mk $(MAKECMDGOALS)

