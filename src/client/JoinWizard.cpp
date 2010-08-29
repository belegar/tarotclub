
#include "JoinWizard.h"
#include "ServerConfig.h"

/*****************************************************************************/
JoinWizardPage1::JoinWizardPage1(QWidget *parent) : QWizardPage(parent)
{
   ui.setupUi(this);
   setTitle(tr("Connexion à un serveur TarotClub"));
   setSubTitle(tr("Un serveur de jeu peut être créé à partir d'une version de TarotClub ou d'un serveur dédié."));
}
/*****************************************************************************/
void JoinWizardPage1::initializePage()
{
   ui.portNumber->setValue(DEFAULT_PORT);
}
/*****************************************************************************/


/*****************************************************************************/
JoinWizard::JoinWizard(QWidget *parent) : QWizard(parent)
{
   page1 = new JoinWizardPage1(this);

   page2 = new QWizardPage(this);
   uiPage2.setupUi(page2);
   page2->setTitle(tr("Connexion à un serveur TarotClub"));
   page2->setSubTitle(tr("Choisissez un salon et une table de jeu."));

   setPage(Page_Server, page1);
   setPage(Page_Lobby, page2);

   setPixmap(QWizard::WatermarkPixmap, QPixmap(":images/wizard.png"));
   setWizardStyle(QWizard::ModernStyle);
}

