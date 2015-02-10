all:
	$(MAKE) -f dist/main.mk $(MAKECMDGOALS)

%:
	$(MAKE) -f dist/main.mk $(MAKECMDGOALS)

checkout:
	$(MAKE) -f dist/main.mk $(MAKECMDGOALS)

