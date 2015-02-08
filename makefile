all:
	$(MAKE) -f dist/main.mk $(MAKECMDGOALS)

%:
	$(MAKE) -f dist/main.mk $(MAKECMDGOALS)

checkout:
	git submodule update --recursive --init
	git submodule update --recursive
	$(MAKE) -f dist/main.mk $(MAKECMDGOALS)

