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
import android.widget.TextView;

// The brains of the operation
public class TarotView extends View {
	
  private CharSequence mAboutText;
  private TextView mTextView;
  private String mPackage = "fr.tarotclub";
  private String mDir ="drawable" ;

  //Card stuff
  private Bitmap[] mCardBitmap;
  private Paint mCardPaint;
  
  // Background
  private int mScreenWidth;
  private int mScreenHeight;
  private Paint mBGPaint;
  
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
    mCardPaint.setARGB(100, 0, 0, 0);

    mCardBitmap = new Bitmap[78];
    LoadCards(context);

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
