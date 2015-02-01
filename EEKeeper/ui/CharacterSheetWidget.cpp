#include "CharacterSheetWidget.h"
#include "ui_CharacterSheetWidget.h"

#include "EEKeeper.h"
#include "InfTlk.h"
#include "SpellTab.h"
#include "ValueList.h"

#include <QMessageBox>

// VALIDATORS
const QIntValidator CharacterSheetWidget::attrValidator(2, 25);
const QIntValidator CharacterSheetWidget::thiefValidator(0, 255);
const QIntValidator CharacterSheetWidget::positiveValidator(0, 100000000);
const QIntValidator CharacterSheetWidget::resistanceValidator(0, 255);
const QIntValidator CharacterSheetWidget::shortNegativeValidator(-100, 100);
const QIntValidator CharacterSheetWidget::twentyValidator(1, 20);

CharacterSheetWidget::CharacterSheetWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CharacterSheetWidget)
{
    ui->setupUi(this);
}

CharacterSheetWidget::CharacterSheetWidget(QWidget *parent, CInfCreature *creature) :
    QWidget(parent),
    ui(new Ui::CharacterSheetWidget)
{
    ui->setupUi(this);

    // setup our value lists
    _vlGender.FillComboBox(*(ui->comboBoxGender));
    _vlRace.FillComboBox(*(ui->comboBoxRace));
    _vlAlignment.FillComboBox(*(ui->comboBoxAlignment));
    _vlClass.FillComboBox(*(ui->comboBoxClass));
    _vlKit.FillComboBox(*(ui->comboBoxKit));
    _vlRacialEnemy.FillComboBox(*(ui->comboBoxRacialEnemy));
    _vlEnemyAlly.FillComboBox(*(ui->comboBoxEnemyAlly));

    // initialize the UI
    InitializeUI(creature);

    // setup our validators
    SetupValidators();
}

CharacterSheetWidget::~CharacterSheetWidget()
{
    delete ui;
}

void CharacterSheetWidget::SetupValidators()
{
    // CHARACTERISTICS
    _vlGender.SetComboBoxValue(*(ui->comboBoxGender), m_creature->GetGender(), true);
    _vlRace.SetComboBoxValue(*(ui->comboBoxRace), m_creature->GetRace(), true);
    _vlAlignment.SetComboBoxValue(*(ui->comboBoxAlignment), m_creature->GetAlignment(), true);
    _vlClass.SetComboBoxValue(*(ui->comboBoxClass), m_creature->GetClass(), true);
    _vlKit.SetComboBoxValue(*(ui->comboBoxKit), m_creature->GetKit(), false);
    _vlRacialEnemy.SetComboBoxValue(*(ui->comboBoxRacialEnemy), m_creature->GetRacialEnemy(), true);
    _vlEnemyAlly.SetComboBoxValue(*(ui->comboBoxEnemyAlly), m_creature->GetEnemyAlly(), true);
    ui->lineMovementSpeed->setValidator(&positiveValidator);

    // ATTRIBUTES
    ui->lineStr->setValidator(&attrValidator);
    ui->lineStrPlus->setValidator(&attrValidator);
    ui->lineDex->setValidator(&attrValidator);
    ui->lineInt->setValidator(&attrValidator);
    ui->lineWis->setValidator(&attrValidator);
    ui->lineCon->setValidator(&attrValidator);
    ui->lineCha->setValidator(&attrValidator);

    ui->lineBaseAC->setValidator(&shortNegativeValidator);
    ui->lineBaseHP->setValidator(&positiveValidator);
    ui->lineCurrentHP->setValidator(&shortNegativeValidator);
    ui->lineThaco->setValidator(&shortNegativeValidator);
    ui->lineReputation->setValidator(&positiveValidator);
    ui->lineExperience->setValidator(&positiveValidator);
    ui->lineExpForKill->setValidator(&positiveValidator);
    ui->lineGold->setValidator(&positiveValidator);
    ui->lineLevel1->setValidator(&positiveValidator);
    ui->lineLevel2->setValidator(&positiveValidator);
    ui->lineLevel3->setValidator(&positiveValidator);

    // SAVES
    ui->lineBreath->setValidator(&twentyValidator);
    ui->lineDeath->setValidator(&twentyValidator);
    ui->linePetrification->setValidator(&twentyValidator);
    ui->lineWand->setValidator(&twentyValidator);
    ui->lineSpells->setValidator(&twentyValidator);

    // RESISTANCES
    ui->lineAcid->setValidator(&resistanceValidator);
    ui->lineFire->setValidator(&resistanceValidator);
    ui->lineCold->setValidator(&resistanceValidator);
    ui->lineElectricity->setValidator(&resistanceValidator);
    ui->lineSlashing->setValidator(&resistanceValidator);
    ui->linePiercing->setValidator(&resistanceValidator);
    ui->lineCrushing->setValidator(&resistanceValidator);
    ui->lineMissile->setValidator(&resistanceValidator);
    ui->lineMagicFire->setValidator(&resistanceValidator);
    ui->lineMagicCold->setValidator(&resistanceValidator);
    ui->lineMagic->setValidator(&resistanceValidator);

    // THIEVES
    ui->lineMoveSilently->setValidator(&thiefValidator);
    ui->lineHideInShadows->setValidator(&thiefValidator);
    ui->lineFindTraps->setValidator(&thiefValidator);
    ui->lineSetTraps->setValidator(&thiefValidator);
    ui->lineOpenLocks->setValidator(&thiefValidator);
    ui->lineDetectIllusions->setValidator(&thiefValidator);
    ui->linePickPockets->setValidator(&thiefValidator);
}

