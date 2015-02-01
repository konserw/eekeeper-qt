#ifndef OPENSAVEDGAMEDIALOG_H
#define OPENSAVEDGAMEDIALOG_H

#include <QDialog>
#include <QDir>

namespace Ui {
class OpenSavedGameDialog;
}

class OpenSavedGameDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit OpenSavedGameDialog(QWidget *parent = 0);
    ~OpenSavedGameDialog();

    QString getSaveDirectory() const { return m_saveDirectory; }
    QString getSaveName() const { return m_saveName; }
    
private slots:
    void on_buttonOK_clicked();

    void on_buttonCancel_clicked();

    void on_toolOpenSavedGame_clicked();

    void on_checkMultiplayer_stateChanged(int arg1);

    void on_checkBlackPits_stateChanged(int arg1);

    void on_tableWidget_clicked(const QModelIndex &index);

    void on_tableWidget_doubleClicked(const QModelIndex &index);

private:
    Ui::OpenSavedGameDialog *ui;
    QDir *savedir;
    QString m_saveDirectory;
    QString m_saveName;

    static const QString strQuickSave;
    static const QString strAutoSave;

    void reset();
    void updateTableWidget(const QString &subdir);
};

#endif // OPENSAVEDGAMEDIALOG_H
