#ifndef DEALSWINDOW_H
#define DEALSWINDOW_H

#include <QDialog>
#include <QPushButton>
#include <QComboBox>
#include <QLayout>
#include <QtWebKitWidgets/QWebView>

class DealsWindow : public QDialog
{
    Q_OBJECT

public:
    explicit DealsWindow(QWidget *parent = 0);
    ~DealsWindow();

private:
    QVBoxLayout *mLayout;
    QComboBox *mComboBox;
    QPushButton *mOkButton;
    QWebView *mWebView;
};

#endif // DEALSWINDOW_H