void CharacterSheetWidget::InitializeUI(CInfCreature *creature)
{
    if (!creature) {
        qDebug() << "Cannot initialize UI with an empty creature.";
        return;
    }

    m_creature = creature;

    // ABILITIES
    ui->lineStr->setText(QString::number(m_creature->GetStr()));
    ui->lineStrPlus->setText(QString::number(m_creature->GetStrBonus()));
    ui->lineDex->setText(QString::number(m_creature->GetDex()));
    ui->lineInt->setText(QString::number(m_creature->GetInt()));
    ui->lineWis->setText(QString::number(m_creature->GetWis()));
    ui->lineCon->setText(QString::number(m_creature->GetCon()));
    ui->lineCha->setText(QString::number(m_creature->GetCha()));

    ui->lineBaseAC->setText(QString::number(m_creature->GetAC()));
    ui->lineBaseHP->setText(QString::number(m_creature->GetBaseHP()));
    ui->lineCurrentHP->setText(QString::number(m_creature->GetCurrentHP()));
    ui->lineThaco->setText(QString::number(m_creature->GetThac0()));
    ui->lineReputation->setText(QString::number(m_creature->GetReputation()));
    ui->lineExperience->setText(QString::number(m_creature->GetExp()));
    ui->lineExpForKill->setText(QString::number(m_creature->GetExpForKill()));
    ui->lineGold->setText(QString::number(m_creature->GetGold()));
    ui->lineLevel1->setText(QString::number(m_creature->GetFirstClassLevel()));
    ui->lineLevel2->setText(QString::number(m_creature->GetSecondClassLevel()));
    ui->lineLevel3->setText(QString::number(m_creature->GetThirdClassLevel()));

    ui->comboBoxAttacks->setCurrentText(QString::number(m_creature->GetAttacks()));
    //qDebug() << "    Attacks:" << QString::number(m_creature->GetAttacks());

    // SAVES
    ui->lineBreath->setText(QString::number(m_creature->GetSaveBreath()));
    ui->lineDeath->setText(QString::number(m_creature->GetSaveDeath()));
    ui->linePetrification->setText(QString::number(m_creature->GetSavePoly()));
    ui->lineWand->setText(QString::number(m_creature->GetSaveWands()));
    ui->lineSpells->setText(QString::number(m_creature->GetSaveSpells()));

    // RESISTANCES
    ui->lineAcid->setText(QString::number(m_creature->GetResAcid()));
    ui->lineFire->setText(QString::number(m_creature->GetResFire()));
    ui->lineCold->setText(QString::number(m_creature->GetResCold()));
    ui->lineElectricity->setText(QString::number(m_creature->GetResElectricity()));
    ui->lineSlashing->setText(QString::number(m_creature->GetResSlashing()));
    ui->linePiercing->setText(QString::number(m_creature->GetResPiercing()));
    ui->lineCrushing->setText(QString::number(m_creature->GetResCrushing()));
    ui->lineMissile->setText(QString::number(m_creature->GetResMissile()));
    ui->lineMagicFire->setText(QString::number(m_creature->GetResMagicFire()));
    ui->lineMagicCold->setText(QString::number(m_creature->GetResMagicCold()));
    ui->lineMagic->setText(QString::number(m_creature->GetResMagic()));

    // THIEVES
    ui->lineMoveSilently->setText(QString::number(m_creature->GetMoveSilently()));
    ui->lineHideInShadows->setText(QString::number(m_creature->GetHideInShadows()));
    ui->lineFindTraps->setText(QString::number(m_creature->GetFindTraps()));
    ui->lineSetTraps->setText(QString::number(m_creature->GetSetTraps()));
    ui->lineOpenLocks->setText(QString::number(m_creature->GetOpenLocks()));
    ui->lineDetectIllusions->setText(QString::number(m_creature->GetDetectIllusions()));
    ui->linePickPockets->setText(QString::number(m_creature->GetPickPockets()));

    // INVENTORY
    SetupInventory();

    // PROFICIENCIES
    SetupProficiencies();

    // MEMORIZATION
    SetupMemorization();

    // SPELLS
    SetupSpells();
}

