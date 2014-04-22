build: Makefile.generated
	moc server.cpp -o server.moc
	mingw32-make -f Makefile.generated LDFLAGS="-static -LC:\Qt\Qt5.2.1\5.2.1\mingw48_32\lib\"

Makefile.generated: Server.pro
	qmake -o Makefile.generated QMAKE_CXXFLAGS="-std=gnu++0x" QMAKE_LDFLAGS=-static

Server.pro: *.cpp *.h
	qmake -project *.cpp *.h
	echo QT += network widgets >> Server.pro

clean:
	del Server.pro Makefile.generated *.o *.moc
	del /q release debug
