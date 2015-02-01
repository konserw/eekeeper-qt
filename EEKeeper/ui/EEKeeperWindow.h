#ifndef EEKEEPERWINDOW_H
#define EEKEEPERWINDOW_H

#include "OpenSavedGameDialog.h"
#include "Inf2DA.h"
#include "SavedGameWidget.h"
#include "ValueList.h"

#include <QMainWindow>
#include <QMap>

namespace Ui {
class EEKeeperWindow;
}

typedef QMap<QString, SavedGameWidget *> SavedGameWidgetMap;

class EEKeeperWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit EEKeeperWindow(QWidget *parent = 0);
    ~EEKeeperWindow();

    void closeEvent(QCloseEvent *event);
    void show();

private:
    void readSettings();
    bool loadResources();

    bool ProcessList(
            QString &pszResource,                       // Resource string.
            CValueList &vl,								// Pointer to a list to receive value pairs.
            bool bUseRange=false,						// True to limit values by a range.
            quint32 nMin=0,								// Minimum value for the range.
            quint32 nMax=0,								// Maximum value for the range.
            int nBracketThreshold=-1					// Any values above this value with get [brackets] around the text. -1 disables it.
            );

    bool Process2da(QString &strResource, CInf2DA *p2DA);

    bool GetLine(const char *pszBuf, int &nStart, QString &strLine);

    bool SplitLine(QString &strLine, quint32 &nValue, QString &strText);

    bool CreateRacialEnemyList();

    bool CreateWeaponProfList();

    bool CreateKitList();
    
private slots:
    void on_actionOpen_Saved_Game_triggered();

    void on_actionToolbar_toggled(bool arg1);

    void on_actionStatus_Bar_toggled(bool arg1);

    void on_actionInstallation_Directory_triggered();

    void on_tabWidget_tabCloseRequested(int index);

    void on_actionSave_Game_triggered();

    void on_actionSpell_Browser_triggered();

    void on_actionKits_triggered();

    void on_actionAffects_triggered();

private:
    Ui::EEKeeperWindow  *ui;
    SavedGameWidgetMap  m_games;

    void DeleteTab(int index);
    void ResetStatus(bool bEnabled);
    void SaveGameDialog(SavedGameWidget *gameWidget);
};

#endif // EEKEEPERWINDOW_H
