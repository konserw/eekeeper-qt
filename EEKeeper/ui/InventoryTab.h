#ifndef INVENTORYTAB_H
#define INVENTORYTAB_H

#include "InfBifFile.h"
#include "InfCreature.h"

#include <QWidget>

namespace Ui {
class InventoryTab;
}

class InventoryTab : public QWidget
{
    Q_OBJECT
    
public:
    explicit InventoryTab(QWidget *parent = 0);
    ~InventoryTab();

    void SetItems(INF_CRE_ITEM *pItem);
    void GetItems(INF_CRE_ITEM *pItem);
    
private:
    Ui::InventoryTab *ui;

    INF_CRE_ITEM     m_infItems[INF_NUM_ITEMSLOTS];
    INF_ITM          m_infItm[INF_NUM_ITEMSLOTS];

    void ClearItems();
};

#endif // INVENTORYTAB_H
