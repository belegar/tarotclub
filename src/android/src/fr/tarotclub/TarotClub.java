/*=============================================================================
 * TarotClub - TarotClub.java
 *=============================================================================
 * Entry point of the game
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
import android.app.Dialog;
import android.content.Context;
import android.content.pm.ActivityInfo;
import android.os.Bundle;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.Window;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.TextView;

// Base activity class.
public class TarotClub extends Activity {
  private static final int MENU_NEW_GAME	= 1;
  private static final int MENU_ABOUT   	= 2;
  private static final int MENU_QUIT     	= 3;

  // View extracted from main.xml.
  private TarotEngine mTarotEngine;
  
  /*****************************************************************************/
  @Override
  public void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);

    // Force landscape and no title bar for extra room
    setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);
    requestWindowFeature(Window.FEATURE_NO_TITLE);

    setContentView(R.layout.main);

    mTarotEngine = (TarotEngine) findViewById(R.id.tarot);
    mTarotEngine.setupUI(this);
    mTarotEngine.setupHandlers();
  }
  /*****************************************************************************/
  // Entry point for starting the game.
  @Override
  public void onStart() {
    super.onStart();
    mTarotEngine.InitGame();
  }
  /*****************************************************************************/
  @Override
  public boolean onCreateOptionsMenu(Menu menu) {
    super.onCreateOptionsMenu(menu);
    menu.add(0, MENU_NEW_GAME, 0, R.string.menu_newgame);
    menu.add(0, MENU_ABOUT, 0, R.string.menu_about);
    menu.add(0, MENU_QUIT, 0, R.string.menu_quit);
    return true;
  }
  /*****************************************************************************/
  @Override
  public boolean onOptionsItemSelected(MenuItem item) {
    switch (item.getItemId()) {
      case MENU_NEW_GAME:
    	  mTarotEngine.newGame();
        break;
      case MENU_ABOUT:
    	  showAboutDialog();
        break;
      case MENU_QUIT:
        finish();
        break;
    }

    return false;
  }
  /*****************************************************************************/
  @Override
  protected void onPause() {
    super.onPause();
  }
  /*****************************************************************************/
  @Override
  protected void onStop() {
    super.onStop();
  }
  /*****************************************************************************/
  @Override
  protected void onResume() {
    super.onResume();
  }
  /*****************************************************************************/
  @Override
  public void onSaveInstanceState(Bundle outState) {
    super.onSaveInstanceState(outState);
  }
  /*****************************************************************************/
  public void showAboutDialog() {
	  AboutBox dialog = new AboutBox(TarotClub.this);   
	  dialog.show();
  }
  // end of class
}
/*****************************************************************************/
final class AboutBox extends Dialog implements OnClickListener {

	public AboutBox(Context context) {
		super(context);
		
		setContentView(R.layout.aboutbox);
		setTitle("TarotClub " + context.getString(R.string.app_version));

		TextView text = (TextView) findViewById(R.id.TextView01);
		text.setText(R.string.about_text);
		
		ImageView img = (ImageView) findViewById(R.id.ImageView01);
		img.setImageResource(R.drawable.splash);
		
		Button button = (Button) findViewById(R.id.Button01);
		button.setOnClickListener(this);
		
	}
	@Override
    public void onClick(View v) {
		dismiss();
    }
}

//=============================================================================
//End of file TarotClub.java
//=============================================================================
