 
var BidValue = {
    PASSE:0,
    PRISE:1,
    GARDE:2,
    GARDE_SANS:3,
    GARDE_CONTRE:4
}


function playCard()
{
	var cards = TDeck.GetBotCards();
	var result = cards.split(/;/g);
	print(result[0]);

	return result[0];
}

function calculateBid()
{
   total = 0;

   //TDbg.Print("coucou!");
   
   print("coucou!");
   
   // We start looking at bouts, each of them increase the total value of points
   if( TStats.hasVingtEtUn() == true ) {
      total += 9;
   }
   if( TStats.hasExcuse == true ) {
      total += 7;
   }
   if( TStats.hasPetit == true ) {
      if( TStats.getNumberOfAtouts == 5 ) {
         total += 5;
      } else if( TStats.getNumberOfAtouts == 6 || TStats.getNumberOfAtouts == 7 ) {
         total += 7;
      } else if( TStats.getNumberOfAtouts > 7 ) {
         total += 8;
      }
   }

   // Each atout counts two points
   // Each major atout counts one more point
   total += TStats.getNumberOfAtouts() * 2;
   total += TStats.getNumberOfAtoutsMajeurs() * 2;
   total += TStats.getNumberOfRois() * 6;
   total += TStats.getNumberOfDames() * 3;
   total += TStats.getNumberOfCavaliers() * 2;
   total += TStats.getNumberOfValets();
   total += TStats.getNumberOfMariages();
   total += TStats.getNumberOfLongues() * 5;
   total += TStats.getNumberOfCoupes() * 5;
   total += TStats.getNumberOfSingletons() * 3;
   total += TStats.getNumberOfSequences() * 4;

   // We decide on a bid depending of thresholds
   if( total <= 35 ) {
      cont = BidValue.PASSE;
   } else if( total >= 36  && total <= 50 ) {
      cont = BidValue.PRISE;
   } else if( total >= 51  && total <= 65 ) {
      cont = BidValue.GARDE;
   } else if( total >= 66  && total <= 75 ) {
      cont = BidValue.GARDE_SANS;
   } else {
      cont = BidValue.GARDE_CONTRE;
   }
   return cont;
}

// End of file
