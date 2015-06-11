#include "ScoreCalculatorWindow.h"
#include "Translations.h"

/*****************************************************************************/
ScoreCalculatorWindow::ScoreCalculatorWindow(QWidget *parent) :
    QDialog(parent)
{
    ui.setupUi(this);

    // Fill the combo boxes
    ui.cmbContract->addItems(QStringList() << ContractToString(Contract::TAKE)
                             << ContractToString(Contract::GUARD)
                             << ContractToString(Contract::GUARD_WITHOUT)
                             << ContractToString(Contract::GUARD_AGAINST));
    ui.cmbHandleType->addItems(QStringList() << tr("No handle") << tr("Simple") << tr("Double") << tr("Triple"));
    ui.cmbLittleEndian->addItems(QStringList() << tr("No bonus") << tr("Attack") << tr("Defense"));

    connect (ui.btnClose, &QPushButton::clicked, this, &ScoreCalculatorWindow::slotClose);
    connect (ui.cbxSlam, &QCheckBox::toggled, this, &ScoreCalculatorWindow::slotToggled);
    connect (ui.spnOudlers, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &ScoreCalculatorWindow::slotValueChanged);
    connect (ui.spnPoints, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &ScoreCalculatorWindow::slotValueChanged);
    connect (ui.cmbContract, static_cast<void(QComboBox::*)(int)>(&QComboBox::activated), this, &ScoreCalculatorWindow::slotActivated);
    connect (ui.cmbHandleType, static_cast<void(QComboBox::*)(int)>(&QComboBox::activated), this, &ScoreCalculatorWindow::slotActivated);
    connect (ui.cmbLittleEndian, static_cast<void(QComboBox::*)(int)>(&QComboBox::activated), this, &ScoreCalculatorWindow::slotActivated);

    Update();
}
/*****************************************************************************/
void ScoreCalculatorWindow::slotClose()
{
    emit sigHide();
}
/*****************************************************************************/
void ScoreCalculatorWindow::Update()
{
    // Contract parameters
    mBid.contract = static_cast<std::uint32_t>(ui.cmbContract->currentIndex() + 1);
    mBid.slam = ui.cbxSlam->isChecked();

    // Bonuses
    mPoints.handlePoints = Tarot::GetHandlePoints(ui.cmbHandleType->currentIndex());
    mPoints.slamDone = ui.cbxSlameDone->isChecked();
    if (ui.cmbLittleEndian->currentIndex() == 0)
    {
        mPoints.littleEndianOwner = Team::NO_TEAM;
    }
    else if (ui.cmbLittleEndian->currentIndex() == 1)
    {
        mPoints.littleEndianOwner = Team::ATTACK;
    }
    else
    {
        mPoints.littleEndianOwner = Team::DEFENSE;
    }

    // Game result
    mPoints.pointsAttack = ui.spnPoints->value();
    mPoints.oudlers = ui.spnOudlers->value();

    // Show the formulat and final scoring
    QString formula = QString("(25 + %1 + %2) * %3 + %4 + %5")
            .arg(abs(mPoints.Difference())).arg(mPoints.GetLittleEndianPoints()).arg(Tarot::GetMultiplier(mBid.contract)).arg(mPoints.handlePoints).arg(mPoints.GetSlamPoints(mBid));
    ui.lblFormula->setText(formula);
    ui.lblPointsAttack->setText(QString().setNum(mPoints.GetPoints(Team::ATTACK, mBid)));
    ui.lblPointsDefense->setText(QString().setNum(mPoints.GetPoints(Team::DEFENSE, mBid)));
}
/*****************************************************************************/
void ScoreCalculatorWindow::slotActivated(int index)
{
    (void)index;
    Update();
}
/*****************************************************************************/
void ScoreCalculatorWindow::slotToggled(bool active)
{
    (void)active;
    Update();
}
/*****************************************************************************/
void ScoreCalculatorWindow::slotValueChanged(int value)
{
    (void)value;
    Update();
}
