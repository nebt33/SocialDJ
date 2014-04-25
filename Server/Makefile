ifeq ($(shell uname -s), Linux)
qt = -qt4
RMF = rm -rf
else
unix = mingw32-
RM = del
RMF = del /q
QMAKE_LIBS="-Wl,-subsystem,console"
endif


build: Makefile.generated
	moc$(qt) server.cpp -o server.moc
	$(unix)make -f Makefile.generated

Makefile.generated: Server.pro
	qmake$(qt) -o Makefile.generated QMAKE_CXXFLAGS="-std=gnu++0x -Wno-unused-parameter" QMAKE_LIBS=$(QMAKE_LIBS)

Server.pro: *.cpp *.h
	qmake$(qt) -project *.cpp *.h
	echo QT += network widgets >> Server.pro
	echo LFLAGS += -Wl,-subsystem,console >> Server.pro

clean:
	$(RM) Server.pro Makefile.generated *.o *.moc
	$(RMF) release debug Server
