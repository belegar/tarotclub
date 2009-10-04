/********************************************************************************
** Form generated from reading ui file 'ResultUI.ui'
**
** Created: Mon Oct 5 01:09:44 2009
**      by: Qt User Interface Compiler version 4.5.2
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_RESULTUI_H
#define UI_RESULTUI_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_ResultUI
{
public:
    QGridLayout *gridLayout;
    QLabel *lblResultat;
    QGroupBox *groupBox;
    QGridLayout *gridLayout1;
    QHBoxLayout *hboxLayout;
    QVBoxLayout *vboxLayout;
    QLabel *TextLabel1;
    QLabel *TextLabel2;
    QLabel *TextLabel3;
    QLabel *TextLabel5;
    QVBoxLayout *vboxLayout1;
    QLabel *lblPreneur;
    QLabel *lblContrat;
    QLabel *lblNbBouts;
    QLabel *lblPoints;
    QSpacerItem *spacerItem;
    QGroupBox *groupBox_2;
    QGridLayout *gridLayout2;
    QHBoxLayout *hboxLayout1;
    QVBoxLayout *vboxLayout2;
    QLabel *TextLabel1_2;
    QLabel *TextLabel2_2;
    QLabel *TextLabel4_2;
    QLabel *TextLabel3_2;
    QLabel *TextLabel5_2;
    QLabel *TextLabel6_2;
    QLabel *TextLabel6_2_2;
    QLabel *TextLabel6_2_2_2;
    QVBoxLayout *vboxLayout3;
    QLabel *lblPtContrat;
    QLabel *lblGains;
    QLabel *lblPetit;
    QLabel *lblMultiple;
    QLabel *lblPoignee;
    QLabel *lblChelem;
    QLabel *lblTotalDefense;
    QLabel *lblTotalAttaque;
    QSpacerItem *spacerItem1;
    QHBoxLayout *hboxLayout2;
    QSpacerItem *spacerItem2;
    QPushButton *continueButton;
    QPushButton *stopButton;

    void setupUi(QDialog *ResultUI)
    {
        if (ResultUI->objectName().isEmpty())
            ResultUI->setObjectName(QString::fromUtf8("ResultUI"));
        ResultUI->resize(520, 311);
        gridLayout = new QGridLayout(ResultUI);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        lblResultat = new QLabel(ResultUI);
        lblResultat->setObjectName(QString::fromUtf8("lblResultat"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(lblResultat->sizePolicy().hasHeightForWidth());
        lblResultat->setSizePolicy(sizePolicy);
        QFont font;
        font.setFamily(QString::fromUtf8("Verdana"));
        font.setPointSize(16);
        font.setBold(false);
        font.setItalic(false);
        font.setUnderline(false);
        font.setWeight(50);
        font.setStrikeOut(false);
        lblResultat->setFont(font);
        lblResultat->setFrameShape(QFrame::Box);
        lblResultat->setTextFormat(Qt::RichText);
        lblResultat->setScaledContents(false);
        lblResultat->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(lblResultat, 0, 0, 1, 2);

        groupBox = new QGroupBox(ResultUI);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        gridLayout1 = new QGridLayout(groupBox);
        gridLayout1->setObjectName(QString::fromUtf8("gridLayout1"));
        gridLayout1->setHorizontalSpacing(6);
        gridLayout1->setVerticalSpacing(6);
        gridLayout1->setContentsMargins(9, 9, 9, 9);
        hboxLayout = new QHBoxLayout();
        hboxLayout->setSpacing(6);
        hboxLayout->setObjectName(QString::fromUtf8("hboxLayout"));
        hboxLayout->setContentsMargins(0, 0, 0, 0);
        vboxLayout = new QVBoxLayout();
        vboxLayout->setSpacing(6);
        vboxLayout->setObjectName(QString::fromUtf8("vboxLayout"));
        vboxLayout->setContentsMargins(0, 0, 0, 0);
        TextLabel1 = new QLabel(groupBox);
        TextLabel1->setObjectName(QString::fromUtf8("TextLabel1"));

        vboxLayout->addWidget(TextLabel1);

        TextLabel2 = new QLabel(groupBox);
        TextLabel2->setObjectName(QString::fromUtf8("TextLabel2"));

        vboxLayout->addWidget(TextLabel2);

        TextLabel3 = new QLabel(groupBox);
        TextLabel3->setObjectName(QString::fromUtf8("TextLabel3"));

        vboxLayout->addWidget(TextLabel3);

        TextLabel5 = new QLabel(groupBox);
        TextLabel5->setObjectName(QString::fromUtf8("TextLabel5"));

        vboxLayout->addWidget(TextLabel5);


        hboxLayout->addLayout(vboxLayout);

        vboxLayout1 = new QVBoxLayout();
        vboxLayout1->setSpacing(6);
        vboxLayout1->setObjectName(QString::fromUtf8("vboxLayout1"));
        vboxLayout1->setContentsMargins(0, 0, 0, 0);
        lblPreneur = new QLabel(groupBox);
        lblPreneur->setObjectName(QString::fromUtf8("lblPreneur"));
        lblPreneur->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        vboxLayout1->addWidget(lblPreneur);

        lblContrat = new QLabel(groupBox);
        lblContrat->setObjectName(QString::fromUtf8("lblContrat"));
        lblContrat->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        vboxLayout1->addWidget(lblContrat);

        lblNbBouts = new QLabel(groupBox);
        lblNbBouts->setObjectName(QString::fromUtf8("lblNbBouts"));
        lblNbBouts->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        vboxLayout1->addWidget(lblNbBouts);

        lblPoints = new QLabel(groupBox);
        lblPoints->setObjectName(QString::fromUtf8("lblPoints"));
        lblPoints->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        vboxLayout1->addWidget(lblPoints);


        hboxLayout->addLayout(vboxLayout1);


        gridLayout1->addLayout(hboxLayout, 0, 0, 1, 1);

        spacerItem = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout1->addItem(spacerItem, 1, 0, 1, 1);


        gridLayout->addWidget(groupBox, 1, 0, 1, 1);

        groupBox_2 = new QGroupBox(ResultUI);
        groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
        gridLayout2 = new QGridLayout(groupBox_2);
        gridLayout2->setObjectName(QString::fromUtf8("gridLayout2"));
        gridLayout2->setHorizontalSpacing(6);
        gridLayout2->setVerticalSpacing(6);
        gridLayout2->setContentsMargins(9, 9, 9, 9);
        hboxLayout1 = new QHBoxLayout();
        hboxLayout1->setSpacing(6);
        hboxLayout1->setObjectName(QString::fromUtf8("hboxLayout1"));
        hboxLayout1->setContentsMargins(0, 0, 0, 0);
        vboxLayout2 = new QVBoxLayout();
        vboxLayout2->setSpacing(6);
        vboxLayout2->setObjectName(QString::fromUtf8("vboxLayout2"));
        vboxLayout2->setContentsMargins(0, 0, 0, 0);
        TextLabel1_2 = new QLabel(groupBox_2);
        TextLabel1_2->setObjectName(QString::fromUtf8("TextLabel1_2"));
        TextLabel1_2->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        vboxLayout2->addWidget(TextLabel1_2);

        TextLabel2_2 = new QLabel(groupBox_2);
        TextLabel2_2->setObjectName(QString::fromUtf8("TextLabel2_2"));
        TextLabel2_2->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        vboxLayout2->addWidget(TextLabel2_2);

        TextLabel4_2 = new QLabel(groupBox_2);
        TextLabel4_2->setObjectName(QString::fromUtf8("TextLabel4_2"));
        TextLabel4_2->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        vboxLayout2->addWidget(TextLabel4_2);

        TextLabel3_2 = new QLabel(groupBox_2);
        TextLabel3_2->setObjectName(QString::fromUtf8("TextLabel3_2"));
        TextLabel3_2->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        vboxLayout2->addWidget(TextLabel3_2);

        TextLabel5_2 = new QLabel(groupBox_2);
        TextLabel5_2->setObjectName(QString::fromUtf8("TextLabel5_2"));
        TextLabel5_2->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        vboxLayout2->addWidget(TextLabel5_2);

        TextLabel6_2 = new QLabel(groupBox_2);
        TextLabel6_2->setObjectName(QString::fromUtf8("TextLabel6_2"));
        TextLabel6_2->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        vboxLayout2->addWidget(TextLabel6_2);

        TextLabel6_2_2 = new QLabel(groupBox_2);
        TextLabel6_2_2->setObjectName(QString::fromUtf8("TextLabel6_2_2"));
        QFont font1;
        font1.setFamily(QString::fromUtf8("Verdana"));
        font1.setPointSize(14);
        font1.setBold(false);
        font1.setItalic(false);
        font1.setUnderline(false);
        font1.setWeight(50);
        font1.setStrikeOut(false);
        TextLabel6_2_2->setFont(font1);
        TextLabel6_2_2->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        vboxLayout2->addWidget(TextLabel6_2_2);

        TextLabel6_2_2_2 = new QLabel(groupBox_2);
        TextLabel6_2_2_2->setObjectName(QString::fromUtf8("TextLabel6_2_2_2"));
        TextLabel6_2_2_2->setFont(font1);
        TextLabel6_2_2_2->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        vboxLayout2->addWidget(TextLabel6_2_2_2);


        hboxLayout1->addLayout(vboxLayout2);

        vboxLayout3 = new QVBoxLayout();
        vboxLayout3->setSpacing(6);
        vboxLayout3->setObjectName(QString::fromUtf8("vboxLayout3"));
        vboxLayout3->setContentsMargins(0, 0, 0, 0);
        lblPtContrat = new QLabel(groupBox_2);
        lblPtContrat->setObjectName(QString::fromUtf8("lblPtContrat"));
        lblPtContrat->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        vboxLayout3->addWidget(lblPtContrat);

        lblGains = new QLabel(groupBox_2);
        lblGains->setObjectName(QString::fromUtf8("lblGains"));
        lblGains->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        vboxLayout3->addWidget(lblGains);

        lblPetit = new QLabel(groupBox_2);
        lblPetit->setObjectName(QString::fromUtf8("lblPetit"));
        lblPetit->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        vboxLayout3->addWidget(lblPetit);

        lblMultiple = new QLabel(groupBox_2);
        lblMultiple->setObjectName(QString::fromUtf8("lblMultiple"));
        lblMultiple->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        vboxLayout3->addWidget(lblMultiple);

        lblPoignee = new QLabel(groupBox_2);
        lblPoignee->setObjectName(QString::fromUtf8("lblPoignee"));
        lblPoignee->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        vboxLayout3->addWidget(lblPoignee);

        lblChelem = new QLabel(groupBox_2);
        lblChelem->setObjectName(QString::fromUtf8("lblChelem"));
        lblChelem->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        vboxLayout3->addWidget(lblChelem);

        lblTotalDefense = new QLabel(groupBox_2);
        lblTotalDefense->setObjectName(QString::fromUtf8("lblTotalDefense"));
        lblTotalDefense->setFont(font1);
        lblTotalDefense->setAlignment(Qt::AlignBottom|Qt::AlignRight|Qt::AlignTrailing);

        vboxLayout3->addWidget(lblTotalDefense);

        lblTotalAttaque = new QLabel(groupBox_2);
        lblTotalAttaque->setObjectName(QString::fromUtf8("lblTotalAttaque"));
        lblTotalAttaque->setFont(font1);
        lblTotalAttaque->setAlignment(Qt::AlignBottom|Qt::AlignRight|Qt::AlignTrailing);

        vboxLayout3->addWidget(lblTotalAttaque);


        hboxLayout1->addLayout(vboxLayout3);


        gridLayout2->addLayout(hboxLayout1, 0, 0, 1, 1);

        spacerItem1 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout2->addItem(spacerItem1, 1, 0, 1, 1);


        gridLayout->addWidget(groupBox_2, 1, 1, 1, 1);

        hboxLayout2 = new QHBoxLayout();
        hboxLayout2->setObjectName(QString::fromUtf8("hboxLayout2"));
        spacerItem2 = new QSpacerItem(271, 31, QSizePolicy::Expanding, QSizePolicy::Minimum);

        hboxLayout2->addItem(spacerItem2);

        continueButton = new QPushButton(ResultUI);
        continueButton->setObjectName(QString::fromUtf8("continueButton"));

        hboxLayout2->addWidget(continueButton);

        stopButton = new QPushButton(ResultUI);
        stopButton->setObjectName(QString::fromUtf8("stopButton"));

        hboxLayout2->addWidget(stopButton);


        gridLayout->addLayout(hboxLayout2, 2, 0, 1, 2);


        retranslateUi(ResultUI);
        QObject::connect(continueButton, SIGNAL(clicked()), ResultUI, SLOT(accept()));
        QObject::connect(stopButton, SIGNAL(clicked()), ResultUI, SLOT(reject()));

        QMetaObject::connectSlotsByName(ResultUI);
    } // setupUi

    void retranslateUi(QDialog *ResultUI)
    {
        ResultUI->setWindowTitle(QApplication::translate("ResultUI", "R\303\203\302\251sultat", 0, QApplication::UnicodeUTF8));
        lblResultat->setText(QApplication::translate("ResultUI", "Resultat", 0, QApplication::UnicodeUTF8));
        groupBox->setTitle(QApplication::translate("ResultUI", "R\303\203\302\251sum\303\203\302\251", 0, QApplication::UnicodeUTF8));
        TextLabel1->setText(QApplication::translate("ResultUI", "Preneur :", 0, QApplication::UnicodeUTF8));
        TextLabel2->setText(QApplication::translate("ResultUI", "Contrat :", 0, QApplication::UnicodeUTF8));
        TextLabel3->setText(QApplication::translate("ResultUI", "Nombre de bouts :", 0, QApplication::UnicodeUTF8));
        TextLabel5->setText(QApplication::translate("ResultUI", "Points r\303\203\302\251alis\303\203\302\251s :", 0, QApplication::UnicodeUTF8));
        lblPreneur->setText(QApplication::translate("ResultUI", "-", 0, QApplication::UnicodeUTF8));
        lblContrat->setText(QApplication::translate("ResultUI", "-", 0, QApplication::UnicodeUTF8));
        lblNbBouts->setText(QApplication::translate("ResultUI", "0 bouts", 0, QApplication::UnicodeUTF8));
        lblPoints->setText(QApplication::translate("ResultUI", "0 points", 0, QApplication::UnicodeUTF8));
        groupBox_2->setTitle(QApplication::translate("ResultUI", "Calcul", 0, QApplication::UnicodeUTF8));
        TextLabel1_2->setText(QApplication::translate("ResultUI", "Contrat :", 0, QApplication::UnicodeUTF8));
        TextLabel2_2->setText(QApplication::translate("ResultUI", "Gain / perte :", 0, QApplication::UnicodeUTF8));
        TextLabel4_2->setText(QApplication::translate("ResultUI", "Petit au bout :", 0, QApplication::UnicodeUTF8));
        TextLabel3_2->setText(QApplication::translate("ResultUI", "Multiplicateur contrat :", 0, QApplication::UnicodeUTF8));
        TextLabel5_2->setText(QApplication::translate("ResultUI", "Poign\303\203\302\251e :", 0, QApplication::UnicodeUTF8));
        TextLabel6_2->setText(QApplication::translate("ResultUI", "Chelem :", 0, QApplication::UnicodeUTF8));
        TextLabel6_2_2->setText(QApplication::translate("ResultUI", "Total d\303\203\302\251fense :", 0, QApplication::UnicodeUTF8));
        TextLabel6_2_2_2->setText(QApplication::translate("ResultUI", "Total attaque :", 0, QApplication::UnicodeUTF8));
        lblPtContrat->setText(QApplication::translate("ResultUI", "25", 0, QApplication::UnicodeUTF8));
        lblGains->setText(QApplication::translate("ResultUI", "0", 0, QApplication::UnicodeUTF8));
        lblPetit->setText(QApplication::translate("ResultUI", "0", 0, QApplication::UnicodeUTF8));
        lblMultiple->setText(QApplication::translate("ResultUI", "1", 0, QApplication::UnicodeUTF8));
        lblPoignee->setText(QApplication::translate("ResultUI", "1", 0, QApplication::UnicodeUTF8));
        lblChelem->setText(QApplication::translate("ResultUI", "1", 0, QApplication::UnicodeUTF8));
        lblTotalDefense->setText(QApplication::translate("ResultUI", "0 points", 0, QApplication::UnicodeUTF8));
        lblTotalAttaque->setText(QApplication::translate("ResultUI", "0 points", 0, QApplication::UnicodeUTF8));
        continueButton->setText(QApplication::translate("ResultUI", "Continuer", 0, QApplication::UnicodeUTF8));
        stopButton->setText(QApplication::translate("ResultUI", "Arr\303\203\302\252ter", 0, QApplication::UnicodeUTF8));
        Q_UNUSED(ResultUI);
    } // retranslateUi

};

namespace Ui {
    class ResultUI: public Ui_ResultUI {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_RESULTUI_H
