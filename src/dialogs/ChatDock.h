

#ifndef CHATDOCK_H
#define CHATDOCK_H

// Includes Qt
#include <QDockWidget>
#include <QLineEdit>
#include <QTextBrowser>
#include <QPushButton>
#include <QListWidget>

// Includes locales
#include "../defines.h"

/*****************************************************************************/
class ChatDock : public QDockWidget
{
   Q_OBJECT

private:
   QTextBrowser    *chatWindow;
   QLineEdit       *textLine;
   QPushButton     *options;
   QListWidget     *playersList;

public:
   ChatDock( QWidget *parent );

   void clear();
   void setPlayersList( int nombre, Identity *idents );
   void clearPlayersList();

protected:
   void closeEvent( QCloseEvent * e );

public slots:
   void slotReturnPressed();
   void message(const QString &message);

signals:
   void sgnlEmitMessage(const QString&);
   void sgnlClose();
};

#endif // CHATDOCK_H

//=============================================================================
// Fin du fichier ChatDock.h
//=============================================================================
