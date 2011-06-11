
-- return one of the following values:
BIDS = {
	PASSE = 0,
	PRISE=1, 
	GARDE=2, 
	GARDE_SANS=3, 
	GARDE_CONTRE=4
}

function CalculateBid()
	local b = Bot(BotObject)
	
	return Stats.nbCards;
	
		-- simple algorithm to calculate the bid
	local total = 0
    local cont

    -- On distribue des points en fonction des stats
    if Stats.vingtEtUn == true then
      total = total + 9
    end
	
	if Stats.excuse == true then
	  total = total + 7
	end
	
	if Stats.petit == true then
	  if Stats.atouts == 5 then
		 total = total + 5
	  elseif Stats.atouts == 6 or Stats.atouts == 7 then
		 total = total + 7
	  elseif Stats.atouts > 7 then
		 total = total + 8
	  end
	end

   -- Chaque atout vaut deux points
   -- Chaque atout majeur vaut 1 point supplémentaire
   total = total + Stats.atouts * 2
   total = total + Stats.atoutsMajeurs * 2
   total = total + Stats.rois * 6
   total = total + Stats.dames * 3
   total = total + Stats.cavaliers * 2
   total = total + Stats.valets
   total = total + Stats.mariages
   total = total + Stats.longues * 5
   total = total + Stats.coupes * 5
   total = total + Stats.singletons * 3
   total = total + Stats.sequences * 4

   -- on décide de l'enchère :
   if total <= 35 then
      cont = PASSE
	elseif total >= 36  and total <= 50 then
      cont = PRISE
	elseif total >= 51  and total <= 65 then
      cont = GARDE;
	elseif total >= 66  and total <= 75 then
      cont = GARDE_SANS;
	else
      cont = GARDE_CONTRE;
	end
	
   return cont
	
end
