#ifndef MEMORIZATIONTAB_H
#define MEMORIZATIONTAB_H

#include "InfCreature.h"

#include <QWidget>

namespace Ui {
class MemorizationTab;
}

class MemorizationTab : public QWidget
{
    Q_OBJECT
    
public:
    explicit MemorizationTab(QWidget *parent = 0);
    ~MemorizationTab();

    void SetData(MEMINFO *pMemInfo, int nCount);
    void GetData(MEMINFO *pMemInfo, int nCount);
    
private slots:
    void on_pushButtonMaxAdd_clicked();

    void on_pushButtonMaxMinus_clicked();

private:
    Ui::MemorizationTab *ui;

    void ModifyMax(int mod);
};

#endif // MEMORIZATIONTAB_H
