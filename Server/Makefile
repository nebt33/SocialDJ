build: Makefile.generated
	moc-qt4 server.cpp -o server.moc
	make -f Makefile.generated

Makefile.generated: Server.pro
	qmake-qt4 -o Makefile.generated QMAKE_CXXFLAGS=-std=gnu++0x

Server.pro: *.cpp *.h
	qmake-qt4 -project *.cpp *.h
	echo "QT += network" | tee -a Server.pro

clean:
	$(RM) Server.pro Makefile.generated *.o *.moc
