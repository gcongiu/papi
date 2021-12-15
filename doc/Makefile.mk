##
## Copyright (C) by Innovative Computing Laboratory
##     See copyright in top-level directory
##

doxygen:
	doxygen doc/Doxyfile-html
	doxygen doc/Doxyfile-man1
	doxygen doc/Doxyfile-man3

install-data-hook:
	mkdir -p $(DESTDIR)$(docdir)
	cp -r doc/doxygen/html $(DESTDIR)$(docdir)
	cp -r doc/doxygen/man/man1 $(DESTDIR)$(docdir)
	cp -r doc/doxygen/man/man3 $(DESTDIR)$(docdir)

uninstall-hook:
	rm -rf $(DESTDIR)$(docdir)
