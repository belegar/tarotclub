/********************************************************************************
** Form generated from reading ui file 'AvatarsUI.ui'
**
** Created: Mon Oct 5 01:09:44 2009
**      by: Qt User Interface Compiler version 4.5.2
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_AVATARSUI_H
#define UI_AVATARSUI_H

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
#include <QtGui/QListWidget>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_Avatars
{
public:
    QGridLayout *gridLayout;
    QGroupBox *groupBox;
    QGridLayout *gridLayout1;
    QListWidget *listWidget;
    QVBoxLayout *vboxLayout;
    QHBoxLayout *hboxLayout;
    QSpacerItem *spacerItem;
    QLabel *avatarImage;
    QSpacerItem *spacerItem1;
    QHBoxLayout *hboxLayout1;
    QPushButton *okButton;
    QPushButton *cancelButton;

    void setupUi(QDialog *Avatars)
    {
        if (Avatars->objectName().isEmpty())
            Avatars->setObjectName(QString::fromUtf8("Avatars"));
        Avatars->resize(231, 363);
        gridLayout = new QGridLayout(Avatars);
#ifndef Q_OS_MAC
        gridLayout->setSpacing(6);
#endif
        gridLayout->setMargin(8);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        groupBox = new QGroupBox(Avatars);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        gridLayout1 = new QGridLayout(groupBox);
#ifndef Q_OS_MAC
        gridLayout1->setSpacing(6);
#endif
        gridLayout1->setMargin(8);
        gridLayout1->setObjectName(QString::fromUtf8("gridLayout1"));
        listWidget = new QListWidget(groupBox);
        listWidget->setObjectName(QString::fromUtf8("listWidget"));

        gridLayout1->addWidget(listWidget, 0, 0, 1, 1);

        vboxLayout = new QVBoxLayout();
#ifndef Q_OS_MAC
        vboxLayout->setSpacing(6);
#endif
        vboxLayout->setMargin(0);
        vboxLayout->setObjectName(QString::fromUtf8("vboxLayout"));
        hboxLayout = new QHBoxLayout();
#ifndef Q_OS_MAC
        hboxLayout->setSpacing(6);
#endif
#ifndef Q_OS_MAC
        hboxLayout->setMargin(0);
#endif
        hboxLayout->setObjectName(QString::fromUtf8("hboxLayout"));
        spacerItem = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        hboxLayout->addItem(spacerItem);

        avatarImage = new QLabel(groupBox);
        avatarImage->setObjectName(QString::fromUtf8("avatarImage"));
        QSizePolicy sizePolicy(static_cast<QSizePolicy::Policy>(0), static_cast<QSizePolicy::Policy>(0));
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(avatarImage->sizePolicy().hasHeightForWidth());
        avatarImage->setSizePolicy(sizePolicy);
        avatarImage->setMinimumSize(QSize(40, 40));
        avatarImage->setMaximumSize(QSize(40, 40));
        avatarImage->setFrameShape(QFrame::Box);

        hboxLayout->addWidget(avatarImage);

        spacerItem1 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        hboxLayout->addItem(spacerItem1);


        vboxLayout->addLayout(hboxLayout);


        gridLayout1->addLayout(vboxLayout, 1, 0, 1, 1);


        gridLayout->addWidget(groupBox, 0, 0, 1, 1);

        hboxLayout1 = new QHBoxLayout();
#ifndef Q_OS_MAC
        hboxLayout1->setSpacing(6);
#endif
        hboxLayout1->setMargin(0);
        hboxLayout1->setObjectName(QString::fromUtf8("hboxLayout1"));
        okButton = new QPushButton(Avatars);
        okButton->setObjectName(QString::fromUtf8("okButton"));

        hboxLayout1->addWidget(okButton);

        cancelButton = new QPushButton(Avatars);
        cancelButton->setObjectName(QString::fromUtf8("cancelButton"));

        hboxLayout1->addWidget(cancelButton);


        gridLayout->addLayout(hboxLayout1, 1, 0, 1, 1);


        retranslateUi(Avatars);
        QObject::connect(okButton, SIGNAL(clicked()), Avatars, SLOT(accept()));
        QObject::connect(cancelButton, SIGNAL(clicked()), Avatars, SLOT(reject()));

        QMetaObject::connectSlotsByName(Avatars);
    } // setupUi

    void retranslateUi(QDialog *Avatars)
    {
        Avatars->setWindowTitle(QApplication::translate("Avatars", "Avatars", 0, QApplication::UnicodeUTF8));
        groupBox->setTitle(QString());
        avatarImage->setText(QString());
        okButton->setText(QApplication::translate("Avatars", "&OK", 0, QApplication::UnicodeUTF8));
        cancelButton->setText(QApplication::translate("Avatars", "&Annuler", 0, QApplication::UnicodeUTF8));
        Q_UNUSED(Avatars);
    } // retranslateUi

};

namespace Ui {
    class Avatars: public Ui_Avatars {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_AVATARSUI_H
