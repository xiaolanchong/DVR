#*****************************************************************************
#                                                                            *
# Make file for VMS                                                          *
# Author : J.Jansen (joukj@hrem.stm.tudelft.nl)                              *
# Date : 10 November 1999                                                    *
#                                                                            *
#*****************************************************************************
.first
	define wx [--.include.wx]

.ifdef __WXMOTIF__
CXX_DEFINE = /define=(__WXMOTIF__=1)/name=(as_is,short)\
	   /assume=(nostdnew,noglobal_array_new)
CC_DEFINE = /define=(__WXMOTIF__=1)/name=(as_is,short)
.else
.ifdef __WXX11__
CXX_DEFINE = /define=(__WXX11__=1,__WXUNIVERSAL__==1)/float=ieee\
	/name=(as_is,short)/assume=(nostdnew,noglobal_array_new)
CC_DEFINE = /define=(__WXX11__=1,__WXUNIVERSAL__==1)/float=ieee\
	/name=(as_is,short)
.else
CXX_DEFINE =
.endif
.endif

.suffixes : .cpp

.cpp.obj :
	cxx $(CXXFLAGS)$(CXX_DEFINE) $(MMS$TARGET_NAME).cpp
.c.obj :
	cc $(CFLAGS)$(CC_DEFINE) $(MMS$TARGET_NAME).c

OBJECTS = \
		bitmap.obj,\
		brush.obj,\
		glcanvas.obj,\
		pen.obj,\
		region.obj,\
		utilsx.obj

OBJECTS_X11=app.obj,dc.obj,\
		dcclient.obj,\
		dcmemory.obj,\
		dcscreen.obj,\
		evtloop.obj,\
		font.obj,\
		gdiobj.obj,\
		gsockx11.obj,\
		icon.obj,\
		minifram.obj,\
		nanox.obj,\
		palette.obj,\
		popupwin.obj,\
		reparent.obj,\
		settings.obj,\
		toplevel.obj,\
		utils.obj,\
		window.obj,\
		colour.obj,\
		cursor.obj,\
		data.obj,\
		dataobj.obj,\
		clipbrd.obj


SOURCES = \
		app.cpp,\
		bitmap.cpp,\
		brush.cpp,\
		glcanvas.cpp,\
		pen.cpp,\
		region.cpp,\
		utilsx.cpp,\
		dc.cpp,\
		dcclient.cpp,\
		dcmemory.cpp,\
		dcscreen.cpp,\
		evtloop.cpp,\
		font.cpp,\
		gdiobj.cpp,\
		gsockx11.cpp,\
		icon.cpp,\
		minifram.cpp,\
		nanox.c,\
		palette.cpp,\
		popupwin.cpp,\
		reparent.cpp,\
		settings.cpp,\
		toplevel.cpp,\
		utils.cpp,\
		window.cpp,\
		colour.cpp,\
		cursor.cpp,\
		data.cpp,\
		dataobj.cpp,\
		clipbrd.cpp

all : $(SOURCES)
	$(MMS)$(MMSQUALIFIERS) $(OBJECTS)
.ifdef __WXMOTIF__
	library [--.lib]libwx_motif.olb $(OBJECTS)
.else
.ifdef __WXX11__
	library [--.lib]libwx_x11_univ.olb $(OBJECTS)
	$(MMS)$(MMSQUALIFIERS) $(OBJECTS_X11)
	library [--.lib]libwx_x11_univ.olb $(OBJECTS_X11)
.endif
.endif

bitmap.obj : bitmap.cpp
brush.obj : brush.cpp
glcanvas.obj : glcanvas.cpp
pen.obj : pen.cpp
region.obj : region.cpp
utilsx.obj : utilsx.cpp
dc.obj : dc.cpp
dcclient.obj : dcclient.cpp
dcmemory.obj : dcmemory.cpp
dcscreen.obj : dcscreen.cpp
evtloop.obj : evtloop.cpp
font.obj : font.cpp
gdiobj.obj : gdiobj.cpp
gsockx11.obj : gsockx11.cpp
icon.obj : icon.cpp
minifram.obj : minifram.cpp
nanox.obj : nanox.c
palette.obj : palette.cpp
popupwin.obj : popupwin.cpp
reparent.obj : reparent.cpp
settings.obj : settings.cpp
toplevel.obj : toplevel.cpp
utils.obj : utils.cpp
window.obj : window.cpp
colour.obj : colour.cpp
app.obj : app.cpp
cursor.obj : cursor.cpp
data.obj : data.cpp
dataobj.obj : dataobj.cpp
clipbrd.obj : clipbrd.cpp
