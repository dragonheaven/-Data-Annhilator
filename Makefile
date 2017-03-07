#############################################################################
# Makefile for building: wipeit
# Generated by qmake (2.01a) (Qt 4.8.7) on: Fri Mar 10 16:49:10 2017
# Project:  wipeit.pro
# Template: app
# Command: /usr/bin/qmake-qt4 -o Makefile wipeit.pro
#############################################################################

####### Compiler, tools and options

CC            = gcc
CXX           = g++
DEFINES       = -DNOHOSTACCESS -DTEST -DRETAIL -DQT_SQL_LIB -DQT_GUI_LIB -DQT_CORE_LIB -DQT_SHARED
CFLAGS        = -m64 -pipe -g -Wall -W -D_REENTRANT $(DEFINES)
CXXFLAGS      = -m64 -pipe -Wno-write-strings -std=c++11 -g -Wall -W -D_REENTRANT $(DEFINES)
INCPATH       = -I/usr/share/qt4/mkspecs/linux-g++-64 -I. -I/usr/include/qt4/QtCore -I/usr/include/qt4/QtGui -I/usr/include/qt4/QtSql -I/usr/include/qt4 -I. -I. -I.
LINK          = g++
LFLAGS        = -m64
LIBS          = $(SUBLIBS)  -L/usr/lib/x86_64-linux-gnu -Lliblshw -llshw -lsgutils2 -lresolv -lQtSql -lQtGui -lQtCore -lpthread 
AR            = ar cqs
RANLIB        = 
QMAKE         = /usr/bin/qmake-qt4
TAR           = tar -cf
COMPRESS      = gzip -9f
COPY          = cp -f
SED           = sed
COPY_FILE     = $(COPY)
COPY_DIR      = $(COPY) -r
STRIP         = strip
INSTALL_FILE  = install -m 644 -p
INSTALL_DIR   = $(COPY_DIR)
INSTALL_PROGRAM = install -m 755 -p
DEL_FILE      = rm -f
SYMLINK       = ln -f -s
DEL_DIR       = rmdir
MOVE          = mv -f
CHK_DIR_EXISTS= test -d
MKDIR         = mkdir -p

####### Output directory

OBJECTS_DIR   = ./

####### Files

SOURCES       = wipeit.cpp \
		main.cpp \
		maindialog.cpp \
		wipedialog.cpp \
		errordialog.cpp \
		megaraid.cpp \
		dbupdatedlg.cpp \
		histdialog.cpp \
		confirmdialog.cpp \
		creditsdlg.cpp \
		simplecrypt.cpp \
		license.cpp \
		sgwrite.cpp \
		sg_pt_common.c \
		sg_pt_linux.c moc_maindialog.cpp \
		moc_wipedialog.cpp \
		moc_errordialog.cpp \
		moc_confirmdialog.cpp \
		moc_creditsdlg.cpp \
		moc_dbupdatedlg.cpp \
		moc_histdialog.cpp \
		qrc_wipeit.cpp
OBJECTS       = wipeit.o \
		main.o \
		maindialog.o \
		wipedialog.o \
		errordialog.o \
		megaraid.o \
		dbupdatedlg.o \
		histdialog.o \
		confirmdialog.o \
		creditsdlg.o \
		simplecrypt.o \
		license.o \
		sgwrite.o \
		sg_pt_common.o \
		sg_pt_linux.o \
		moc_maindialog.o \
		moc_wipedialog.o \
		moc_errordialog.o \
		moc_confirmdialog.o \
		moc_creditsdlg.o \
		moc_dbupdatedlg.o \
		moc_histdialog.o \
		qrc_wipeit.o
