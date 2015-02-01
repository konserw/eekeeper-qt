#ifndef CHARACTERSHEETWIDGET_H
#define CHARACTERSHEETWIDGET_H

#include "InfBifFile.h"
#include "InfCreature.h"

#include <QIntValidator>
#include <QWidget>

namespace Ui {
class CharacterSheetWidget;
}

class CharacterSheetWidget : public QWidget
{
    Q_OBJECT
    
private:
    static const QIntValidator attrValidator;
    static const QIntValidator thiefValidator;
    static const QIntValidator positiveValidator;
    static const QIntValidator resistanceValidator;
    static const QIntValidator shortNegativeValidator;
    static const QIntValidator twentyValidator;

public:
    explicit CharacterSheetWidget(QWidget *parent = 0);
    CharacterSheetWidget(QWidget *parent, CInfCreature *creature);

    ~CharacterSheetWidget();

    void InitializeUI(CInfCreature *creature);
    void SetupValidators();

    // Gets the current index of the tab set for consistency
    int GetTabIndex();
    void SetTabIndex(int index);

    void Update();

private:
    Ui::CharacterSheetWidget *ui;

    CInfCreature    *m_creature;

    void SetupInventory();
    void SetupSpells();
    void SetupMemorization();
    void SetupProficiencies();
};

#endif // CHARACTERSHEETWIDGET_H
