
#include "Jeu.h"

Card Jeu::cards[78];

/*****************************************************************************/
Jeu::Jeu()
{

}
/*****************************************************************************/
void Jeu::init()
{
   int i,j,n;
   CardColor coul;
   int id;

   //----- 4 couleurs
   for( i=0; i<4; i++ ){
      if( i==0 ) {
         coul = PIC;
         id = 0;
      } else if( i==1 ) {
         coul = COEUR;
         id = 14;
      } else if( i==2 ) {
         coul = TREFLE;
         id = 28;
      } else {
         coul = CARREAU;
         id = 42;
      }

      // de l'as au roi (14 cartes)
      for( j=0; j<14; j++ ) {
         n = i*14+j;
         cards[n].setColor( coul );
         cards[n].setType( CARTE );
         cards[n].setValue( j+1 );  // [1..14]
         cards[n].setId( id + j );

         if( j==10 ) {// Valet
            cards[n].setPoints( 1.5 );
         } else if( j==11 ) {// Cavalier
            cards[n].setPoints( 2.5 );
         } else if( j==12 ) {// Dame
            cards[n].setPoints( 3.5 );
         } else if( j==13 ) {// Roi
            cards[n].setPoints( 4.5 );
         } else {
            cards[n].setPoints( 0.5 );
         } 
      }
   }

   id = 56;

   //----- Les 21 atouts
   for( i=56; i<77; i++) {
      cards[i].setType( ATOUT );
      cards[i].setValue( i-55 ); // [1..21]
      cards[i].setColor( NO_COLOR );
      cards[i].setId( id + i - 56 );

      if( i==56 || i==76) { // Petit ou 21
         cards[i].setPoints( 4.5 );
      } else {
         cards[i].setPoints( 0.5 );
      }
   }

   //----- L'excuse
   cards[77].setType( EXCUSE );
   cards[77].setColor( NO_COLOR );
   cards[77].setValue( 0 );
   cards[77].setPoints( 4.5 );
   cards[77].setId( 77 );
}
/*****************************************************************************/
Card *Jeu::getCard(int id)
{
   if( id<0 || id>77 ) {
      return NULL;
   } else {
      return(&cards[id]);
   }
}

//=============================================================================
// End of file PhysicalCards.cpp
//=============================================================================