DIST          = /usr/share/qt4/mkspecs/common/unix.conf \
		/usr/share/qt4/mkspecs/common/linux.conf \
		/usr/share/qt4/mkspecs/common/gcc-base.conf \
		/usr/share/qt4/mkspecs/common/gcc-base-unix.conf \
		/usr/share/qt4/mkspecs/common/g++-base.conf \
		/usr/share/qt4/mkspecs/common/g++-unix.conf \
		/usr/share/qt4/mkspecs/qconfig.pri \
		/usr/share/qt4/mkspecs/features/qt_functions.prf \
		/usr/share/qt4/mkspecs/features/qt_config.prf \
		/usr/share/qt4/mkspecs/features/exclusive_builds.prf \
		/usr/share/qt4/mkspecs/features/default_pre.prf \
		/usr/share/qt4/mkspecs/features/debug.prf \
		/usr/share/qt4/mkspecs/features/default_post.prf \
		/usr/share/qt4/mkspecs/features/shared.prf \
		/usr/share/qt4/mkspecs/features/unix/gdb_dwarf_index.prf \
		/usr/share/qt4/mkspecs/features/warn_on.prf \
		/usr/share/qt4/mkspecs/features/qt.prf \
		/usr/share/qt4/mkspecs/features/unix/thread.prf \
		/usr/share/qt4/mkspecs/features/moc.prf \
		/usr/share/qt4/mkspecs/features/resources.prf \
		/usr/share/qt4/mkspecs/features/uic.prf \
		/usr/share/qt4/mkspecs/features/yacc.prf \
		/usr/share/qt4/mkspecs/features/lex.prf \
		/usr/share/qt4/mkspecs/features/include_source_dir.prf \
		wipeit.pro
QMAKE_TARGET  = wipeit
DESTDIR       = 
TARGET        = wipeit

first: all
####### Implicit rules

.SUFFIXES: .o .c .cpp .cc .cxx .C

.cpp.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.cc.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.cxx.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.C.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.c.o:
	$(CC) -c $(CFLAGS) $(INCPATH) -o "$@" "$<"

####### Build rules

all: Makefile $(TARGET)

$(TARGET): liblshw/liblshw.a ui_maindialog.h ui_scandialog.h ui_wipedialog.h ui_errordialog.h ui_dbupdatedlg.h ui_dbautoupdatedlg.h ui_histdialog.h ui_confirmdialog.h ui_creditsdlg.h $(OBJECTS)  
	$(LINK) $(LFLAGS) -o $(TARGET) $(OBJECTS) $(OBJCOMP) $(LIBS)
	{ test -n "$(DESTDIR)" && DESTDIR="$(DESTDIR)" || DESTDIR=.; } && test $$(gdb --version | sed -e 's,[^0-9][^0-9]*\([0-9]\)\.\([0-9]\).*,\1\2,;q') -gt 72 && gdb --nx --batch --quiet -ex 'set confirm off' -ex "save gdb-index $$DESTDIR" -ex quit '$(TARGET)' && test -f $(TARGET).gdb-index && objcopy --add-section '.gdb_index=$(TARGET).gdb-index' --set-section-flags '.gdb_index=readonly' '$(TARGET)' '$(TARGET)' && rm -f $(TARGET).gdb-index || true

Makefile: wipeit.pro  /usr/share/qt4/mkspecs/linux-g++-64/qmake.conf /usr/share/qt4/mkspecs/common/unix.conf \
		/usr/share/qt4/mkspecs/common/linux.conf \
		/usr/share/qt4/mkspecs/common/gcc-base.conf \
		/usr/share/qt4/mkspecs/common/gcc-base-unix.conf \
		/usr/share/qt4/mkspecs/common/g++-base.conf \
		/usr/share/qt4/mkspecs/common/g++-unix.conf \
		/usr/share/qt4/mkspecs/qconfig.pri \
		/usr/share/qt4/mkspecs/features/qt_functions.prf \
		/usr/share/qt4/mkspecs/features/qt_config.prf \
		/usr/share/qt4/mkspecs/features/exclusive_builds.prf \
		/usr/share/qt4/mkspecs/features/default_pre.prf \
		/usr/share/qt4/mkspecs/features/debug.prf \
		/usr/share/qt4/mkspecs/features/default_post.prf \
		/usr/share/qt4/mkspecs/features/shared.prf \
		/usr/share/qt4/mkspecs/features/unix/gdb_dwarf_index.prf \
		/usr/share/qt4/mkspecs/features/warn_on.prf \
		/usr/share/qt4/mkspecs/features/qt.prf \
		/usr/share/qt4/mkspecs/features/unix/thread.prf \
		/usr/share/qt4/mkspecs/features/moc.prf \
		/usr/share/qt4/mkspecs/features/resources.prf \
		/usr/share/qt4/mkspecs/features/uic.prf \
		/usr/share/qt4/mkspecs/features/yacc.prf \
		/usr/share/qt4/mkspecs/features/lex.prf \
		/usr/share/qt4/mkspecs/features/include_source_dir.prf \
		/usr/lib/x86_64-linux-gnu/libQtSql.prl \
		/usr/lib/x86_64-linux-gnu/libQtGui.prl \
		/usr/lib/x86_64-linux-gnu/libQtCore.prl
	$(QMAKE) -o Makefile wipeit.pro
