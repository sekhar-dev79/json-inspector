QT       += core gui svg

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

TARGET   = JsonInspector
TEMPLATE = app

# ─── Include Paths ────────────────────────────────────────────────────────────
# Each folder is added so headers can be included by filename only.
# $$PWD is the absolute path to the directory containing this .pro file.
# This makes includes portable — they work on any machine, any OS.
INCLUDEPATH += $$PWD/ui
INCLUDEPATH += $$PWD/theme
INCLUDEPATH += $$PWD/icons
INCLUDEPATH += $$PWD/utils
INCLUDEPATH += $$PWD/core
INCLUDEPATH += $$PWD/models

# ─── Sources ──────────────────────────────────────────────────────────────────
SOURCES += \
    core/jsondiff.cpp \
    core/jsonparser.cpp \
    main.cpp \
    models/diffmodel.cpp \
    models/jsonmodel.cpp \
    ui/diffpanel.cpp \
    ui/errorbannerwidget.cpp \
    ui/inputpanel.cpp \
    ui/jsonhighlighter.cpp \
    ui/jsonsearchdelegate.cpp \
    ui/jsonsearchproxy.cpp \
    ui/jsontreedelegate.cpp \
    ui/jsontreeview.cpp \
    ui/mainwindow.cpp \
    theme/thememanager.cpp \
    ui/searchbar.cpp \
    ui/toolbarwidget.cpp

# ─── Headers ──────────────────────────────────────────────────────────────────
HEADERS += \
    core/diffentry.h \
    core/jsondiff.h \
    core/jsonparser.h \
    models/diffmodel.h \
    models/jsonmodel.h \
    ui/diffpanel.h \
    ui/errorbannerwidget.h \
    ui/inputpanel.h \
    ui/jsonhighlighter.h \
    ui/jsonsearchdelegate.h \
    ui/jsonsearchproxy.h \
    ui/jsontreedelegate.h \
    ui/jsontreeview.h \
    ui/mainwindow.h \
    theme/thememanager.h \
    icons/iconhelper.h \
    ui/searchbar.h \
    ui/toolbarwidget.h \
    utils/constants.h

# ─── Resources ────────────────────────────────────────────────────────────────
RESOURCES += \
    resources.qrc

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

