#ifndef SCORE_CALCULATOR_WINDOW_H
#define SCORE_CALCULATOR_WINDOW_H

#include <QDialog>
#include "ui_ScoreCalculatorUI.h"
#include "Score.h"

class ScoreCalculatorWindow : public QDialog
{
    Q_OBJECT

public:
    ScoreCalculatorWindow(QWidget *parent = 0);

signals:
    void sigHide();

private slots:
    void slotClose();
    void slotValueChanged(int value);
    void slotToggled(bool active);
    void slotActivated(int index);

private:
    Ui::scoreCalculatorWindow ui;
    Points mPoints;
    Tarot::Bid mBid;

    void Update();
};

#endif // SCORE_CALCULATOR_WINDOW_H