/usr/share/qt4/mkspecs/common/unix.conf:
/usr/share/qt4/mkspecs/common/linux.conf:
/usr/share/qt4/mkspecs/common/gcc-base.conf:
/usr/share/qt4/mkspecs/common/gcc-base-unix.conf:
/usr/share/qt4/mkspecs/common/g++-base.conf:
/usr/share/qt4/mkspecs/common/g++-unix.conf:
/usr/share/qt4/mkspecs/qconfig.pri:
/usr/share/qt4/mkspecs/features/qt_functions.prf:
/usr/share/qt4/mkspecs/features/qt_config.prf:
/usr/share/qt4/mkspecs/features/exclusive_builds.prf:
/usr/share/qt4/mkspecs/features/default_pre.prf:
/usr/share/qt4/mkspecs/features/debug.prf:
/usr/share/qt4/mkspecs/features/default_post.prf:
/usr/share/qt4/mkspecs/features/shared.prf:
/usr/share/qt4/mkspecs/features/unix/gdb_dwarf_index.prf:
/usr/share/qt4/mkspecs/features/warn_on.prf:
/usr/share/qt4/mkspecs/features/qt.prf:
/usr/share/qt4/mkspecs/features/unix/thread.prf:
/usr/share/qt4/mkspecs/features/moc.prf:
/usr/share/qt4/mkspecs/features/resources.prf:
/usr/share/qt4/mkspecs/features/uic.prf:
/usr/share/qt4/mkspecs/features/yacc.prf:
/usr/share/qt4/mkspecs/features/lex.prf:
/usr/share/qt4/mkspecs/features/include_source_dir.prf:
/usr/lib/x86_64-linux-gnu/libQtSql.prl:
/usr/lib/x86_64-linux-gnu/libQtGui.prl:
/usr/lib/x86_64-linux-gnu/libQtCore.prl:
qmake:  FORCE
	@$(QMAKE) -o Makefile wipeit.pro

dist: 
	@$(CHK_DIR_EXISTS) .tmp/wipeit1.0.0 || $(MKDIR) .tmp/wipeit1.0.0 
	$(COPY_FILE) --parents $(SOURCES) $(DIST) .tmp/wipeit1.0.0/ && $(COPY_FILE) --parents maindialog.h wipeit.h scandialog.h wipedialog.h errordialog.h confirmdialog.h creditsdlg.h raiddrv.h megaraid.h dbupdatedlg.h histdialog.h sg_io_linux.h simplecrypt.h license.h sg_linux_inc.h sg_lib.h sg_pt.h .tmp/wipeit1.0.0/ && $(COPY_FILE) --parents wipeit.qrc .tmp/wipeit1.0.0/ && $(COPY_FILE) --parents wipeit.cpp main.cpp maindialog.cpp wipedialog.cpp errordialog.cpp megaraid.cpp dbupdatedlg.cpp histdialog.cpp confirmdialog.cpp creditsdlg.cpp simplecrypt.cpp license.cpp sgwrite.cpp sg_pt_common.c sg_pt_linux.c .tmp/wipeit1.0.0/ && $(COPY_FILE) --parents maindialog.ui scandialog.ui wipedialog.ui errordialog.ui dbupdatedlg.ui dbautoupdatedlg.ui histdialog.ui confirmdialog.ui creditsdlg.ui .tmp/wipeit1.0.0/ && (cd `dirname .tmp/wipeit1.0.0` && $(TAR) wipeit1.0.0.tar wipeit1.0.0 && $(COMPRESS) wipeit1.0.0.tar) && $(MOVE) `dirname .tmp/wipeit1.0.0`/wipeit1.0.0.tar.gz . && $(DEL_FILE) -r .tmp/wipeit1.0.0


