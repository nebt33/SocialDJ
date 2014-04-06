build: Makefile.generated
	make -f Makefile.generated

Makefile.generated: Server.pro
	qmake-qt4 -o Makefile.generated

Server.pro: *.cpp *.h
	qmake-qt4 -project *.cpp *.h

clean:
	$(RM) Server.pro Makefile.generated *.o
