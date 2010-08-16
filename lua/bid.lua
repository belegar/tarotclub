
-- return one of the following values:
-- PASSE=0, PRISE=1, GARDE=2, GARDE_SANS=3, GARDE_CONTRE=4 

function CalculateBid()
	local b = Bot(BotObject)
    return b:getCardNumber();
end