clean:compiler_clean 
	-$(DEL_FILE) $(OBJECTS)
	-$(DEL_FILE) *~ core *.core


####### Sub-libraries

distclean: clean
	-$(DEL_FILE) $(TARGET) 
	-$(DEL_FILE) Makefile


liblshw/liblshw.a: FORCE
	make -C liblshw

check: first

mocclean: compiler_moc_header_clean compiler_moc_source_clean

mocables: compiler_moc_header_make_all compiler_moc_source_make_all

compiler_moc_header_make_all: moc_maindialog.cpp moc_wipedialog.cpp moc_errordialog.cpp moc_confirmdialog.cpp moc_creditsdlg.cpp moc_dbupdatedlg.cpp moc_histdialog.cpp
compiler_moc_header_clean:
	-$(DEL_FILE) moc_maindialog.cpp moc_wipedialog.cpp moc_errordialog.cpp moc_confirmdialog.cpp moc_creditsdlg.cpp moc_dbupdatedlg.cpp moc_histdialog.cpp
moc_maindialog.cpp: ui_maindialog.h \
		wipeit.h \
		maindialog.h
	/usr/lib/x86_64-linux-gnu/qt4/bin/moc $(DEFINES) $(INCPATH) maindialog.h -o moc_maindialog.cpp

moc_wipedialog.cpp: ui_wipedialog.h \
		wipeit.h \
		maindialog.h \
		ui_maindialog.h \
		wipedialog.h
	/usr/lib/x86_64-linux-gnu/qt4/bin/moc $(DEFINES) $(INCPATH) wipedialog.h -o moc_wipedialog.cpp

moc_errordialog.cpp: ui_errordialog.h \
		wipeit.h \
		errordialog.h
	/usr/lib/x86_64-linux-gnu/qt4/bin/moc $(DEFINES) $(INCPATH) errordialog.h -o moc_errordialog.cpp

moc_confirmdialog.cpp: ui_confirmdialog.h \
		confirmdialog.h
	/usr/lib/x86_64-linux-gnu/qt4/bin/moc $(DEFINES) $(INCPATH) confirmdialog.h -o moc_confirmdialog.cpp

moc_creditsdlg.cpp: ui_creditsdlg.h \
		maindialog.h \
		ui_maindialog.h \
		wipeit.h \
		creditsdlg.h
	/usr/lib/x86_64-linux-gnu/qt4/bin/moc $(DEFINES) $(INCPATH) creditsdlg.h -o moc_creditsdlg.cpp

moc_dbupdatedlg.cpp: ui_dbupdatedlg.h \
		ui_dbautoupdatedlg.h \
		wipeit.h \
		maindialog.h \
		ui_maindialog.h \
		dbupdatedlg.h
	/usr/lib/x86_64-linux-gnu/qt4/bin/moc $(DEFINES) $(INCPATH) dbupdatedlg.h -o moc_dbupdatedlg.cpp

moc_histdialog.cpp: ui_histdialog.h \
		histdialog.h
	/usr/lib/x86_64-linux-gnu/qt4/bin/moc $(DEFINES) $(INCPATH) histdialog.h -o moc_histdialog.cpp

