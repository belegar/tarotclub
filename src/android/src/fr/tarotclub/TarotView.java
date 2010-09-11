/* License here! */

package fr.tarotclub;

import android.content.Context;
import android.content.res.Resources;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.graphics.drawable.Drawable;
import android.util.AttributeSet;
import android.view.Gravity;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.View;
import android.widget.TextView;

// The brains of the operation
public class TarotView extends View {
  private CharSequence mAboutText;
  private TextView mTextView;
  private RefreshHandler mRefreshHandler;
  private String mPackage = "fr.tarotclub";
  private String mDir ="drawable" ;

  //Card stuff
  private Bitmap[] mCardBitmap;
  
  // Background
  private int mScreenWidth;
  private int mScreenHeight;
  private Paint mBGPaint;
  
  // Tarot game stuff
  private Table mTable;
  
  /*****************************************************************************/
  public TarotView(Context context, AttributeSet attrs) {
    super(context, attrs);
    setFocusable(true);
    setFocusableInTouchMode(true);
    
    mRefreshHandler = new RefreshHandler(this);
    mAboutText = context.getResources().getText(R.string.about_text);
        
    // Default to this for simplicity
    mScreenWidth = 480;
    mScreenHeight = 295;

    // Background
    mBGPaint = new Paint();
    mBGPaint.setARGB(255, 0, 128, 0);

    mCardBitmap = new Bitmap[78];
    DrawCards(context);
    
    mTable = new Table();
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
/*
  protected void onSizeChanged(int w, int h, int oldw, int oldh) {
    mDrawMaster.SetScreenSize(w, h);
    mRules.Resize(w, h);
    mSelectCard.SetHeight(h);
  }
*/
  /*****************************************************************************/
  public void DisplayAbout() {
    mTextView.setTextSize(15);
    mTextView.setGravity(Gravity.LEFT);
    DisplayText(mAboutText);
  }
/*
  public void DisplayWin() {
    MarkWin();
    mTextView.setTextSize(24);
    mTextView.setGravity(Gravity.CENTER_HORIZONTAL);
    DisplayText(mWinText);
    ChangeViewMode(MODE_WIN);
    mTextView.setVisibility(View.VISIBLE);
    mRules.SetIgnoreEvents(true);
    mReplay.StartReplay(mMoveHistory, mCardAnchor);
  }
*/
  /*****************************************************************************/
  public void DisplayText(CharSequence text) {
   // ChangeViewMode(MODE_TEXT);
    mTextView.setVisibility(View.VISIBLE);
    mTextView.setText(text);
   }

  /*****************************************************************************/
  @Override
  public void onDraw(Canvas canvas) {
	  canvas.drawRect(0, 0, mScreenWidth, mScreenHeight, mBGPaint);
	  DrawCard(canvas, mTable.getCard(0));
	  Refresh();
  }

  public void Refresh() {
    mRefreshHandler.SingleRefresh();
  }
  /*****************************************************************************/
  @Override
  public boolean onKeyDown(int keyCode, KeyEvent msg) {
    switch (keyCode) {
    case KeyEvent.KEYCODE_DPAD_CENTER:
      return true;
      case KeyEvent.KEYCODE_BACK:
        return true;
      }
    return super.onKeyDown(keyCode, msg);
  }
  /*****************************************************************************/
  @Override
  public boolean onTouchEvent(MotionEvent event) {
    boolean ret = false;

    switch (event.getAction()) {
      case MotionEvent.ACTION_DOWN:
        break;
      case MotionEvent.ACTION_UP:
      case MotionEvent.ACTION_CANCEL:
        break;
      case MotionEvent.ACTION_MOVE:
        break;
    }
    return ret;
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
  private void DrawCards(Context context) {   
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


/*****************************************************************************/
class RefreshHandler implements Runnable {
  public static final int NO_REFRESH = 1;
  public static final int SINGLE_REFRESH = 2;
  public static final int LOCK_REFRESH = 3;

  private static final int FPS = 30;

  private boolean mRun;
  private int mRefresh;
  private TarotView mView;

  public RefreshHandler(TarotView solitaireView) {
    mView = solitaireView;
    mRun = true;
    mRefresh = NO_REFRESH;
  }

  public void SetRefresh(int refresh) {
    synchronized (this) {
      mRefresh = refresh;
    }
  }

  public void SingleRefresh() {
    synchronized (this) {
      if (mRefresh == NO_REFRESH) {
        mRefresh = SINGLE_REFRESH;
      }
    }
  }

  public void SetRunning(boolean run) {
    mRun = run;
  }

  public void run() {
    while (mRun) {
      try {
        Thread.sleep(1000 / FPS);
      } catch (InterruptedException e) {
      }
      if (mRefresh != NO_REFRESH) {
        mView.postInvalidate();
        if (mRefresh == SINGLE_REFRESH) {
          SetRefresh(NO_REFRESH);
        }
      }
    }
  }
}
