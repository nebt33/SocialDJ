ifeq ($(shell uname -s), Linux)
qt = -qt5
RMF = rm -rf
QMAKE_LIBS="-lid3 -lz"
else
unix = mingw32-
RM = del
RMF = del /q
QMAKE_CXXFLAGS="-I../id3lib-3.8.3/include"
QMAKE_LIBS="-Wl,-subsystem,console ./libid3.a ./libz.a"
kill_server=taskkill /f /im Server.exe
endif

build: Makefile.generated
	$(kill_server)
	moc$(qt) server.cpp -o server.moc
	$(unix)make -f Makefile.generated

Makefile.generated: Server.pro
	qmake$(qt) -o Makefile.generated QMAKE_CXXFLAGS="$(QMAKE_CXXFLAGS) -std=gnu++0x -Wno-unused-parameter" QMAKE_LIBS=$(QMAKE_LIBS)

Server.pro: *.cpp *.h
	$(RM) qrc_resources.cpp qrc_resources.o
	qmake$(qt) -project *.cpp *.h
	echo QT += network widgets multimedia >> Server.pro
	echo CONFIG += mobility >> Server.pro
	echo MOBILITY += multimedia >> Server.pro

clean:
	$(RM) Server.pro Makefile.generated *.o *.moc
	$(RM) qrc_resources.cpp qrc_resources.o
	$(RMF) release debug Server