compiler_rcc_make_all: qrc_wipeit.cpp
compiler_rcc_clean:
	-$(DEL_FILE) qrc_wipeit.cpp
qrc_wipeit.cpp: wipeit.qrc \
		logo2.jpg \
		icon.jpg
	/usr/lib/x86_64-linux-gnu/qt4/bin/rcc -name wipeit wipeit.qrc -o qrc_wipeit.cpp

compiler_image_collection_make_all: qmake_image_collection.cpp
compiler_image_collection_clean:
	-$(DEL_FILE) qmake_image_collection.cpp
compiler_moc_source_make_all:
compiler_moc_source_clean:
compiler_uic_make_all: ui_maindialog.h ui_scandialog.h ui_wipedialog.h ui_errordialog.h ui_dbupdatedlg.h ui_dbautoupdatedlg.h ui_histdialog.h ui_confirmdialog.h ui_creditsdlg.h
compiler_uic_clean:
	-$(DEL_FILE) ui_maindialog.h ui_scandialog.h ui_wipedialog.h ui_errordialog.h ui_dbupdatedlg.h ui_dbautoupdatedlg.h ui_histdialog.h ui_confirmdialog.h ui_creditsdlg.h
ui_maindialog.h: maindialog.ui
	/usr/lib/x86_64-linux-gnu/qt4/bin/uic maindialog.ui -o ui_maindialog.h

ui_scandialog.h: scandialog.ui
	/usr/lib/x86_64-linux-gnu/qt4/bin/uic scandialog.ui -o ui_scandialog.h

ui_wipedialog.h: wipedialog.ui
	/usr/lib/x86_64-linux-gnu/qt4/bin/uic wipedialog.ui -o ui_wipedialog.h

ui_errordialog.h: errordialog.ui
	/usr/lib/x86_64-linux-gnu/qt4/bin/uic errordialog.ui -o ui_errordialog.h

ui_dbupdatedlg.h: dbupdatedlg.ui
	/usr/lib/x86_64-linux-gnu/qt4/bin/uic dbupdatedlg.ui -o ui_dbupdatedlg.h

ui_dbautoupdatedlg.h: dbautoupdatedlg.ui
	/usr/lib/x86_64-linux-gnu/qt4/bin/uic dbautoupdatedlg.ui -o ui_dbautoupdatedlg.h

ui_histdialog.h: histdialog.ui
	/usr/lib/x86_64-linux-gnu/qt4/bin/uic histdialog.ui -o ui_histdialog.h

ui_confirmdialog.h: confirmdialog.ui
	/usr/lib/x86_64-linux-gnu/qt4/bin/uic confirmdialog.ui -o ui_confirmdialog.h

ui_creditsdlg.h: creditsdlg.ui
	/usr/lib/x86_64-linux-gnu/qt4/bin/uic creditsdlg.ui -o ui_creditsdlg.h

compiler_yacc_decl_make_all:
compiler_yacc_decl_clean:
compiler_yacc_impl_make_all:
compiler_yacc_impl_clean:
compiler_lex_make_all:
compiler_lex_clean:
compiler_clean: compiler_moc_header_clean compiler_rcc_clean compiler_uic_clean 

####### Compile

wipeit.o: wipeit.cpp liblshw/hw.h \
		liblshw/main.h \
		liblshw/options.h \
		liblshw/osutils.h \
		raiddrv.h \
		wipeit.h \
		megaraid.h \
		sg_lib.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o wipeit.o wipeit.cpp

main.o: main.cpp maindialog.h \
		ui_maindialog.h \
		wipeit.h \
		license.h \
		megaraid.h \
		raiddrv.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o main.o main.cpp

maindialog.o: maindialog.cpp maindialog.h \
		ui_maindialog.h \
		wipeit.h \
		scandialog.h \
		ui_scandialog.h \
		wipedialog.h \
		ui_wipedialog.h \
		dbupdatedlg.h \
		ui_dbupdatedlg.h \
		ui_dbautoupdatedlg.h \
		errordialog.h \
		ui_errordialog.h \
		confirmdialog.h \
		ui_confirmdialog.h \
		histdialog.h \
		ui_histdialog.h \
		creditsdlg.h \
		ui_creditsdlg.h \
		liblshw/osutils.h \
		raiddrv.h \
		license.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o maindialog.o maindialog.cpp

