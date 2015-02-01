#ifndef SPELLTAB_H
#define SPELLTAB_H

#include "EEKeeper.h"
#include "InfCreature.h"
#include "InfKey.h"

#include <QWidget>

#define TABSPELL_MAXMEM 50

struct SpellTabData {
    CResInfo    *pInfo;
    QString     strName;
    uchar       chMem;
    uchar       chLevel;
    uchar       chTimesCanCast;

    // to satisfy Q_DECLARE_METATYPE requirements
    SpellTabData() : pInfo(NULL), strName(""), chMem(0), chLevel(0), chTimesCanCast(0) { }

    SpellTabData(const SpellTabData &copy) :
        pInfo(copy.pInfo),
        strName(copy.strName),
        chMem(copy.chMem),
        chLevel(copy.chLevel),
        chTimesCanCast(copy.chTimesCanCast)
    {

    }

    ~SpellTabData() { /* not responsible for pInfo */ }

    // easy of use
    operator QVariant() const { return QVariant::fromValue(*this); }
};

// declare our SpellTabData as a QMetaType for storage in our table
Q_DECLARE_METATYPE(SpellTabData)

namespace Ui {
class SpellTab;
}

class SpellTab : public QWidget
{
    Q_OBJECT
    
public:
    explicit SpellTab(QWidget *parent = 0);
    ~SpellTab();

    // Sets the spells from a character. wType needs to be one of the INC_CRE_ST defines found
    // in InfCre.h.
    void SetSpells(quint16 wType, int nSpellCount, SPELLDATA *pData);

    // pData must point to an array big enough to hold as many entries as GetSpellCount()
    // returns.
    void GetSpells(SPELLDATA *pData);

    int  GetSpellCount();
    
private slots:
    void on_pushButtonAddSpell_clicked();

    void on_pushButtonRemoveSpell_clicked();

    void on_pushButtonMemMinus_clicked();

    void on_pushButtonMemAdd_clicked();

    void on_pushButtonKit_clicked();

private:
    void ClearList();
    void UpdateButtonStates();

    // Adds a spell to the list. Returns TRUE is successful.
    bool AddSpellToList(SPELLDATA *pData);

    // Modifies the currently selected spell by the given mod
    void ModifySpellMem(int mod);

private:
    Ui::SpellTab    *ui;
    quint16         m_wSpellType;
    bool            m_bInnates;
};

#endif // SPELLTAB_H
