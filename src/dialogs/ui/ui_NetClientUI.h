/********************************************************************************
** Form generated from reading ui file 'NetClientUI.ui'
**
** Created: Fri 29. Jun 23:14:34 2007
**      by: Qt User Interface Compiler version 4.3.0
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_NETCLIENTUI_H
#define UI_NETCLIENTUI_H

#include <Qt3Support/Q3MimeSourceFactory>
#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QFrame>
#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QVBoxLayout>

class Ui_NetClientUI
{
public:
    QVBoxLayout *vboxLayout;
    QVBoxLayout *vboxLayout1;
    QLabel *label_2;
    QLabel *label_3;
    QFrame *line;
    QLabel *TextLabel2_2_2_2;
    QLineEdit *ipAddress;
    QSpacerItem *spacerItem;
    QFrame *line_2;
    QHBoxLayout *hboxLayout;
    QSpacerItem *spacerItem1;
    QPushButton *pushButton;

    void setupUi(QDialog *NetClientUI)
    {
    if (NetClientUI->objectName().isEmpty())
        NetClientUI->setObjectName(QString::fromUtf8("NetClientUI"));
    QSize size(301, 192);
    size = size.expandedTo(NetClientUI->minimumSizeHint());
    NetClientUI->resize(size);
    NetClientUI->setSizeGripEnabled(false);
    NetClientUI->setModal(true);
    vboxLayout = new QVBoxLayout(NetClientUI);
    vboxLayout->setSpacing(6);
    vboxLayout->setMargin(9);
    vboxLayout->setObjectName(QString::fromUtf8("vboxLayout"));
    vboxLayout1 = new QVBoxLayout();
    vboxLayout1->setSpacing(6);
    vboxLayout1->setMargin(0);
    vboxLayout1->setObjectName(QString::fromUtf8("vboxLayout1"));
    label_2 = new QLabel(NetClientUI);
    label_2->setObjectName(QString::fromUtf8("label_2"));

    vboxLayout1->addWidget(label_2);

    label_3 = new QLabel(NetClientUI);
    label_3->setObjectName(QString::fromUtf8("label_3"));
    label_3->setTextFormat(Qt::AutoText);

    vboxLayout1->addWidget(label_3);


    vboxLayout->addLayout(vboxLayout1);

    line = new QFrame(NetClientUI);
    line->setObjectName(QString::fromUtf8("line"));
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);

    vboxLayout->addWidget(line);

    TextLabel2_2_2_2 = new QLabel(NetClientUI);
    TextLabel2_2_2_2->setObjectName(QString::fromUtf8("TextLabel2_2_2_2"));
    QSizePolicy sizePolicy(static_cast<QSizePolicy::Policy>(1), static_cast<QSizePolicy::Policy>(1));
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(TextLabel2_2_2_2->sizePolicy().hasHeightForWidth());
    TextLabel2_2_2_2->setSizePolicy(sizePolicy);

    vboxLayout->addWidget(TextLabel2_2_2_2);

    ipAddress = new QLineEdit(NetClientUI);
    ipAddress->setObjectName(QString::fromUtf8("ipAddress"));
    ipAddress->setCursor(QCursor(static_cast<Qt::CursorShape>(4)));
    ipAddress->setMaxLength(15);

    vboxLayout->addWidget(ipAddress);

    spacerItem = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

    vboxLayout->addItem(spacerItem);

    line_2 = new QFrame(NetClientUI);
    line_2->setObjectName(QString::fromUtf8("line_2"));
    line_2->setFrameShape(QFrame::HLine);
    line_2->setFrameShadow(QFrame::Sunken);

    vboxLayout->addWidget(line_2);

    hboxLayout = new QHBoxLayout();
    hboxLayout->setSpacing(6);
    hboxLayout->setMargin(0);
    hboxLayout->setObjectName(QString::fromUtf8("hboxLayout"));
    spacerItem1 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    hboxLayout->addItem(spacerItem1);

    pushButton = new QPushButton(NetClientUI);
    pushButton->setObjectName(QString::fromUtf8("pushButton"));

    hboxLayout->addWidget(pushButton);


    vboxLayout->addLayout(hboxLayout);


    retranslateUi(NetClientUI);

    QMetaObject::connectSlotsByName(NetClientUI);
    } // setupUi

    void retranslateUi(QDialog *NetClientUI)
    {
    NetClientUI->setWindowTitle(QApplication::translate("NetClientUI", "Assistant connexion", 0, QApplication::UnicodeUTF8));
    label_2->setText(QApplication::translate("NetClientUI", "<html><head><meta name=\"qrichtext\" content=\"1\" /></head><body style=\" white-space: pre-wrap; font-family:MS Shell Dlg; font-weight:400; font-style:normal; text-decoration:none;\"><p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-weight:600; color:#000000;\">Connexion au serveur Tarot Club</span></p></body></html>", 0, QApplication::UnicodeUTF8));
    label_3->setText(QApplication::translate("NetClientUI", "<html><head><meta name=\"qrichtext\" content=\"1\" /></head><body style=\" white-space: pre-wrap; font-family:MS Shell Dlg; font-weight:400; font-style:normal; text-decoration:none;\"><p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" color:#000000;\">Vous devez conna\303\256tre l'adresse IP du joueur serveur</span></p></body></html>", 0, QApplication::UnicodeUTF8));
    TextLabel2_2_2_2->setText(QApplication::translate("NetClientUI", "Adresse IP du serveur :", 0, QApplication::UnicodeUTF8));
    ipAddress->setInputMask(QApplication::translate("NetClientUI", "000.000.000.000; ", 0, QApplication::UnicodeUTF8));
    ipAddress->setText(QApplication::translate("NetClientUI", "...", 0, QApplication::UnicodeUTF8));
    pushButton->setText(QApplication::translate("NetClientUI", "PushButton", 0, QApplication::UnicodeUTF8));
    Q_UNUSED(NetClientUI);
    } // retranslateUi

};

namespace Ui {
    class NetClientUI: public Ui_NetClientUI {};
} // namespace Ui

#endif // UI_NETCLIENTUI_H