int CharacterSheetWidget::GetTabIndex()
{
    return ui->tabSheet->currentIndex();
}

void CharacterSheetWidget::SetTabIndex(int index)
{
    ui->tabSheet->setCurrentIndex(index);
}

void CharacterSheetWidget::SetupInventory()
{
    INF_CRE_ITEM creItems[INF_NUM_ITEMSLOTS];
    m_creature->GetItems(&creItems[0]);
    ui->widgetInventory->SetItems(&creItems[0]);
}

void CharacterSheetWidget::SetupProficiencies()
{
    // fill our widget with the global list
    ui->widgetProficiencies->FillList(_vlProfs);

    // setup our actual proficiencies for this character
    CProfDataPlist list;
    m_creature->GetProfs(list);
    ui->widgetProficiencies->SetProfs(list);

    // the SetProf call handles deleting the list
}

void CharacterSheetWidget::SetupSpells()
{
    int nSpellCount;
    SpellTab *pSpellTab;
    SPELLDATA *pSpells;

    for (int i=0; i<INF_CRE_SPELLTYPES; i++)
    {
        switch(i)
        {
        case INF_CRE_ST_INNATE :
            pSpellTab = ui->widgetInnate;
            break;
        case INF_CRE_ST_PRIEST :
            pSpellTab = ui->widgetPriest;
            break;
        case INF_CRE_ST_WIZARD :
            pSpellTab = ui->widgetWizard;
            break;
        default :
            pSpellTab = NULL;
            break;
        }

        if (pSpellTab)
        {
            nSpellCount = m_creature->GetSpellCount(i);
            if (!nSpellCount)
                pSpellTab->SetSpells(i, nSpellCount, NULL);
            else
            {
                pSpells = new SPELLDATA[nSpellCount];
                m_creature->GetSpells(i, pSpells);
                pSpellTab->SetSpells(i, nSpellCount, pSpells);

                delete pSpells;
                pSpells = NULL;
            }
        }
    }
}

void CharacterSheetWidget::SetupMemorization()
{
    int nMemInfoCount = m_creature->GetMemorizationInfoCount();
    MEMINFO *pMemInfo = NULL;
    if (nMemInfoCount)
    {
        pMemInfo = new MEMINFO[nMemInfoCount];
        m_creature->GetMemorizationInfo(pMemInfo);
    }

    ui->widgetMemorization->SetData(pMemInfo, nMemInfoCount);
    delete [] pMemInfo;
}

