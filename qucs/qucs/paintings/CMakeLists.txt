# qucs/paintings library

include_directories(${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR})

# INCLUDES = $(X11_INCLUDES) $(QT_INCLUDES) -I$(top_srcdir)/qucs

set(PAINTINGS_HDRS
    arrow.h
    arrowdialog.h
    ellipse.h
    ellipsearc.h
    filldialog.h
    graphicline.h
    graphictext.h
    graphictextdialog.h
    id_dialog.h
    id_text.h
    painting.h
    paintings.h
    portsymbol.h
    rectangle.h)

set(PAINTINGS_SRCS
    arrow.cpp
    arrowdialog.cpp
    graphicline.cpp
    id_text.cpp
    graphictext.cpp
    painting.cpp
    ellipse.cpp
    graphictextdialog.cpp
    portsymbol.cpp
    ellipsearc.cpp
    rectangle.cpp
    filldialog.cpp
    id_dialog.cpp)

set(PAINTINGS_MOC_HDRS arrowdialog.h filldialog.h graphictextdialog.h
                       id_dialog.h)

qt4_wrap_cpp(PAINTINGS_MOC_SRCS ${PAINTINGS_MOC_HDRS})

add_library(paintings OBJECT ${PAINTINGS_HDRS} ${PAINTINGS_SRCS}
                             ${PAINTINGS_MOC_SRCS})
