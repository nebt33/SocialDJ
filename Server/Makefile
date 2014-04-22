build: Makefile.generated
	moc server.cpp -o server.moc
	mingw32-make -f Makefile.generated

Makefile.generated: Server.pro
	qmake -o Makefile.generated QMAKE_CXXFLAGS="-std=gnu++0x" QMAKE_LIBS="-Wl,-subsystem,console"

Server.pro: *.cpp *.h
	qmake -project *.cpp *.h
	echo QT += network widgets >> Server.pro
	echo LFLAGS += -Wl,-subsystem,console >> Server.pro

clean:
	del Server.pro Makefile.generated *.o *.moc
	del /q release debug
