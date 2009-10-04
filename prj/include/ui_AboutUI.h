/********************************************************************************
** Form generated from reading ui file 'AboutUI.ui'
**
** Created: Mon Oct 5 01:09:44 2009
**      by: Qt User Interface Compiler version 4.5.2
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_ABOUTUI_H
#define UI_ABOUTUI_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_AboutUI
{
public:
    QGridLayout *gridLayout;
    QGroupBox *groupBox;
    QGridLayout *gridLayout1;
    QLabel *TextLabel1;
    QVBoxLayout *vboxLayout;
    QLabel *label;
    QGridLayout *gridLayout2;
    QLabel *nom;
    QLabel *version;
    QLabel *TextLabel4;
    QPushButton *btnOk;
    QWidget *widget;

    void setupUi(QDialog *AboutUI)
    {
        if (AboutUI->objectName().isEmpty())
            AboutUI->setObjectName(QString::fromUtf8("AboutUI"));
        AboutUI->resize(583, 311);
        QSizePolicy sizePolicy(static_cast<QSizePolicy::Policy>(0), static_cast<QSizePolicy::Policy>(0));
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(AboutUI->sizePolicy().hasHeightForWidth());
        AboutUI->setSizePolicy(sizePolicy);
        gridLayout = new QGridLayout(AboutUI);
        gridLayout->setSpacing(6);
        gridLayout->setMargin(9);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        groupBox = new QGroupBox(AboutUI);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        sizePolicy.setHeightForWidth(groupBox->sizePolicy().hasHeightForWidth());
        groupBox->setSizePolicy(sizePolicy);
        gridLayout1 = new QGridLayout(groupBox);
        gridLayout1->setSpacing(6);
        gridLayout1->setMargin(9);
        gridLayout1->setObjectName(QString::fromUtf8("gridLayout1"));
        TextLabel1 = new QLabel(groupBox);
        TextLabel1->setObjectName(QString::fromUtf8("TextLabel1"));

        gridLayout1->addWidget(TextLabel1, 0, 0, 1, 1);


        gridLayout->addWidget(groupBox, 0, 0, 1, 1);

        vboxLayout = new QVBoxLayout();
        vboxLayout->setSpacing(6);
        vboxLayout->setMargin(0);
        vboxLayout->setObjectName(QString::fromUtf8("vboxLayout"));
        label = new QLabel(AboutUI);
        label->setObjectName(QString::fromUtf8("label"));
        sizePolicy.setHeightForWidth(label->sizePolicy().hasHeightForWidth());
        label->setSizePolicy(sizePolicy);
        label->setMinimumSize(QSize(16, 16));
        label->setMaximumSize(QSize(151, 213));
        label->setPixmap(QPixmap(QString::fromUtf8(":/images/dragonball.png")));
        label->setScaledContents(true);

        vboxLayout->addWidget(label);

        gridLayout2 = new QGridLayout();
        gridLayout2->setSpacing(6);
        gridLayout2->setMargin(0);
        gridLayout2->setObjectName(QString::fromUtf8("gridLayout2"));
        nom = new QLabel(AboutUI);
        nom->setObjectName(QString::fromUtf8("nom"));

        gridLayout2->addWidget(nom, 1, 0, 1, 1);

        version = new QLabel(AboutUI);
        version->setObjectName(QString::fromUtf8("version"));
        version->setScaledContents(false);

        gridLayout2->addWidget(version, 0, 0, 1, 1);

        TextLabel4 = new QLabel(AboutUI);
        TextLabel4->setObjectName(QString::fromUtf8("TextLabel4"));
        TextLabel4->setCursor(QCursor(static_cast<Qt::CursorShape>(13)));
        TextLabel4->setTextFormat(Qt::RichText);

        gridLayout2->addWidget(TextLabel4, 2, 0, 1, 1);


        vboxLayout->addLayout(gridLayout2);

        btnOk = new QPushButton(AboutUI);
        btnOk->setObjectName(QString::fromUtf8("btnOk"));

        vboxLayout->addWidget(btnOk);


        gridLayout->addLayout(vboxLayout, 0, 1, 1, 1);

        widget = new QWidget(AboutUI);
        widget->setObjectName(QString::fromUtf8("widget"));

        gridLayout->addWidget(widget, 0, 2, 1, 1);


        retranslateUi(AboutUI);
        QObject::connect(btnOk, SIGNAL(clicked()), AboutUI, SLOT(accept()));

        QMetaObject::connectSlotsByName(AboutUI);
    } // setupUi

    void retranslateUi(QDialog *AboutUI)
    {
        AboutUI->setWindowTitle(QApplication::translate("AboutUI", "\303\203\302\200 propos...", 0, QApplication::UnicodeUTF8));
        TextLabel1->setText(QApplication::translate("AboutUI", "<html><head><meta name=\"qrichtext\" content=\"1\" /></head><body style=\" white-space: pre-wrap; font-family:MS Shell Dlg; font-weight:400; font-style:normal; text-decoration:none;\"><p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" color:#000000;\">Tarot Club est un jeu de Tarot fran\303\203\302\247ais</span></p><p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; color:#000000;\">Copyright (C) 2003-2005  Anthony Rabine</p><p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; color:#000000;\">anthony@ooso.org</p><p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; color:#000000;\">http://tarotclub.ooso.org</p><p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent"
                        ":0; text-indent:0px; color:#000000;\"></p><p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; color:#000000;\">This program is free software; you can redistribute it and/or</p><p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; color:#000000;\">modify it under the terms of the GNU General Public License</p><p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; color:#000000;\">as published by the Free Software Foundation; either version 2</p><p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; color:#000000;\">of the License, or any later version.</p><p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; color:#000000;\"></p><p style=\" margin-top:0px; margin-bott"
                        "om:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; color:#000000;\">This program is distributed in the hope that it will be useful,</p><p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; color:#000000;\">but WITHOUT ANY WARRANTY; without even the implied warranty of</p><p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; color:#000000;\">MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the</p><p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; color:#000000;\">GNU General Public License for more details.</p><p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; color:#000000;\"></p><p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-in"
                        "dent:0px; color:#000000;\">You should have received a copy of the GNU General Public License</p><p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; color:#000000;\">along with this program; if not, write to the Free Software</p><p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; color:#000000;\">Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA</p></body></html>", 0, QApplication::UnicodeUTF8));
        label->setText(QString());
        nom->setText(QApplication::translate("AboutUI", "<Nom>", 0, QApplication::UnicodeUTF8));
        version->setText(QApplication::translate("AboutUI", "Tarot Club 0.0.0", 0, QApplication::UnicodeUTF8));
        TextLabel4->setText(QApplication::translate("AboutUI", "<a href=\"http://tarotclub.ooso.org\">http://tarotclub.ooso.org</a><br>\n"
"\n"
"\n"
"\n"
"<a href=\"mailto:anthony@ooso.org\">anthony@ooso.org</a>", 0, QApplication::UnicodeUTF8));
        btnOk->setText(QApplication::translate("AboutUI", "&Ok", 0, QApplication::UnicodeUTF8));
        btnOk->setShortcut(QApplication::translate("AboutUI", "Alt+O", 0, QApplication::UnicodeUTF8));
        Q_UNUSED(AboutUI);
    } // retranslateUi

};

namespace Ui {
    class AboutUI: public Ui_AboutUI {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ABOUTUI_H
