#ifndef JOINWIZARD_H
#define JOINWIZARD_H

#include <QWizard>
#include <ui_JoinWizardPage1.h>
#include <ui_JoinWizardPage2.h>

class JoinWizardPage1 : public QWizardPage
{
private:
   Ui::Page1 ui;
public:
   JoinWizardPage1(QWidget * parent = 0);

   void initializePage();

};


/*****************************************************************************/
class JoinWizard : public QWizard
{
   Q_OBJECT

private:

   enum { Page_Server, Page_Lobby };

   JoinWizardPage1 *page1;

   Ui::Page2 uiPage2;
   QWizardPage *page2;

public:
   JoinWizard(QWidget *parent);

};


#endif // JOINWIZARD_H
