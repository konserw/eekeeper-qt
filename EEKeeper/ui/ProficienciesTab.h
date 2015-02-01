#ifndef PROFICIENCIESTAB_H
#define PROFICIENCIESTAB_H

#include "InfCreature.h"
#include "ValueList.h"
#include <QWidget>

struct PROFDATA {
    quint8  chProf;
    int     nFirstClass;
    int     nSecondClass;

    // to satisfy Q_DECLARE_METATYPE requirements
    PROFDATA() : chProf(0), nFirstClass(0), nSecondClass(0) { }

    PROFDATA(const PROFDATA &copy) {
        chProf = copy.chProf;
        nFirstClass = copy.nFirstClass;
        nSecondClass = copy.nSecondClass;
    }

    ~PROFDATA() { }

    // easy of use
    operator QVariant() const { return QVariant::fromValue(*this); }
};

// declare our PROFDATA as a QMetaType for storage in our table
Q_DECLARE_METATYPE(PROFDATA)

namespace Ui {
class ProficienciesTab;
}

class ProficienciesTab : public QWidget
{
    Q_OBJECT
    
public:
    explicit ProficienciesTab(QWidget *parent = 0);
    ~ProficienciesTab();

    // Sets the proficiencies based on the list provided. This deletes and clears
    // the list so the caller doesn't have to
    void SetProfs(CProfDataPlist &list);

    // Gets the proficiencies that are set for storing.
    void GetProfs(CProfDataPlist &list);

    // Fills in the table with the names of the proficiencies
    void FillList(const CValueList &vlProf);
    
private slots:
    void on_pushButtonAddFirst_clicked();

    void on_pushButtonAddSecond_clicked();

    void on_pushButtonSubFirst_clicked();

    void on_pushButtonSubSecond_clicked();

private:
    Ui::ProficienciesTab *ui;

    void UpdateCount(quint8 chProf, int nFirstClass, int nSecondClass);
    void ModifyCount(int nModFirstClass, int nModSecondClass);
    bool GetProfData(int row, PROFDATA &pData);
};

#endif // PROFICIENCIESTAB_H
