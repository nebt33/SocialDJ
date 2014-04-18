ifeq ($(shell uname -s), Linux)
suffix = -qt4
else

endif

build: Makefile.generated
	moc$(suffix) server.cpp -o server.moc
	make -f Makefile.generated

Makefile.generated: Server.pro
	qmake$(suffix) -o Makefile.generated QMAKE_CXXFLAGS=-std=gnu++0x

Server.pro: *.cpp *.h
	qmake$(suffix) -project *.cpp *.h
	echo "QT += network" | tee -a Server.pro

clean:
	$(RM) Server.pro Makefile.generated *.o *.moc
