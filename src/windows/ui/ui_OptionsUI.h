/********************************************************************************
** Form generated from reading ui file 'OptionsUI.ui'
**
** Created: Fri 29. Jun 23:14:34 2007
**      by: Qt User Interface Compiler version 4.3.0
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_OPTIONSUI_H
#define UI_OPTIONSUI_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QDialog>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QRadioButton>
#include <QtGui/QScrollBar>
#include <QtGui/QSlider>
#include <QtGui/QSpacerItem>
#include <QtGui/QSpinBox>
#include <QtGui/QTabWidget>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

class Ui_OptionsUI
{
public:
    QGridLayout *gridLayout;
    QTabWidget *tabWidget;
    QWidget *tab_2;
    QGridLayout *gridLayout1;
    QGroupBox *groupBox_6;
    QGridLayout *gridLayout2;
    QHBoxLayout *hboxLayout;
    QVBoxLayout *vboxLayout;
    QLabel *pixSud;
    QPushButton *btnPixSud;
    QSpacerItem *spacerItem;
    QGroupBox *groupBox_7;
    QGridLayout *gridLayout3;
    QVBoxLayout *vboxLayout1;
    QRadioButton *sexeM;
    QRadioButton *sexeF;
    QSpacerItem *spacerItem1;
    QHBoxLayout *hboxLayout1;
    QLabel *TextLabel2_3_2;
    QLineEdit *nomJoueurSud;
    QHBoxLayout *hboxLayout2;
    QLabel *TextLabel3_2_2;
    QLineEdit *citationSud;
    QSpacerItem *spacerItem2;
    QGroupBox *groupBox_10;
    QVBoxLayout *vboxLayout2;
    QCheckBox *afficheAvatars;
    QHBoxLayout *hboxLayout3;
    QLabel *TextLabel1_3;
    QSpinBox *portReseau;
    QHBoxLayout *hboxLayout4;
    QLabel *label_4;
    QComboBox *langList;
    QSpacerItem *spacerItem3;
    QGroupBox *groupBox_8;
    QGridLayout *gridLayout4;
    QVBoxLayout *vboxLayout3;
    QLabel *TextLabel1;
    QLabel *TextLabel2;
    QVBoxLayout *vboxLayout4;
    QSlider *slider1;
    QSlider *slider2;
    QVBoxLayout *vboxLayout5;
    QLabel *temps1;
    QLabel *temps2;
    QSpacerItem *spacerItem4;
    QGroupBox *groupBox_9;
    QVBoxLayout *vboxLayout6;
    QRadioButton *troisJoueurs;
    QRadioButton *quatreJoueurs;
    QRadioButton *cinqJoueurs;
    QSpacerItem *spacerItem5;
    QWidget *tab_3;
    QGridLayout *gridLayout5;
    QGroupBox *groupBox_2;
    QGridLayout *gridLayout6;
    QVBoxLayout *vboxLayout7;
    QLabel *pixEst;
    QPushButton *btnPixEst;
    QVBoxLayout *vboxLayout8;
    QLineEdit *nomJoueurEst;
    QComboBox *niveauEst;
    QVBoxLayout *vboxLayout9;
    QLabel *label_2;
    QLabel *label_3;
    QGroupBox *groupBox_3;
    QGridLayout *gridLayout7;
    QVBoxLayout *vboxLayout10;
    QLabel *label_2_2;
    QLabel *label_3_2;
    QVBoxLayout *vboxLayout11;
    QLabel *pixNord;
    QPushButton *btnPixNord;
    QVBoxLayout *vboxLayout12;
    QLineEdit *nomJoueurNord;
    QComboBox *niveauNord;
    QGroupBox *groupBox_4;
    QGridLayout *gridLayout8;
    QVBoxLayout *vboxLayout13;
    QLabel *label_2_3;
    QLabel *label_3_3;
    QVBoxLayout *vboxLayout14;
    QLineEdit *nomJoueurOuest;
    QComboBox *niveauOuest;
    QVBoxLayout *vboxLayout15;
    QLabel *pixOuest;
    QPushButton *btnPixOuest;
    QGroupBox *groupBox_5;
    QGridLayout *gridLayout9;
    QVBoxLayout *vboxLayout16;
    QLabel *label_2_3_2;
    QLabel *label_3_3_2;
    QVBoxLayout *vboxLayout17;
    QLabel *pixNordOuest;
    QPushButton *btnPixNordOuest;
    QVBoxLayout *vboxLayout18;
    QLineEdit *nomJoueurNordOuest;
    QComboBox *niveauNordOuest;
    QSpacerItem *spacerItem6;
    QWidget *tab_4;
    QGridLayout *gridLayout10;
    QGroupBox *groupBox;
    QGridLayout *gridLayout11;
    QLabel *imageTapis;
    QSpacerItem *spacerItem7;
    QScrollBar *scrollBarTapis;
    QSpacerItem *spacerItem8;
    QHBoxLayout *hboxLayout5;
    QPushButton *btnOk;
    QSpacerItem *spacerItem9;
    QPushButton *btnDefaut;
    QPushButton *btnAnnuler;

    void setupUi(QDialog *OptionsUI)
    {
    if (OptionsUI->objectName().isEmpty())
        OptionsUI->setObjectName(QString::fromUtf8("OptionsUI"));
    QSize size(548, 458);
    size = size.expandedTo(OptionsUI->minimumSizeHint());
    OptionsUI->resize(size);
    QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(OptionsUI->sizePolicy().hasHeightForWidth());
    OptionsUI->setSizePolicy(sizePolicy);
    OptionsUI->setSizeGripEnabled(true);
    gridLayout = new QGridLayout(OptionsUI);
    gridLayout->setSpacing(6);
    gridLayout->setMargin(11);
    gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
    gridLayout->setHorizontalSpacing(6);
    gridLayout->setVerticalSpacing(6);
    gridLayout->setContentsMargins(9, 9, 9, 9);
    tabWidget = new QTabWidget(OptionsUI);
    tabWidget->setObjectName(QString::fromUtf8("tabWidget"));
    tab_2 = new QWidget();
    tab_2->setObjectName(QString::fromUtf8("tab_2"));
    gridLayout1 = new QGridLayout(tab_2);
    gridLayout1->setSpacing(6);
    gridLayout1->setMargin(11);
    gridLayout1->setObjectName(QString::fromUtf8("gridLayout1"));
    gridLayout1->setHorizontalSpacing(6);
    gridLayout1->setVerticalSpacing(6);
    gridLayout1->setContentsMargins(9, 9, 9, 9);
    groupBox_6 = new QGroupBox(tab_2);
    groupBox_6->setObjectName(QString::fromUtf8("groupBox_6"));
    gridLayout2 = new QGridLayout(groupBox_6);
    gridLayout2->setSpacing(6);
    gridLayout2->setMargin(11);
    gridLayout2->setObjectName(QString::fromUtf8("gridLayout2"));
    gridLayout2->setHorizontalSpacing(6);
    gridLayout2->setVerticalSpacing(6);
    gridLayout2->setContentsMargins(9, 9, 9, 9);
    hboxLayout = new QHBoxLayout();
    hboxLayout->setSpacing(6);
    hboxLayout->setObjectName(QString::fromUtf8("hboxLayout"));
    hboxLayout->setContentsMargins(0, 0, 0, 0);
    vboxLayout = new QVBoxLayout();
    vboxLayout->setSpacing(6);
    vboxLayout->setObjectName(QString::fromUtf8("vboxLayout"));
    vboxLayout->setContentsMargins(0, 0, 0, 0);
    pixSud = new QLabel(groupBox_6);
    pixSud->setObjectName(QString::fromUtf8("pixSud"));
    pixSud->setMinimumSize(QSize(40, 40));
    pixSud->setMaximumSize(QSize(40, 40));
    pixSud->setFrameShape(QFrame::Box);
    pixSud->setFrameShadow(QFrame::Plain);
    pixSud->setScaledContents(true);
    pixSud->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);

    vboxLayout->addWidget(pixSud);

    btnPixSud = new QPushButton(groupBox_6);
    btnPixSud->setObjectName(QString::fromUtf8("btnPixSud"));
    btnPixSud->setMinimumSize(QSize(40, 0));
    btnPixSud->setIcon(QIcon(QString::fromUtf8(":/images/fileopen.png")));
    btnPixSud->setAutoDefault(false);

    vboxLayout->addWidget(btnPixSud);


    hboxLayout->addLayout(vboxLayout);

    spacerItem = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    hboxLayout->addItem(spacerItem);

    groupBox_7 = new QGroupBox(groupBox_6);
    groupBox_7->setObjectName(QString::fromUtf8("groupBox_7"));
    gridLayout3 = new QGridLayout(groupBox_7);
    gridLayout3->setSpacing(6);
    gridLayout3->setMargin(11);
    gridLayout3->setObjectName(QString::fromUtf8("gridLayout3"));
    gridLayout3->setHorizontalSpacing(6);
    gridLayout3->setVerticalSpacing(6);
    gridLayout3->setContentsMargins(9, 9, 9, 9);
    vboxLayout1 = new QVBoxLayout();
    vboxLayout1->setSpacing(6);
    vboxLayout1->setObjectName(QString::fromUtf8("vboxLayout1"));
    vboxLayout1->setContentsMargins(0, 0, 0, 0);
    sexeM = new QRadioButton(groupBox_7);
    sexeM->setObjectName(QString::fromUtf8("sexeM"));

    vboxLayout1->addWidget(sexeM);

    sexeF = new QRadioButton(groupBox_7);
    sexeF->setObjectName(QString::fromUtf8("sexeF"));

    vboxLayout1->addWidget(sexeF);


    gridLayout3->addLayout(vboxLayout1, 0, 0, 1, 1);


    hboxLayout->addWidget(groupBox_7);


    gridLayout2->addLayout(hboxLayout, 0, 0, 1, 2);

    spacerItem1 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    gridLayout2->addItem(spacerItem1, 1, 0, 1, 1);

    hboxLayout1 = new QHBoxLayout();
    hboxLayout1->setSpacing(6);
    hboxLayout1->setObjectName(QString::fromUtf8("hboxLayout1"));
    hboxLayout1->setContentsMargins(0, 0, 0, 0);
    TextLabel2_3_2 = new QLabel(groupBox_6);
    TextLabel2_3_2->setObjectName(QString::fromUtf8("TextLabel2_3_2"));
    TextLabel2_3_2->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);

    hboxLayout1->addWidget(TextLabel2_3_2);

    nomJoueurSud = new QLineEdit(groupBox_6);
    nomJoueurSud->setObjectName(QString::fromUtf8("nomJoueurSud"));
    nomJoueurSud->setEnabled(true);
    nomJoueurSud->setMaxLength(10);

    hboxLayout1->addWidget(nomJoueurSud);


    gridLayout2->addLayout(hboxLayout1, 2, 0, 1, 2);

    hboxLayout2 = new QHBoxLayout();
    hboxLayout2->setSpacing(6);
    hboxLayout2->setObjectName(QString::fromUtf8("hboxLayout2"));
    hboxLayout2->setContentsMargins(0, 0, 0, 0);
    TextLabel3_2_2 = new QLabel(groupBox_6);
    TextLabel3_2_2->setObjectName(QString::fromUtf8("TextLabel3_2_2"));

    hboxLayout2->addWidget(TextLabel3_2_2);

    citationSud = new QLineEdit(groupBox_6);
    citationSud->setObjectName(QString::fromUtf8("citationSud"));
    citationSud->setEnabled(true);
    citationSud->setMaxLength(100);

    hboxLayout2->addWidget(citationSud);


    gridLayout2->addLayout(hboxLayout2, 3, 0, 1, 2);

    spacerItem2 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

    gridLayout2->addItem(spacerItem2, 4, 1, 1, 1);


    gridLayout1->addWidget(groupBox_6, 0, 0, 1, 1);

    groupBox_10 = new QGroupBox(tab_2);
    groupBox_10->setObjectName(QString::fromUtf8("groupBox_10"));
    vboxLayout2 = new QVBoxLayout(groupBox_10);
    vboxLayout2->setSpacing(6);
    vboxLayout2->setMargin(11);
    vboxLayout2->setObjectName(QString::fromUtf8("vboxLayout2"));
    vboxLayout2->setContentsMargins(9, 9, 9, 9);
    afficheAvatars = new QCheckBox(groupBox_10);
    afficheAvatars->setObjectName(QString::fromUtf8("afficheAvatars"));

    vboxLayout2->addWidget(afficheAvatars);

    hboxLayout3 = new QHBoxLayout();
    hboxLayout3->setSpacing(6);
    hboxLayout3->setObjectName(QString::fromUtf8("hboxLayout3"));
    hboxLayout3->setContentsMargins(0, 0, 0, 0);
    TextLabel1_3 = new QLabel(groupBox_10);
    TextLabel1_3->setObjectName(QString::fromUtf8("TextLabel1_3"));

    hboxLayout3->addWidget(TextLabel1_3);

    portReseau = new QSpinBox(groupBox_10);
    portReseau->setObjectName(QString::fromUtf8("portReseau"));
    portReseau->setMaximum(999999);

    hboxLayout3->addWidget(portReseau);


    vboxLayout2->addLayout(hboxLayout3);

    hboxLayout4 = new QHBoxLayout();
    hboxLayout4->setSpacing(6);
    hboxLayout4->setObjectName(QString::fromUtf8("hboxLayout4"));
    hboxLayout4->setContentsMargins(0, 0, 0, 0);
    label_4 = new QLabel(groupBox_10);
    label_4->setObjectName(QString::fromUtf8("label_4"));
    QSizePolicy sizePolicy1(QSizePolicy::Minimum, QSizePolicy::Preferred);
    sizePolicy1.setHorizontalStretch(0);
    sizePolicy1.setVerticalStretch(0);
    sizePolicy1.setHeightForWidth(label_4->sizePolicy().hasHeightForWidth());
    label_4->setSizePolicy(sizePolicy1);

    hboxLayout4->addWidget(label_4);

    langList = new QComboBox(groupBox_10);
    langList->setObjectName(QString::fromUtf8("langList"));
    QSizePolicy sizePolicy2(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
    sizePolicy2.setHorizontalStretch(0);
    sizePolicy2.setVerticalStretch(0);
    sizePolicy2.setHeightForWidth(langList->sizePolicy().hasHeightForWidth());
    langList->setSizePolicy(sizePolicy2);
    langList->setIconSize(QSize(15, 15));

    hboxLayout4->addWidget(langList);


    vboxLayout2->addLayout(hboxLayout4);

    spacerItem3 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

    vboxLayout2->addItem(spacerItem3);


    gridLayout1->addWidget(groupBox_10, 0, 1, 1, 1);

    groupBox_8 = new QGroupBox(tab_2);
    groupBox_8->setObjectName(QString::fromUtf8("groupBox_8"));
    gridLayout4 = new QGridLayout(groupBox_8);
    gridLayout4->setSpacing(6);
    gridLayout4->setMargin(11);
    gridLayout4->setObjectName(QString::fromUtf8("gridLayout4"));
    gridLayout4->setHorizontalSpacing(6);
    gridLayout4->setVerticalSpacing(6);
    gridLayout4->setContentsMargins(9, 9, 9, 9);
    vboxLayout3 = new QVBoxLayout();
    vboxLayout3->setSpacing(6);
    vboxLayout3->setObjectName(QString::fromUtf8("vboxLayout3"));
    vboxLayout3->setContentsMargins(0, 0, 0, 0);
    TextLabel1 = new QLabel(groupBox_8);
    TextLabel1->setObjectName(QString::fromUtf8("TextLabel1"));

    vboxLayout3->addWidget(TextLabel1);

    TextLabel2 = new QLabel(groupBox_8);
    TextLabel2->setObjectName(QString::fromUtf8("TextLabel2"));

    vboxLayout3->addWidget(TextLabel2);


    gridLayout4->addLayout(vboxLayout3, 0, 0, 1, 1);

    vboxLayout4 = new QVBoxLayout();
    vboxLayout4->setSpacing(6);
    vboxLayout4->setObjectName(QString::fromUtf8("vboxLayout4"));
    vboxLayout4->setContentsMargins(0, 0, 0, 0);
    slider1 = new QSlider(groupBox_8);
    slider1->setObjectName(QString::fromUtf8("slider1"));
    slider1->setMaximum(9000);
    slider1->setSingleStep(100);
    slider1->setPageStep(100);
    slider1->setOrientation(Qt::Horizontal);

    vboxLayout4->addWidget(slider1);

    slider2 = new QSlider(groupBox_8);
    slider2->setObjectName(QString::fromUtf8("slider2"));
    slider2->setMaximum(9000);
    slider2->setSingleStep(100);
    slider2->setPageStep(100);
    slider2->setOrientation(Qt::Horizontal);

    vboxLayout4->addWidget(slider2);


    gridLayout4->addLayout(vboxLayout4, 0, 1, 1, 1);

    vboxLayout5 = new QVBoxLayout();
    vboxLayout5->setSpacing(6);
    vboxLayout5->setObjectName(QString::fromUtf8("vboxLayout5"));
    vboxLayout5->setContentsMargins(0, 0, 0, 0);
    temps1 = new QLabel(groupBox_8);
    temps1->setObjectName(QString::fromUtf8("temps1"));
    temps1->setScaledContents(false);
    temps1->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);

    vboxLayout5->addWidget(temps1);

    temps2 = new QLabel(groupBox_8);
    temps2->setObjectName(QString::fromUtf8("temps2"));
    temps2->setScaledContents(false);

    vboxLayout5->addWidget(temps2);


    gridLayout4->addLayout(vboxLayout5, 0, 2, 1, 1);

    spacerItem4 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

    gridLayout4->addItem(spacerItem4, 1, 1, 1, 1);


    gridLayout1->addWidget(groupBox_8, 1, 0, 1, 1);

    groupBox_9 = new QGroupBox(tab_2);
    groupBox_9->setObjectName(QString::fromUtf8("groupBox_9"));
    vboxLayout6 = new QVBoxLayout(groupBox_9);
    vboxLayout6->setSpacing(6);
    vboxLayout6->setMargin(11);
    vboxLayout6->setObjectName(QString::fromUtf8("vboxLayout6"));
    vboxLayout6->setContentsMargins(9, 9, 9, 9);
    troisJoueurs = new QRadioButton(groupBox_9);
    troisJoueurs->setObjectName(QString::fromUtf8("troisJoueurs"));
    troisJoueurs->setEnabled(false);

    vboxLayout6->addWidget(troisJoueurs);

    quatreJoueurs = new QRadioButton(groupBox_9);
    quatreJoueurs->setObjectName(QString::fromUtf8("quatreJoueurs"));

    vboxLayout6->addWidget(quatreJoueurs);

    cinqJoueurs = new QRadioButton(groupBox_9);
    cinqJoueurs->setObjectName(QString::fromUtf8("cinqJoueurs"));
    cinqJoueurs->setEnabled(false);

    vboxLayout6->addWidget(cinqJoueurs);

    spacerItem5 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

    vboxLayout6->addItem(spacerItem5);


    gridLayout1->addWidget(groupBox_9, 1, 1, 1, 1);

    tabWidget->addTab(tab_2, QString());
    tab_3 = new QWidget();
    tab_3->setObjectName(QString::fromUtf8("tab_3"));
    gridLayout5 = new QGridLayout(tab_3);
    gridLayout5->setSpacing(6);
    gridLayout5->setMargin(11);
    gridLayout5->setObjectName(QString::fromUtf8("gridLayout5"));
    gridLayout5->setHorizontalSpacing(6);
    gridLayout5->setVerticalSpacing(6);
    gridLayout5->setContentsMargins(9, 9, 9, 9);
    groupBox_2 = new QGroupBox(tab_3);
    groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
    gridLayout6 = new QGridLayout(groupBox_2);
    gridLayout6->setSpacing(6);
    gridLayout6->setMargin(11);
    gridLayout6->setObjectName(QString::fromUtf8("gridLayout6"));
    gridLayout6->setHorizontalSpacing(6);
    gridLayout6->setVerticalSpacing(6);
    gridLayout6->setContentsMargins(8, 8, 8, 8);
    vboxLayout7 = new QVBoxLayout();
    vboxLayout7->setSpacing(6);
    vboxLayout7->setObjectName(QString::fromUtf8("vboxLayout7"));
    vboxLayout7->setContentsMargins(0, 0, 0, 0);
    pixEst = new QLabel(groupBox_2);
    pixEst->setObjectName(QString::fromUtf8("pixEst"));
    pixEst->setMinimumSize(QSize(40, 40));
    pixEst->setMaximumSize(QSize(40, 40));
    pixEst->setFrameShape(QFrame::Box);
    pixEst->setFrameShadow(QFrame::Plain);
    pixEst->setScaledContents(true);

    vboxLayout7->addWidget(pixEst);

    btnPixEst = new QPushButton(groupBox_2);
    btnPixEst->setObjectName(QString::fromUtf8("btnPixEst"));
    btnPixEst->setMaximumSize(QSize(40, 16777215));
    btnPixEst->setIcon(QIcon(QString::fromUtf8(":/images/fileopen.png")));

    vboxLayout7->addWidget(btnPixEst);


    gridLayout6->addLayout(vboxLayout7, 0, 2, 1, 1);

    vboxLayout8 = new QVBoxLayout();
    vboxLayout8->setSpacing(6);
    vboxLayout8->setObjectName(QString::fromUtf8("vboxLayout8"));
    vboxLayout8->setContentsMargins(0, 0, 0, 0);
    nomJoueurEst = new QLineEdit(groupBox_2);
    nomJoueurEst->setObjectName(QString::fromUtf8("nomJoueurEst"));
    nomJoueurEst->setEnabled(true);
    nomJoueurEst->setMaxLength(10);

    vboxLayout8->addWidget(nomJoueurEst);

    niveauEst = new QComboBox(groupBox_2);
    niveauEst->setObjectName(QString::fromUtf8("niveauEst"));

    vboxLayout8->addWidget(niveauEst);


    gridLayout6->addLayout(vboxLayout8, 0, 1, 1, 1);

    vboxLayout9 = new QVBoxLayout();
    vboxLayout9->setSpacing(6);
    vboxLayout9->setObjectName(QString::fromUtf8("vboxLayout9"));
    vboxLayout9->setContentsMargins(0, 0, 0, 0);
    label_2 = new QLabel(groupBox_2);
    label_2->setObjectName(QString::fromUtf8("label_2"));

    vboxLayout9->addWidget(label_2);

    label_3 = new QLabel(groupBox_2);
    label_3->setObjectName(QString::fromUtf8("label_3"));

    vboxLayout9->addWidget(label_3);


    gridLayout6->addLayout(vboxLayout9, 0, 0, 1, 1);


    gridLayout5->addWidget(groupBox_2, 0, 0, 1, 1);

    groupBox_3 = new QGroupBox(tab_3);
    groupBox_3->setObjectName(QString::fromUtf8("groupBox_3"));
    gridLayout7 = new QGridLayout(groupBox_3);
    gridLayout7->setSpacing(6);
    gridLayout7->setMargin(11);
    gridLayout7->setObjectName(QString::fromUtf8("gridLayout7"));
    gridLayout7->setHorizontalSpacing(6);
    gridLayout7->setVerticalSpacing(6);
    gridLayout7->setContentsMargins(8, 8, 8, 8);
    vboxLayout10 = new QVBoxLayout();
    vboxLayout10->setSpacing(6);
    vboxLayout10->setObjectName(QString::fromUtf8("vboxLayout10"));
    vboxLayout10->setContentsMargins(0, 0, 0, 0);
    label_2_2 = new QLabel(groupBox_3);
    label_2_2->setObjectName(QString::fromUtf8("label_2_2"));

    vboxLayout10->addWidget(label_2_2);

    label_3_2 = new QLabel(groupBox_3);
    label_3_2->setObjectName(QString::fromUtf8("label_3_2"));

    vboxLayout10->addWidget(label_3_2);


    gridLayout7->addLayout(vboxLayout10, 0, 0, 1, 1);

    vboxLayout11 = new QVBoxLayout();
    vboxLayout11->setSpacing(6);
    vboxLayout11->setObjectName(QString::fromUtf8("vboxLayout11"));
    vboxLayout11->setContentsMargins(0, 0, 0, 0);
    pixNord = new QLabel(groupBox_3);
    pixNord->setObjectName(QString::fromUtf8("pixNord"));
    pixNord->setMinimumSize(QSize(40, 40));
    pixNord->setMaximumSize(QSize(40, 40));
    pixNord->setFrameShape(QFrame::Box);
    pixNord->setFrameShadow(QFrame::Plain);
    pixNord->setScaledContents(true);

    vboxLayout11->addWidget(pixNord);

    btnPixNord = new QPushButton(groupBox_3);
    btnPixNord->setObjectName(QString::fromUtf8("btnPixNord"));
    btnPixNord->setMaximumSize(QSize(40, 16777215));
    btnPixNord->setIcon(QIcon(QString::fromUtf8(":/images/fileopen.png")));

    vboxLayout11->addWidget(btnPixNord);


    gridLayout7->addLayout(vboxLayout11, 0, 2, 1, 1);

    vboxLayout12 = new QVBoxLayout();
    vboxLayout12->setSpacing(6);
    vboxLayout12->setObjectName(QString::fromUtf8("vboxLayout12"));
    vboxLayout12->setContentsMargins(0, 0, 0, 0);
    nomJoueurNord = new QLineEdit(groupBox_3);
    nomJoueurNord->setObjectName(QString::fromUtf8("nomJoueurNord"));
    nomJoueurNord->setEnabled(true);
    nomJoueurNord->setMaxLength(10);

    vboxLayout12->addWidget(nomJoueurNord);

    niveauNord = new QComboBox(groupBox_3);
    niveauNord->setObjectName(QString::fromUtf8("niveauNord"));

    vboxLayout12->addWidget(niveauNord);


    gridLayout7->addLayout(vboxLayout12, 0, 1, 1, 1);


    gridLayout5->addWidget(groupBox_3, 0, 1, 1, 1);

    groupBox_4 = new QGroupBox(tab_3);
    groupBox_4->setObjectName(QString::fromUtf8("groupBox_4"));
    gridLayout8 = new QGridLayout(groupBox_4);
    gridLayout8->setSpacing(6);
    gridLayout8->setMargin(11);
    gridLayout8->setObjectName(QString::fromUtf8("gridLayout8"));
    gridLayout8->setHorizontalSpacing(6);
    gridLayout8->setVerticalSpacing(6);
    gridLayout8->setContentsMargins(8, 8, 8, 8);
    vboxLayout13 = new QVBoxLayout();
    vboxLayout13->setSpacing(6);
    vboxLayout13->setObjectName(QString::fromUtf8("vboxLayout13"));
    vboxLayout13->setContentsMargins(0, 0, 0, 0);
    label_2_3 = new QLabel(groupBox_4);
    label_2_3->setObjectName(QString::fromUtf8("label_2_3"));

    vboxLayout13->addWidget(label_2_3);

    label_3_3 = new QLabel(groupBox_4);
    label_3_3->setObjectName(QString::fromUtf8("label_3_3"));

    vboxLayout13->addWidget(label_3_3);


    gridLayout8->addLayout(vboxLayout13, 0, 0, 1, 1);

    vboxLayout14 = new QVBoxLayout();
    vboxLayout14->setSpacing(6);
    vboxLayout14->setObjectName(QString::fromUtf8("vboxLayout14"));
    vboxLayout14->setContentsMargins(0, 0, 0, 0);
    nomJoueurOuest = new QLineEdit(groupBox_4);
    nomJoueurOuest->setObjectName(QString::fromUtf8("nomJoueurOuest"));
    nomJoueurOuest->setEnabled(true);
    nomJoueurOuest->setMaxLength(10);

    vboxLayout14->addWidget(nomJoueurOuest);

    niveauOuest = new QComboBox(groupBox_4);
    niveauOuest->setObjectName(QString::fromUtf8("niveauOuest"));

    vboxLayout14->addWidget(niveauOuest);


    gridLayout8->addLayout(vboxLayout14, 0, 1, 1, 1);

    vboxLayout15 = new QVBoxLayout();
    vboxLayout15->setSpacing(6);
    vboxLayout15->setObjectName(QString::fromUtf8("vboxLayout15"));
    vboxLayout15->setContentsMargins(0, 0, 0, 0);
    pixOuest = new QLabel(groupBox_4);
    pixOuest->setObjectName(QString::fromUtf8("pixOuest"));
    pixOuest->setMinimumSize(QSize(40, 40));
    pixOuest->setMaximumSize(QSize(40, 40));
    pixOuest->setFrameShape(QFrame::Box);
    pixOuest->setFrameShadow(QFrame::Plain);
    pixOuest->setScaledContents(true);

    vboxLayout15->addWidget(pixOuest);

    btnPixOuest = new QPushButton(groupBox_4);
    btnPixOuest->setObjectName(QString::fromUtf8("btnPixOuest"));
    btnPixOuest->setMaximumSize(QSize(40, 16777215));
    btnPixOuest->setIcon(QIcon(QString::fromUtf8(":/images/fileopen.png")));
    btnPixOuest->setAutoDefault(true);
    btnPixOuest->setDefault(false);

    vboxLayout15->addWidget(btnPixOuest);


    gridLayout8->addLayout(vboxLayout15, 0, 2, 1, 1);


    gridLayout5->addWidget(groupBox_4, 1, 0, 1, 1);

    groupBox_5 = new QGroupBox(tab_3);
    groupBox_5->setObjectName(QString::fromUtf8("groupBox_5"));
    gridLayout9 = new QGridLayout(groupBox_5);
    gridLayout9->setSpacing(6);
    gridLayout9->setMargin(11);
    gridLayout9->setObjectName(QString::fromUtf8("gridLayout9"));
    gridLayout9->setHorizontalSpacing(6);
    gridLayout9->setVerticalSpacing(6);
    gridLayout9->setContentsMargins(8, 8, 8, 8);
    vboxLayout16 = new QVBoxLayout();
    vboxLayout16->setSpacing(6);
    vboxLayout16->setObjectName(QString::fromUtf8("vboxLayout16"));
    vboxLayout16->setContentsMargins(0, 0, 0, 0);
    label_2_3_2 = new QLabel(groupBox_5);
    label_2_3_2->setObjectName(QString::fromUtf8("label_2_3_2"));

    vboxLayout16->addWidget(label_2_3_2);

    label_3_3_2 = new QLabel(groupBox_5);
    label_3_3_2->setObjectName(QString::fromUtf8("label_3_3_2"));

    vboxLayout16->addWidget(label_3_3_2);


    gridLayout9->addLayout(vboxLayout16, 0, 0, 1, 1);

    vboxLayout17 = new QVBoxLayout();
    vboxLayout17->setSpacing(6);
    vboxLayout17->setObjectName(QString::fromUtf8("vboxLayout17"));
    vboxLayout17->setContentsMargins(0, 0, 0, 0);
    pixNordOuest = new QLabel(groupBox_5);
    pixNordOuest->setObjectName(QString::fromUtf8("pixNordOuest"));
    pixNordOuest->setMinimumSize(QSize(40, 40));
    pixNordOuest->setMaximumSize(QSize(40, 40));
    pixNordOuest->setFrameShape(QFrame::Box);
    pixNordOuest->setFrameShadow(QFrame::Plain);
    pixNordOuest->setScaledContents(true);

    vboxLayout17->addWidget(pixNordOuest);

    btnPixNordOuest = new QPushButton(groupBox_5);
    btnPixNordOuest->setObjectName(QString::fromUtf8("btnPixNordOuest"));
    btnPixNordOuest->setMaximumSize(QSize(40, 16777215));
    btnPixNordOuest->setIcon(QIcon(QString::fromUtf8(":/images/fileopen.png")));
    btnPixNordOuest->setAutoDefault(true);
    btnPixNordOuest->setDefault(false);

    vboxLayout17->addWidget(btnPixNordOuest);


    gridLayout9->addLayout(vboxLayout17, 0, 2, 1, 1);

    vboxLayout18 = new QVBoxLayout();
    vboxLayout18->setSpacing(6);
    vboxLayout18->setObjectName(QString::fromUtf8("vboxLayout18"));
    vboxLayout18->setContentsMargins(0, 0, 0, 0);
    nomJoueurNordOuest = new QLineEdit(groupBox_5);
    nomJoueurNordOuest->setObjectName(QString::fromUtf8("nomJoueurNordOuest"));
    nomJoueurNordOuest->setEnabled(true);
    nomJoueurNordOuest->setMaxLength(10);

    vboxLayout18->addWidget(nomJoueurNordOuest);

    niveauNordOuest = new QComboBox(groupBox_5);
    niveauNordOuest->setObjectName(QString::fromUtf8("niveauNordOuest"));

    vboxLayout18->addWidget(niveauNordOuest);


    gridLayout9->addLayout(vboxLayout18, 0, 1, 1, 1);


    gridLayout5->addWidget(groupBox_5, 1, 1, 1, 1);

    spacerItem6 = new QSpacerItem(20, 71, QSizePolicy::Minimum, QSizePolicy::Expanding);

    gridLayout5->addItem(spacerItem6, 2, 1, 1, 1);

    tabWidget->addTab(tab_3, QString());
    tab_4 = new QWidget();
    tab_4->setObjectName(QString::fromUtf8("tab_4"));
    gridLayout10 = new QGridLayout(tab_4);
    gridLayout10->setSpacing(6);
    gridLayout10->setMargin(11);
    gridLayout10->setObjectName(QString::fromUtf8("gridLayout10"));
    gridLayout10->setHorizontalSpacing(6);
    gridLayout10->setVerticalSpacing(6);
    gridLayout10->setContentsMargins(9, 9, 9, 9);
    groupBox = new QGroupBox(tab_4);
    groupBox->setObjectName(QString::fromUtf8("groupBox"));
    gridLayout11 = new QGridLayout(groupBox);
    gridLayout11->setSpacing(6);
    gridLayout11->setMargin(11);
    gridLayout11->setObjectName(QString::fromUtf8("gridLayout11"));
    imageTapis = new QLabel(groupBox);
    imageTapis->setObjectName(QString::fromUtf8("imageTapis"));
    QSizePolicy sizePolicy3(QSizePolicy::Fixed, QSizePolicy::Fixed);
    sizePolicy3.setHorizontalStretch(0);
    sizePolicy3.setVerticalStretch(0);
    sizePolicy3.setHeightForWidth(imageTapis->sizePolicy().hasHeightForWidth());
    imageTapis->setSizePolicy(sizePolicy3);
    imageTapis->setMinimumSize(QSize(100, 100));
    imageTapis->setMaximumSize(QSize(100, 100));
    imageTapis->setFrameShape(QFrame::Box);

    gridLayout11->addWidget(imageTapis, 0, 0, 1, 1);

    spacerItem7 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    gridLayout11->addItem(spacerItem7, 0, 1, 1, 1);

    scrollBarTapis = new QScrollBar(groupBox);
    scrollBarTapis->setObjectName(QString::fromUtf8("scrollBarTapis"));
    scrollBarTapis->setOrientation(Qt::Horizontal);

    gridLayout11->addWidget(scrollBarTapis, 1, 0, 1, 1);

    spacerItem8 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

    gridLayout11->addItem(spacerItem8, 2, 0, 1, 1);


    gridLayout10->addWidget(groupBox, 0, 0, 1, 1);

    tabWidget->addTab(tab_4, QString());

    gridLayout->addWidget(tabWidget, 0, 0, 1, 1);

    hboxLayout5 = new QHBoxLayout();
    hboxLayout5->setSpacing(6);
    hboxLayout5->setObjectName(QString::fromUtf8("hboxLayout5"));
    hboxLayout5->setContentsMargins(0, 0, 0, 0);
    btnOk = new QPushButton(OptionsUI);
    btnOk->setObjectName(QString::fromUtf8("btnOk"));
    btnOk->setAutoDefault(true);
    btnOk->setDefault(true);

    hboxLayout5->addWidget(btnOk);

    spacerItem9 = new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    hboxLayout5->addItem(spacerItem9);

    btnDefaut = new QPushButton(OptionsUI);
    btnDefaut->setObjectName(QString::fromUtf8("btnDefaut"));

    hboxLayout5->addWidget(btnDefaut);

    btnAnnuler = new QPushButton(OptionsUI);
    btnAnnuler->setObjectName(QString::fromUtf8("btnAnnuler"));
    btnAnnuler->setAutoDefault(true);

    hboxLayout5->addWidget(btnAnnuler);


    gridLayout->addLayout(hboxLayout5, 1, 0, 1, 1);

    QWidget::setTabOrder(btnOk, btnDefaut);
    QWidget::setTabOrder(btnDefaut, btnAnnuler);

    retranslateUi(OptionsUI);
    QObject::connect(btnOk, SIGNAL(clicked()), OptionsUI, SLOT(accept()));
    QObject::connect(btnAnnuler, SIGNAL(clicked()), OptionsUI, SLOT(reject()));

    tabWidget->setCurrentIndex(2);


    QMetaObject::connectSlotsByName(OptionsUI);
    } // setupUi

    void retranslateUi(QDialog *OptionsUI)
    {
    OptionsUI->setWindowTitle(QApplication::translate("OptionsUI", "Options", 0, QApplication::UnicodeUTF8));
    groupBox_6->setTitle(QApplication::translate("OptionsUI", "Identit\303\251", 0, QApplication::UnicodeUTF8));
    pixSud->setText(QString());
    btnPixSud->setText(QString());
    groupBox_7->setTitle(QApplication::translate("OptionsUI", "Sexe", 0, QApplication::UnicodeUTF8));
    sexeM->setText(QApplication::translate("OptionsUI", "Masculin", 0, QApplication::UnicodeUTF8));
    sexeF->setText(QApplication::translate("OptionsUI", "F\303\251minin", 0, QApplication::UnicodeUTF8));
    TextLabel2_3_2->setText(QApplication::translate("OptionsUI", "Nom :", 0, QApplication::UnicodeUTF8));
    TextLabel3_2_2->setText(QApplication::translate("OptionsUI", "Citation :", 0, QApplication::UnicodeUTF8));
    groupBox_10->setTitle(QApplication::translate("OptionsUI", "Divers", 0, QApplication::UnicodeUTF8));
    afficheAvatars->setText(QApplication::translate("OptionsUI", "Afficher les avatars sur le tapis", 0, QApplication::UnicodeUTF8));
    TextLabel1_3->setText(QApplication::translate("OptionsUI", "Port TCP/IP :", 0, QApplication::UnicodeUTF8));
    label_4->setText(QApplication::translate("OptionsUI", "Langue", 0, QApplication::UnicodeUTF8));
    langList->clear();
    langList->insertItems(0, QStringList()
     << QApplication::translate("OptionsUI", "Fran\303\247ais", 0, QApplication::UnicodeUTF8)
     << QApplication::translate("OptionsUI", "English", 0, QApplication::UnicodeUTF8)
    );
    groupBox_8->setTitle(QApplication::translate("OptionsUI", "Temps des pauses", 0, QApplication::UnicodeUTF8));
    TextLabel1->setText(QApplication::translate("OptionsUI", "Entre deux joueurs", 0, QApplication::UnicodeUTF8));
    TextLabel2->setText(QApplication::translate("OptionsUI", "\303\200 la fin du tour", 0, QApplication::UnicodeUTF8));
    temps1->setText(QApplication::translate("OptionsUI", "0.0 secondes", 0, QApplication::UnicodeUTF8));
    temps2->setText(QApplication::translate("OptionsUI", "0.0 secondes", 0, QApplication::UnicodeUTF8));
    groupBox_9->setTitle(QApplication::translate("OptionsUI", "Nombre de joueurs", 0, QApplication::UnicodeUTF8));
    troisJoueurs->setText(QApplication::translate("OptionsUI", "3 joueurs", 0, QApplication::UnicodeUTF8));
    quatreJoueurs->setText(QApplication::translate("OptionsUI", "4 joueurs", 0, QApplication::UnicodeUTF8));
    cinqJoueurs->setText(QApplication::translate("OptionsUI", "5 joueurs", 0, QApplication::UnicodeUTF8));
    tabWidget->setTabText(tabWidget->indexOf(tab_2), QApplication::translate("OptionsUI", "Pr\303\251f\303\251rences", 0, QApplication::UnicodeUTF8));
    groupBox_2->setTitle(QApplication::translate("OptionsUI", "Joueur Est", 0, QApplication::UnicodeUTF8));
    btnPixEst->setText(QString());
    label_2->setText(QApplication::translate("OptionsUI", "Nom", 0, QApplication::UnicodeUTF8));
    label_3->setText(QApplication::translate("OptionsUI", "Niveau", 0, QApplication::UnicodeUTF8));
    groupBox_3->setTitle(QApplication::translate("OptionsUI", "Joueur Nord", 0, QApplication::UnicodeUTF8));
    label_2_2->setText(QApplication::translate("OptionsUI", "Nom", 0, QApplication::UnicodeUTF8));
    label_3_2->setText(QApplication::translate("OptionsUI", "Niveau", 0, QApplication::UnicodeUTF8));
    btnPixNord->setText(QString());
    groupBox_4->setTitle(QApplication::translate("OptionsUI", "Joueur Ouest", 0, QApplication::UnicodeUTF8));
    label_2_3->setText(QApplication::translate("OptionsUI", "Nom", 0, QApplication::UnicodeUTF8));
    label_3_3->setText(QApplication::translate("OptionsUI", "Niveau", 0, QApplication::UnicodeUTF8));
    btnPixOuest->setText(QString());
    groupBox_5->setTitle(QApplication::translate("OptionsUI", "Joueur Nord Ouest", 0, QApplication::UnicodeUTF8));
    label_2_3_2->setText(QApplication::translate("OptionsUI", "Nom", 0, QApplication::UnicodeUTF8));
    label_3_3_2->setText(QApplication::translate("OptionsUI", "Niveau", 0, QApplication::UnicodeUTF8));
    pixNordOuest->setText(QString());
    btnPixNordOuest->setText(QString());
    tabWidget->setTabText(tabWidget->indexOf(tab_3), QApplication::translate("OptionsUI", "Adversaires", 0, QApplication::UnicodeUTF8));
    groupBox->setTitle(QApplication::translate("OptionsUI", "Tapis de jeu", 0, QApplication::UnicodeUTF8));
    imageTapis->setText(QString());
    tabWidget->setTabText(tabWidget->indexOf(tab_4), QApplication::translate("OptionsUI", "Jacky touch", 0, QApplication::UnicodeUTF8));
    btnOk->setText(QApplication::translate("OptionsUI", "&Ok", 0, QApplication::UnicodeUTF8));
    btnOk->setShortcut(QApplication::translate("OptionsUI", "Alt+O", 0, QApplication::UnicodeUTF8));
    btnDefaut->setText(QApplication::translate("OptionsUI", "&D\303\251faut", 0, QApplication::UnicodeUTF8));
    btnAnnuler->setText(QApplication::translate("OptionsUI", "A&nnuler", 0, QApplication::UnicodeUTF8));
    Q_UNUSED(OptionsUI);
    } // retranslateUi

};

namespace Ui {
    class OptionsUI: public Ui_OptionsUI {};
} // namespace Ui

#endif // UI_OPTIONSUI_H