void CharacterSheetWidget::Update()
{
    // ABILITIES
    m_creature->SetStr(ui->lineStr->text().toInt());
    m_creature->SetStrBonus(ui->lineStrPlus->text().toInt());
    m_creature->SetDex(ui->lineDex->text().toInt());
    m_creature->SetInt(ui->lineInt->text().toInt());
    m_creature->SetWis(ui->lineWis->text().toInt());
    m_creature->SetCon(ui->lineCon->text().toInt());
    m_creature->SetCha(ui->lineCha->text().toInt());

    m_creature->SetAC(ui->lineBaseAC->text().toInt());
    m_creature->SetBaseHP(ui->lineBaseHP->text().toUShort());
    m_creature->SetCurrentHP(ui->lineCurrentHP->text().toUShort());
    m_creature->SetThac0(ui->lineThaco->text().toInt());
    m_creature->SetReputation(ui->lineReputation->text().toInt());
    m_creature->SetExp(ui->lineExperience->text().toUInt());
    m_creature->SetExpForKill(ui->lineExpForKill->text().toUInt());
    m_creature->SetGold(ui->lineGold->text().toUInt());
    m_creature->SetFirstClassLevel(ui->lineLevel1->text().toInt());
    m_creature->SetSecondClassLevel(ui->lineLevel2->text().toInt());
    m_creature->SetThirdClassLevel(ui->lineLevel3->text().toInt());
    m_creature->SetAttacks(ui->comboBoxAttacks->currentText().toInt());

    // CHARACTERISTICS
    m_creature->SetSpeed(ui->lineMoveSilently->text().toUInt());
    m_creature->SetGender(ui->comboBoxGender->itemData(ui->comboBoxGender->currentIndex()).toInt());
    m_creature->SetRace(ui->comboBoxRace->itemData(ui->comboBoxRace->currentIndex()).toInt());
    m_creature->SetAlignment(ui->comboBoxAlignment->itemData(ui->comboBoxAlignment->currentIndex()).toInt());
    m_creature->SetClass(ui->comboBoxClass->itemData(ui->comboBoxClass->currentIndex()).toInt());
    m_creature->SetKit(ui->comboBoxKit->itemData(ui->comboBoxKit->currentIndex()).toUInt());
    m_creature->SetEnemyAlly(ui->comboBoxEnemyAlly->itemData(ui->comboBoxEnemyAlly->currentIndex()).toInt());
    if (ui->comboBoxRacialEnemy->isEnabled())
        m_creature->SetRacialEnemy(ui->comboBoxRacialEnemy->itemData(ui->comboBoxRacialEnemy->currentIndex()).toInt());

    // SAVES
    m_creature->SetSaveBreath(ui->lineBreath->text().toInt());
    m_creature->SetSaveDeath(ui->lineDeath->text().toInt());
    m_creature->SetSavePoly(ui->linePetrification->text().toInt());
    m_creature->SetSaveWands(ui->lineWand->text().toInt());
    m_creature->SetSaveSpells(ui->lineSpells->text().toInt());

    // RESISTANCES
    m_creature->SetResAcid(ui->lineAcid->text().toInt());
    m_creature->SetResFire(ui->lineFire->text().toInt());
    m_creature->SetResCold(ui->lineCold->text().toInt());
    m_creature->SetResElectricity(ui->lineElectricity->text().toInt());
    m_creature->SetResSlashing(ui->lineSlashing->text().toInt());
    m_creature->SetResPiercing(ui->linePiercing->text().toInt());
    m_creature->SetResCrushing(ui->lineCrushing->text().toInt());
    m_creature->SetResMissile(ui->lineMissile->text().toInt());
    m_creature->SetResMagicFire(ui->lineMagicFire->text().toInt());
    m_creature->SetResMagicCold(ui->lineMagicCold->text().toInt());
    m_creature->SetResMagic(ui->lineMagic->text().toInt());

    // THIEVES
    m_creature->SetMoveSilently(ui->lineMoveSilently->text().toInt());
    m_creature->SetHideInShadows(ui->lineHideInShadows->text().toInt());
    m_creature->SetFindTraps(ui->lineFindTraps->text().toInt());
    m_creature->SetSetTraps(ui->lineSetTraps->text().toInt());
    m_creature->SetOpenLocks(ui->lineOpenLocks->text().toInt());
    m_creature->SetDetectIllusions(ui->lineDetectIllusions->text().toInt());
    m_creature->SetPickPockets(ui->linePickPockets->text().toInt());

    // PROFICIENCIES
    CProfDataPlist liProfs;
    ui->widgetProficiencies->GetProfs(liProfs);
    m_creature->SetProfs(liProfs);

    // INVENTORY
    INF_CRE_ITEM creItems[INF_NUM_ITEMSLOTS];
    ui->widgetInventory->GetItems(&creItems[0]);
    m_creature->SetItems(&creItems[0]);

    // MEMORIZATION
    int nMemInfoCount = m_creature->GetMemorizationInfoCount();
    MEMINFO *pMemInfo = NULL;
    if (nMemInfoCount)
    {
        pMemInfo = new MEMINFO[nMemInfoCount];

        ui->widgetMemorization->GetData(pMemInfo, nMemInfoCount);
        m_creature->SetMemorizationInfo(pMemInfo);

        delete [] pMemInfo;
    }

    // SPELLS
    int nSpellCount;
    SpellTab *pSpellTab;
    SPELLDATA *pSpells;

    for (int i=0; i<TABSPELL_MAXMEM; i++)
    {
        switch(i)
        {
        case INF_CRE_ST_INNATE :
            pSpellTab = ui->widgetInnate;
            break;
        case INF_CRE_ST_PRIEST :
            pSpellTab = ui->widgetPriest;
            break;
        case INF_CRE_ST_WIZARD :
            pSpellTab = ui->widgetWizard;
            break;
        default :
            pSpellTab = NULL;
            break;
        }

        if (pSpellTab)
        {
            nSpellCount = pSpellTab->GetSpellCount();
            if (!nSpellCount)
                m_creature->SetSpells(i, nSpellCount, NULL);
            else
            {
                pSpells = new SPELLDATA[nSpellCount];
                pSpellTab->GetSpells(pSpells);
                m_creature->SetSpells(i, nSpellCount, pSpells);

                delete pSpells;
                pSpells = NULL;
            }
        }
    }
}
