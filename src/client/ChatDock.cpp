
// Includes Qt
#include <QDockWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSplitter>

// Includes locales
#include "ChatDock.h"

/*****************************************************************************/
ChatDock::ChatDock( QWidget *parent )
    : QDockWidget( tr("Discussion"), parent )
{
   chatWindow = new QTextBrowser();
   textLine = new QLineEdit();
   options = new QPushButton(tr("Commandes"));
   playersList = new QListWidget();

   QHBoxLayout *lay1 = new QHBoxLayout();
   lay1->addWidget(textLine);
   lay1->addWidget(options);

   QSplitter *lay2 = new QSplitter();
   lay2->addWidget(chatWindow);
   lay2->addWidget(playersList);

   QVBoxLayout *principal = new QVBoxLayout();
   principal->addLayout(lay1);
   principal->addWidget(lay2);

   QFrame *fr = new QFrame(this);
   fr->setLayout(principal);
   setWidget(fr);

   connect( textLine,SIGNAL( returnPressed() ), this, SLOT(slotReturnPressed()) );
}
/*****************************************************************************/
void ChatDock::closeEvent( QCloseEvent * e )
{
    emit sgnlClose();
}
/*****************************************************************************/
void ChatDock::clear()
{
   playersList->clear();
   chatWindow->clear();
}
/*****************************************************************************/
void ChatDock::clearPlayersList()
{
   chatWindow->clear();
}
/*****************************************************************************/
void ChatDock::message(const QString &message)
{
   chatWindow->append( message );
}
/*****************************************************************************/
void ChatDock::slotReturnPressed()
{
	QString message = textLine->text();
   if( message.length() == 0 ) {
		return;
   }
	emit sgnlEmitMessage(message);
	textLine->clear();
}
/*****************************************************************************/
void ChatDock::setPlayersList( int nombre, Identity *idents )
{
	QString server;
	QStringList nicks;

   for( int i=0; i<nombre; i++ ) {
		nicks += idents[i].name;
   }
	playersList->clear();
	// On marque le premier joueur comme étant le serveur
	playersList->addItems( nicks );
   QListWidgetItem *it = playersList->item(0);
	server = it->text();
	server.prepend( '@' );
	it->setText( server );
}

//=============================================================================
// Fin du fichier ChatDock.cpp
//=============================================================================
