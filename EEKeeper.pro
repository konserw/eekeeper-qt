#-------------------------------------------------
#
# Project created by QtCreator 2013-02-26T13:46:29
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = EEKeeper
TEMPLATE = app


SOURCES +=\
    EEKeeper/main.cpp \
    EEKeeper/Global.cpp \
    EEKeeper/InfCreature.cpp \
    EEKeeper/InfGame.cpp \
    EEKeeper/InfBam.cpp \
    EEKeeper/InfBifFile.cpp \
    EEKeeper/InfKey.cpp \
    EEKeeper/InfTlk.cpp \
    EEKeeper/Inf2DA.cpp \
    EEKeeper/Log.cpp \
    EEKeeper/InfChr.cpp \
    EEKeeper/SpellBitmaps.cpp \
    EEKeeper/PalImageList.cpp \
    EEKeeper/ValueList.cpp \
    EEKeeper/EEKeeper.cpp \
    EEKeeper/ui/EEKeeperWindow.cpp \
    EEKeeper/ui/OpenSavedGameDialog.cpp \
    EEKeeper/ui/InstallationDirectory.cpp \
    EEKeeper/ui/SavedGameWidget.cpp \
    EEKeeper/ui/CharacterSheetWidget.cpp \
    EEKeeper/ui/SpellTab.cpp \
    EEKeeper/ui/MemorizationTab.cpp \
    EEKeeper/ui/InventoryTab.cpp \
    EEKeeper/ui/ProficienciesTab.cpp \
    EEKeeper/ui/StringFinderDialog.cpp \
    EEKeeper/ui/SpellBrowserWidget.cpp \
    EEKeeper/ui/ValueListDialog.cpp \
    EEKeeper/ui/ValueItemDialog.cpp \
    EEKeeper/ui/ItemBrowserWidget.cpp \
    EEKeeper/ui/SaveGameNameDialog.cpp

HEADERS  += \
    EEKeeper/include/defines.h \
    EEKeeper/include/Global.h \
    EEKeeper/include/InfBam.h \
    EEKeeper/include/InfBifFile.h \
    EEKeeper/include/InfCreature.h \
    EEKeeper/include/InfGame.h \
    EEKeeper/include/InfKey.h \
    EEKeeper/include/InfTlk.h \
    EEKeeper/include/Inf2DA.h \
    EEKeeper/include/Log.h \
    EEKeeper/include/InfChr.h \
    EEKeeper/include/SpellBitmaps.h \
    EEKeeper/include/PalImageList.h \
    EEKeeper/include/ValueList.h \
    EEKeeper/include/EEKeeper.h \
    EEKeeper/ui/EEKeeperWindow.h \
    EEKeeper/ui/OpenSavedGameDialog.h \
    EEKeeper/ui/InstallationDirectory.h \
    EEKeeper/ui/SavedGameWidget.h \
    EEKeeper/ui/CharacterSheetWidget.h \
    EEKeeper/ui/SpellTab.h \
    EEKeeper/ui/MemorizationTab.h \
    EEKeeper/ui/InventoryTab.h \
    EEKeeper/ui/ProficienciesTab.h \
    EEKeeper/ui/StringFinderDialog.h \
    EEKeeper/ui/SpellBrowserWidget.h \
    EEKeeper/ui/ValueListDialog.h \
    EEKeeper/ui/ValueItemDialog.h \
    EEKeeper/ui/ItemBrowserWidget.h \
    EEKeeper/ui/SaveGameNameDialog.h

linux {
FORMS   += \
    EEKeeper/ui/linux/CharacterSheetWidget.ui \
    EEKeeper/ui/linux/EEKeeperWindow.ui \
    EEKeeper/ui/linux/InstallationDirectory.ui \
    EEKeeper/ui/linux/InventoryTab.ui \
    EEKeeper/ui/linux/MemorizationTab.ui \
    EEKeeper/ui/linux/OpenSavedGameDialog.ui \
    EEKeeper/ui/linux/ProficienciesTab.ui \
    EEKeeper/ui/linux/SavedGameWidget.ui \
    EEKeeper/ui/linux/SpellBrowserWidget.ui \
    EEKeeper/ui/linux/SpellTab.ui \
    EEKeeper/ui/linux/StringFinderDialog.ui \
    EEKeeper/ui/linux/ValueListDialog.ui \
    EEKeeper/ui/linux/ValueItemDialog.ui \
    EEKeeper/ui/linux/ItemBrowserWidget.ui \
    EEKeeper/ui/linux/SaveGameNameDialog.ui
}

mac {
FORMS    += \
    EEKeeper/ui/mac/CharacterSheetWidget.ui \
    EEKeeper/ui/mac/EEKeeperWindow.ui \
    EEKeeper/ui/mac/InstallationDirectory.ui \
    EEKeeper/ui/mac/InventoryTab.ui \
    EEKeeper/ui/mac/MemorizationTab.ui \
    EEKeeper/ui/mac/OpenSavedGameDialog.ui \
    EEKeeper/ui/mac/ProficienciesTab.ui \
    EEKeeper/ui/mac/SavedGameWidget.ui \
    EEKeeper/ui/mac/SpellBrowserWidget.ui \
    EEKeeper/ui/mac/SpellTab.ui \
    EEKeeper/ui/mac/StringFinderDialog.ui \
    EEKeeper/ui/mac/ValueListDialog.ui \
    EEKeeper/ui/mac/ValueItemDialog.ui \
    EEKeeper/ui/mac/ItemBrowserWidget.ui \
    EEKeeper/ui/mac/SaveGameNameDialog.ui
}

win32 {
FORMS   += \
    EEKeeper/ui/win32/CharacterSheetWidget.ui \
    EEKeeper/ui/win32/EEKeeperWindow.ui \
    EEKeeper/ui/win32/InstallationDirectory.ui \
    EEKeeper/ui/win32/InventoryTab.ui \
    EEKeeper/ui/win32/MemorizationTab.ui \
    EEKeeper/ui/win32/OpenSavedGameDialog.ui \
    EEKeeper/ui/win32/ProficienciesTab.ui \
    EEKeeper/ui/win32/SavedGameWidget.ui \
    EEKeeper/ui/win32/SpellBrowserWidget.ui \
    EEKeeper/ui/win32/SpellTab.ui \
    EEKeeper/ui/win32/StringFinderDialog.ui \
    EEKeeper/ui/win32/ValueListDialog.ui \
    EEKeeper/ui/win32/ValueItemDialog.ui \
    EEKeeper/ui/win32/ItemBrowserWidget.ui \
    EEKeeper/ui/win32/SaveGameNameDialog.ui
}

RESOURCES += \
    eekeeper.qrc

INCLUDEPATH += \
    EEKeeper/include \
    EEKeeper/ui \
    EEKeeper
