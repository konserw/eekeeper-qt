#ifndef SAVEDGAMEWIDGET_H
#define SAVEDGAMEWIDGET_H

#include "CharacterSheetWidget.h"
#include "InfCreature.h"
#include "InfGame.h"

#include <QWidget>

namespace Ui {
class SavedGameWidget;
}

class SavedGameWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit SavedGameWidget(QWidget *parent, QString &directory);
    ~SavedGameWidget();

    // Determine if the game is open
    bool IsOpen() const { return m_open; }

    // Determine if the game has changed
    bool HasChanged();

    // Update the game
    void Update();

    // Save the game
    bool Write(const QString &strName = "", bool bOverwrite = true);

    // Get the saved game directory name
    const QString GameName() const { return m_dir.section('-', -1).remove(QDir::separator()); }
    
private slots:
    void on_scrollBarCharacter_valueChanged(int value);

    void on_toolStrRef_clicked();

private:
    Ui::SavedGameWidget *ui;
    QString         m_dir;
    CInfGame        m_game;
    CInfCreature    *m_current;
    bool            m_open;
};

#endif // SAVEDGAMEWIDGET_H