wipedialog.o: wipedialog.cpp wipedialog.h \
		ui_wipedialog.h \
		wipeit.h \
		maindialog.h \
		ui_maindialog.h \
		dbupdatedlg.h \
		ui_dbupdatedlg.h \
		ui_dbautoupdatedlg.h \
		liblshw/osutils.h \
		raiddrv.h \
		creditsdlg.h \
		ui_creditsdlg.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o wipedialog.o wipedialog.cpp

errordialog.o: errordialog.cpp errordialog.h \
		ui_errordialog.h \
		wipeit.h \
		liblshw/osutils.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o errordialog.o errordialog.cpp

megaraid.o: megaraid.cpp megaraid.h \
		raiddrv.h \
		wipeit.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o megaraid.o megaraid.cpp

dbupdatedlg.o: dbupdatedlg.cpp dbupdatedlg.h \
		ui_dbupdatedlg.h \
		ui_dbautoupdatedlg.h \
		wipeit.h \
		maindialog.h \
		ui_maindialog.h \
		liblshw/osutils.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o dbupdatedlg.o dbupdatedlg.cpp

histdialog.o: histdialog.cpp histdialog.h \
		ui_histdialog.h \
		maindialog.h \
		ui_maindialog.h \
		wipeit.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o histdialog.o histdialog.cpp

confirmdialog.o: confirmdialog.cpp confirmdialog.h \
		ui_confirmdialog.h \
		maindialog.h \
		ui_maindialog.h \
		wipeit.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o confirmdialog.o confirmdialog.cpp

creditsdlg.o: creditsdlg.cpp creditsdlg.h \
		ui_creditsdlg.h \
		maindialog.h \
		ui_maindialog.h \
		wipeit.h \
		license.h \
		simplecrypt.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o creditsdlg.o creditsdlg.cpp

simplecrypt.o: simplecrypt.cpp simplecrypt.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o simplecrypt.o simplecrypt.cpp

license.o: license.cpp license.h \
		simplecrypt.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o license.o license.cpp

sgwrite.o: sgwrite.cpp sg_lib.h \
		sg_pt.h \
		sg_io_linux.h \
		sg_linux_inc.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o sgwrite.o sgwrite.cpp

sg_pt_common.o: sg_pt_common.c sg_pt.h \
		liblshw/config.h
	$(CC) -c $(CFLAGS) $(INCPATH) -o sg_pt_common.o sg_pt_common.c

sg_pt_linux.o: sg_pt_linux.c liblshw/config.h \
		sg_pt.h \
		sg_lib.h \
		sg_linux_inc.h
	$(CC) -c $(CFLAGS) $(INCPATH) -o sg_pt_linux.o sg_pt_linux.c

moc_maindialog.o: moc_maindialog.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o moc_maindialog.o moc_maindialog.cpp

moc_wipedialog.o: moc_wipedialog.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o moc_wipedialog.o moc_wipedialog.cpp

moc_errordialog.o: moc_errordialog.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o moc_errordialog.o moc_errordialog.cpp

moc_confirmdialog.o: moc_confirmdialog.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o moc_confirmdialog.o moc_confirmdialog.cpp

moc_creditsdlg.o: moc_creditsdlg.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o moc_creditsdlg.o moc_creditsdlg.cpp

moc_dbupdatedlg.o: moc_dbupdatedlg.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o moc_dbupdatedlg.o moc_dbupdatedlg.cpp

moc_histdialog.o: moc_histdialog.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o moc_histdialog.o moc_histdialog.cpp

qrc_wipeit.o: qrc_wipeit.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o qrc_wipeit.o qrc_wipeit.cpp

####### Install

install:   FORCE

uninstall:   FORCE

FORCE:

