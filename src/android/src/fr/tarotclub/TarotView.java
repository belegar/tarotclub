/* License here! */

package fr.tarotclub;

import android.content.Context;
import android.content.res.Resources;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Paint;
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
  private Bitmap[] mCardBitmap;
  private Paint mCardPaint;
  
  // Background
  private int mScreenWidth;
  private int mScreenHeight;
  private Paint mBGPaint;
  
  // bid values
  private TextView mBidSouth;
  private TextView mBidEast;
  private TextView mBidNorth;
  private TextView mBidWest;
  
  // bid buttons
  Button buttonPasse = (Button) findViewById(R.id.buttonPasse);
  Button buttonPrise = (Button) findViewById(R.id.buttonPrise);
  Button buttonGarde = (Button) findViewById(R.id.buttonGarde);
  Button buttonGardeSans = (Button) findViewById(R.id.buttonGardeSans);
  Button buttonGardeContre = (Button) findViewById(R.id.buttonGardeContre);
  
  /*****************************************************************************/
  public TarotView(Context context, AttributeSet attrs) {
    super(context, attrs);
    setFocusable(true);
    setFocusableInTouchMode(true);
  
    mAboutText = context.getResources().getText(R.string.about_text);
    
    mBidSouth = (TextView) findViewById(R.id.bidSouth);
    mBidEast = (TextView) findViewById(R.id.bidEast);
    mBidNorth = (TextView) findViewById(R.id.bidNorth);
    mBidWest = (TextView) findViewById(R.id.bidWest);
        
    // Default to this for simplicity
    mScreenWidth = 480;
    mScreenHeight = 295;

    // Background
    mBGPaint = new Paint();
    mBGPaint.setARGB(255, 0, 128, 0);
    
    // card footprints
    mCardPaint = new Paint();
    mCardPaint.setARGB(70, 0, 0, 0);

    mCardBitmap = new Bitmap[78];
    LoadCards(context);
    
//    removeBidButtons();
  }
  /*****************************************************************************/
  public void showBidButtons(int bid) {
	  
	  buttonPasse.setVisibility(View.VISIBLE);
	  
	  if (bid > Game.PRISE) {
		  buttonPrise.setVisibility(View.VISIBLE);
	  } else {
		  buttonPrise.setVisibility(View.INVISIBLE);
	  }
	  if (bid > Game.GARDE) {
		  buttonGarde.setVisibility(View.VISIBLE);
	  } else {
		  buttonGarde.setVisibility(View.INVISIBLE);
	  }
	  if (bid > Game.GARDE_SANS) {
		  buttonGardeSans.setVisibility(View.VISIBLE);
	  } else {
		  buttonGardeSans.setVisibility(View.INVISIBLE);
	  }
	  if (bid > Game.GARDE_CONTRE) {
		  buttonGardeContre.setVisibility(View.VISIBLE);
	  } else {
		  buttonGardeContre.setVisibility(View.INVISIBLE);
	  }
  }
  /*****************************************************************************/
  public void removeBidButtons() {
	  buttonPasse.setVisibility(View.INVISIBLE);
	  buttonPrise.setVisibility(View.INVISIBLE);
	  buttonGarde.setVisibility(View.INVISIBLE);
	  buttonGardeSans.setVisibility(View.INVISIBLE);
	  buttonGardeContre.setVisibility(View.INVISIBLE);
  }
  /*****************************************************************************/
  public void showBid(int place, int bid) {
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
	  
	  if (place == Game.SOUTH) {
		  mBidSouth.setText(txt);
	  } else if (place == Game.EAST) {
		  mBidEast.setText(txt);
	  } else if (place == Game.NORTH) {
		  mBidNorth.setText(txt);
	  } else {
		  mBidWest.setText(txt);
	  }
  }
  /*****************************************************************************/
public void InitGame() {
    // We really really want focus :)
    setFocusable(true);
    setFocusableInTouchMode(true);
    requestFocus();
    mTextView.setVisibility(View.INVISIBLE);
  }
/*****************************************************************************/
  public void SetTextView(TextView textView) {
    mTextView = textView;
  }
  /*****************************************************************************/
  public void DisplayAbout() {
    mTextView.setTextSize(15);
    mTextView.setGravity(Gravity.LEFT);
    DisplayText(mAboutText);
  }
  /*****************************************************************************/
  public void ClearText() {
	  mTextView.setVisibility(View.INVISIBLE);
  }
  /*****************************************************************************/
  public void DisplayText(CharSequence text) {
    mTextView.setVisibility(View.VISIBLE);
    mTextView.setText(text);
   }
  /*****************************************************************************/
  public void drawCardFootPrint(int x, int y, Canvas canvas) {
	  RectF pos = new RectF(x, y, x + Card.WIDTH, y + Card.HEIGHT);
	  canvas.drawRoundRect(pos, 4, 4, mCardPaint);
  }
  /*****************************************************************************/
  public void drawChienArea(int x, int y, Canvas canvas) {
	  RectF pos = new RectF(x, y, x+200, y+230);
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
	  // get suits images
	  for (int i=0; i<4; i++) {
	  	String varImg;
	  	if (i == Card.SPADES) {
	  		varImg = "pique";
		} else if (i == Card.HEARTS) {
			varImg = "coeur";
		} else if (i == Card.CLUBS) {
			varImg = "trefle";
		} else {
			varImg = "carreau";
			}
	  	for (int j=0; j<14; j++) {
	  		LoadCardBitmap(context, i*14+j, varImg, j+1);
	  	}
	  }
	  
	  // 21 atouts
	  for(int i=0; i<21; i++) {
	  	LoadCardBitmap(context, 56+i, "atout", i+1);
	  }
		
	  // Excuse
	  LoadCardBitmap(context, 77, "excuse", -1);
  }
}
/*****************************************************************************/
