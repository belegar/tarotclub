 
var BidValue = {
    PASSE:0,
    PRISE:1,
    GARDE:2,
    GARDE_SANS:3,
    GARDE_CONTRE:4
}
function calculEnchere()
{
	var total = this.getToto();
	
	//var total = stats.nbCards;
	return total;
}

function calculateBid()
{
   total = 0;

   // On distribue des points en fonction des stats
   if( stats.vingtEtUn == true ) {
      total += 9;
   }
   if( stats.excuse == true ) {
      total += 7;
   }
   if( stats.petit == true ) {
      if( stats.atouts == 5 ) {
         total += 5;
      } else if( stats.atouts == 6 || stats.atouts == 7 ) {
         total += 7;
      } else if( stats.atouts > 7 ) {
         total += 8;
      }
   }

   // Chaque atout vaut deux points
   // Chaque atout majeur vaut 1 point supplémentaire
   total += stats.atouts * 2;
   total += stats.atoutsMajeurs * 2;
   total += stats.rois * 6;
   total += stats.dames * 3;
   total += stats.cavaliers * 2;
   total += stats.valets;
   total += stats.mariages;
   total += stats.longues * 5;
   total += stats.coupes * 5;
   total += stats.singletons * 3;
   total += stats.sequences * 4;

   // on décide de l'enchère :
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
