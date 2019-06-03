#!/usr/bin/make
.SUFFIXES:
	$(RM) $(RMFILES) .srv_pid
pack: clean
	zip -r $(PCK) cli srv Makefile -x "*/.*"

clean:
	@$(MAKE) -C cli clean
	@$(MAKE) -C srv clean
	$(RM) $(RMFILES) $(PCK)

cli/client: $(wildcard cli/*.c) $(wildcard cli/*.h)
	@$(MAKE) -C $(@D)
srv/server: $(wildcard srv/*.c) $(wildcard srv/*.h)
	@$(MAKE) -C $(@D)
	./server&
	./client

clean:
	$(RM) $(RMFILES) $(TAR) *.o

