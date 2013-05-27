#include "Table.h"

Table::Table()
    : maximumPlayers(4)
    , size(0)
{
}
/*****************************************************************************/
void Table::run()
{
    exec();
}
/*****************************************************************************/
void Table::customEvent(QEvent *e)
{
    Q_UNUSED(e);
    /*
       TODO: review for the stand alone server

       if( (int)e->type() == MsgStartGame ) {
          if( gateState !=  ) {
             emit sigPrintMessage("Game already started.");
             return;
          }
          newServerGame();
       } else if( (int)e->type() == MsgStopGame ) {
          if( newGame == false ) {
             emit sigPrintMessage("Game not started.");
             return;
          }
          closeServerGame();
       } else if( (int)e->type() == MsgExitGame ) {
          closeServerGame();
         // QCoreApplication::exit(0);
       }
    */
}
/*****************************************************************************/
void Table::SetMaximumPlayers(int n)
{

}

