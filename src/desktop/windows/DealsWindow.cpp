#include "DealsWindow.h"


DealsWindow::DealsWindow(QWidget *parent)
    : QDialog(parent)
{
    QWebView *mWebView = new QWebView(this);
    QComboBox *mComboBox = new QComboBox(this);
    QPushButton *mOkButton = new QPushButton(this);

    mLayout = new QVBoxLayout(this);
    mLayout->addWidget(mComboBox);
    mLayout->addWidget(mWebView);
    mLayout->addWidget(mOkButton);

}

DealsWindow::~DealsWindow()
{

}
