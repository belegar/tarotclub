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
import android.content.pm.ActivityInfo;
import android.os.Bundle;
import android.view.Menu;
import android.view.MenuItem;
import android.view.Window;

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
    	  mTarotEngine.DisplayAbout();
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
  
  // end of class
}

//=============================================================================
//End of file TarotClub.java
//=============================================================================
