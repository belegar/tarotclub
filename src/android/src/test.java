
public class test
{
    public static void main(String[] args)
    {
		Card c = new Card(0, Card.SPADES, 4.5f, 0);
		
		Deck deck = new Deck();
		
		deck.addCard(c);
		
		System.out.println("Hello, World!");
		System.out.println(c.getPoints());
		System.out.println(deck.size());
		System.out.println(deck.getCard(0).getPoints());
	}
}
