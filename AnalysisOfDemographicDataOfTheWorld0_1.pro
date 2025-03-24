QT += core gui widgets network charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

SOURCES += \
    dataeditdialog.cpp \
    dataloader.cpp \
    demographicsmanager.cpp \
    documentationdialog.cpp \
    findcountrydialog.cpp \
    infobubble.cpp \ # Добавляем новый файл
    main.cpp \
    mainwindow.cpp \
    mapwidget.cpp \
    statswidget.cpp \
    verticalbutton.cpp

HEADERS += \
    CustomException.h \
    DataLoader.h \
    DemographicsManager.h \
    InfoBubble.h \ # Добавляем новый файл
    MainWindow.h \
    MapWidget.h \
    StatsWidget.h \
    VerticalButton.h \
    abstractwidget.h \
    dataeditdialog.h \
    documentationdialog.h \
    findcountrydialog.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc
