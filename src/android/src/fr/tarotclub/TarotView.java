/*=============================================================================
 * TarotClub - TarotView.java
 *=============================================================================
 * Handle all gfx stuff
 *=============================================================================
 * TarotClub ( http://www.tarotclub.fr ) - This file is part of TarotClub
 * Copyright (C) 2003-2999 - Anthony Rabine
 * anthony@tarotclub.fr
 *
 * This file must be used under the terms of the CeCILL.
 * This source file is licensed as described in the file COPYING, which
 * you should have received as part of this distribution.  The terms
 * are also available at
 * http://www.cecill.info/licences/Licence_CeCILL_V2-en.txt
 *
 *=============================================================================
 */

package fr.tarotclub;

import android.app.Activity;
import android.content.Context;
import android.content.res.Resources;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.graphics.Point;
import android.graphics.RectF;
import android.graphics.drawable.Drawable;
import android.util.AttributeSet;
import android.view.Gravity;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

// The brains of the operation
public class TarotView extends View {
	
  private CharSequence mAboutText;
  private TextView mTextView;
  private String mPackage = "fr.tarotclub";
  private String mDir = "drawable" ;

  //Card stuff
  protected Card[] cards;
  private Bitmap[] mCardBitmap;
  private Paint mCardPaint;
  
  // Background
  protected int mScreenWidth;
  protected int mScreenHeight;
  private Paint mBGPaint;
  
  // bid values
  private TextView mBidSouth;
  private TextView mBidEast;
  private TextView mBidNorth;
  private TextView mBidWest;
  
  // bid buttons
  Button buttonPasse;
  Button buttonPrise;
  Button buttonGarde; 
  Button buttonGardeSans; 
  Button buttonGardeContre;
  
  /*****************************************************************************/
  public TarotView(Context context, AttributeSet attrs) {
    super(context, attrs);
    setFocusable(true);
    setFocusableInTouchMode(true);
  
    mAboutText = context.getResources().getText(R.string.about_text);

    // Default to this for simplicity
    mScreenWidth = 480;
    mScreenHeight = 295;

    // Background
    mBGPaint = new Paint();
    mBGPaint.setARGB(255, 0, 128, 0);
    
    // card footprints
    mCardPaint = new Paint();
    mCardPaint.setARGB(70, 0, 0, 0);

    cards = new Card[78];
    mCardBitmap = new Bitmap[78];
    LoadCards(context);
  }
  /*****************************************************************************/
  public void showBidButtons(int bid) {
	  
	  buttonPasse.setVisibility(View.VISIBLE);
	  
	  if (bid >= Game.PRISE) {
		  buttonPrise.setVisibility(View.INVISIBLE);
	  } else {
		  buttonPrise.setVisibility(View.VISIBLE);
	  }
	  if (bid >= Game.GARDE) {
		  buttonGarde.setVisibility(View.INVISIBLE);
	  } else {
		  buttonGarde.setVisibility(View.VISIBLE);
	  }
	  if (bid >= Game.GARDE_SANS) {
		  buttonGardeSans.setVisibility(View.INVISIBLE);
	  } else {
		  buttonGardeSans.setVisibility(View.VISIBLE);
	  }
	  if (bid >= Game.GARDE_CONTRE) {
		  buttonGardeContre.setVisibility(View.INVISIBLE);
	  } else {
		  buttonGardeContre.setVisibility(View.VISIBLE);
	  }
  }
  /*****************************************************************************/
  public void hideBidButtons() {
	  buttonPasse.setVisibility(View.INVISIBLE);
	  buttonPrise.setVisibility(View.INVISIBLE);
	  buttonGarde.setVisibility(View.INVISIBLE);
	  buttonGardeSans.setVisibility(View.INVISIBLE);
	  buttonGardeContre.setVisibility(View.INVISIBLE);
  }
  /*****************************************************************************/
  public void hideBidTexts() {
	  mBidSouth.setVisibility(View.INVISIBLE);
	  mBidEast.setVisibility(View.INVISIBLE);
	  mBidNorth.setVisibility(View.INVISIBLE);
	  mBidWest.setVisibility(View.INVISIBLE);
  }
  /*****************************************************************************/
  public CharSequence getBidText(int bid) {
	  CharSequence txt;
	  
	  if (bid == Game.PASSE)
		  txt = getContext().getResources().getString(R.string.bid_passe);
	  else if (bid == Game.PRISE)
		  txt = getContext().getResources().getString(R.string.bid_prise);
	  else if (bid == Game.GARDE)
		  txt = getContext().getResources().getString(R.string.bid_garde);
	  else if (bid == Game.GARDE_SANS)
		  txt = getContext().getResources().getString(R.string.bid_garde_sans);
	  else
		  txt = getContext().getResources().getString(R.string.bid_garde_contre);
	  
	  return txt;
  }
  
  /*****************************************************************************/
  public void showBid(int place, int bid) {
	  CharSequence txt = "";
	  
	  txt = getBidText(bid);
	  
	  if (place == Game.SOUTH) {
		  mBidSouth.setText(txt);
		  mBidSouth.setVisibility(View.VISIBLE);
	  } else if (place == Game.EAST) {
		  mBidEast.setText(txt);
		  mBidEast.setVisibility(View.VISIBLE);
	  } else if (place == Game.NORTH) {
		  mBidNorth.setText(txt);
		  mBidNorth.setVisibility(View.VISIBLE);
	  } else {
		  mBidWest.setText(txt);
		  mBidWest.setVisibility(View.VISIBLE);
	  }
  }
  /*****************************************************************************/
  /**
   * One-time initializations
   */
  public void InitGame() {
    // We really really want focus :)
    setFocusable(true);
    setFocusableInTouchMode(true);
    requestFocus();
    mTextView.setVisibility(View.INVISIBLE);
    hideBidButtons();

  }
  /*****************************************************************************/
  public void setupUI(Activity act) {
	  mTextView = (TextView) act.findViewById(R.id.text);

	  buttonPasse = (Button) act.findViewById(R.id.buttonPasse);
	  buttonPrise = (Button) act.findViewById(R.id.buttonPrise);
	  buttonGarde = (Button) act.findViewById(R.id.buttonGarde);
	  buttonGardeSans = (Button) act.findViewById(R.id.buttonGardeSans);
	  buttonGardeContre = (Button) act.findViewById(R.id.buttonGardeContre);
		
	  mBidSouth = (TextView) act.findViewById(R.id.bidSouth);
	  mBidEast = (TextView) act.findViewById(R.id.bidEast);
	  mBidNorth = (TextView) act.findViewById(R.id.bidNorth);
	  mBidWest = (TextView) act.findViewById(R.id.bidWest);
  }
  /*****************************************************************************/
  void showResult(Infos infos) {
	  StringBuilder txt = new StringBuilder("Résultat: ");
	  
	  if (((int)infos.attaque) >= infos.pointsAFaire) {
		  txt.append("L'attaque gagne ! :)\n");
	  } else {
		  txt.append("L'attaque perd ! :(\n");
	  }
	  
	  txt.append("Preneur: ");
	  // On affiche les caractéristiques de la partie
	  if( infos.taker == Game.SOUTH ) {
		  txt.append(getContext().getResources().getString(R.string.place_south));
	  } else if( infos.taker == Game.EAST ) {
		  txt.append(getContext().getResources().getString(R.string.place_east));
	  } else if( infos.taker == Game.NORTH ) {
		  txt.append(getContext().getResources().getString(R.string.place_north));
	  } else {
		  txt.append(getContext().getResources().getString(R.string.place_west));
	  }
	  
	  txt.append("\nContrat: ");
	  if( infos.contract == Game.PRISE ) {
		  txt.append(getContext().getResources().getString(R.string.bid_prise));
	  } else if( infos.contract == Game.GARDE ) {
		  txt.append(getContext().getResources().getString(R.string.bid_garde));
	  } else if( infos.contract == Game.GARDE_SANS ) {
		  txt.append(getContext().getResources().getString(R.string.bid_garde_sans));
	  } else {
		  txt.append(getContext().getResources().getString(R.string.bid_garde_contre));
	  }
	  
	  txt.append("\nBouts: ");
	  txt.append(infos.bouts);
	  txt.append("\nPoints à faire: ");
	  txt.append(infos.pointsAFaire);
	  txt.append("\nPoints réalisés: ");
	  txt.append((int)infos.attaque);
	  
	  DisplayText(txt.toString());
  }
  /*****************************************************************************/
  public void DisplayAbout() {
    mTextView.setTextSize(15);
    mTextView.setGravity(Gravity.LEFT);
    DisplayText(mAboutText);
  }
  /*****************************************************************************/
  public void clearText() {
	  mTextView.setVisibility(View.INVISIBLE);
  }
  /*****************************************************************************/
  public void DisplayText(CharSequence text) {
    mTextView.setVisibility(View.VISIBLE);
    mTextView.setText(text);
   }
  /*****************************************************************************/
  public void drawCardFootPrint(Point p, Canvas canvas) {
	  RectF pos = new RectF(p.x, p.y, p.x + Card.WIDTH, p.y + Card.HEIGHT);
	  canvas.drawRoundRect(pos, 4, 4, mCardPaint);
  }
  /*****************************************************************************/
  public void drawChienArea(RectF pos, Canvas canvas) {
	  canvas.drawRoundRect(pos, 4, 4, mCardPaint);
  }
  /*****************************************************************************/
  public void drawBackground(Canvas canvas) {
	  canvas.drawRect(0, 0, mScreenWidth, mScreenHeight, mBGPaint);
  }
  /*****************************************************************************/
  public void DrawCard(Canvas canvas, Card card) {
    float x = card.getX();
    float y = card.getY();
    int idx = card.getId();
    canvas.drawBitmap(mCardBitmap[idx], x, y, null);
  }
  /*****************************************************************************/
  private void LoadCardBitmap(Context context, int idx, String varImg, int i) {
	Canvas canvas;
	String numImg = "";
	Resources r = context.getResources();
	
	// avoid slow String format
	if (i>0) {
		if (i<10)
			numImg = "0" + Integer.toString(i);
		else
			numImg =  Integer.toString(i);
	}
	
	int resID = r.getIdentifier(varImg + numImg, mDir, mPackage);
	Drawable drawable = r.getDrawable(resID);
	mCardBitmap[idx] = Bitmap.createBitmap(Card.WIDTH, Card.HEIGHT, Bitmap.Config.ARGB_4444);
	canvas = new Canvas(mCardBitmap[idx]);
	drawable.setBounds(0, 0, Card.WIDTH, Card.HEIGHT);
	drawable.draw(canvas);
  }
  /*****************************************************************************/
  private void LoadCards(Context context) {   
	  int id;
	  
	  // get suits images
	  for (int i=0; i<4; i++) {
	  	String varImg;
	  	if (i == Card.SPADES) {
	  		id = 0;
	  		varImg = "pique";
		} else if (i == Card.HEARTS) {
			id = 14;
			varImg = "coeur";
		} else if (i == Card.CLUBS) {
			id = 28;
			varImg = "trefle";
		} else {
			id = 42;
			varImg = "carreau";
		}
	  	
	  	// 14 cards per suit (1-10 + joker, knight, queen, king)
	  	for (int j=0; j<14; j++) {
	  		float points;
			if (j == 10) {
				points = 1.5f;
			} else if (j == 11) {
				points = 2.5f;
			} else if (j == 12) {
				points = 3.5f;
			} else if (j == 13) {
				points = 4.5f;
			} else {
				points = 0.5f;
			}
			cards[id+j] = new Card(j+1, i, points, id+j);
	  		LoadCardBitmap(context, i*14+j, varImg, j+1);
	  	}
	  }
	  
	// 21 atouts cards
	for (int i=56; i<77; i++) {
		float points;
		if (i == 56 || i == 76) {
			points = 4.5f;
		} else {
			points = 0.5f;
		}
		cards[i] = new Card(i-55, Card.ATOUT, points, i);
		LoadCardBitmap(context, i, "atout", i-55);
	}
		
	// Excuse
	cards[77] = new Card(-1, Card.EXCUSE, 4.5f, 77);
	LoadCardBitmap(context, 77, "excuse", -1);
	
  }
}

// End of file
