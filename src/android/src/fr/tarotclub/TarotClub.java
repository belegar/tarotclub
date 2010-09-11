
/* License here! */

package fr.tarotclub;

import android.app.Activity;
import android.content.pm.ActivityInfo;
import android.os.Bundle;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.Window;
import android.widget.TextView;

// Base activity class.
public class TarotClub extends Activity {
  private static final int MENU_NEW_GAME	= 1;
  private static final int MENU_ABOUT   	= 2;
  private static final int MENU_QUIT     	= 3;

  // View extracted from main.xml.
  private View mMainView;
  private TarotView mTarotView;

  @Override
  public void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);

    // Force landscape and no title for extra room
    setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);
    requestWindowFeature(Window.FEATURE_NO_TITLE);

    // If the user has never accepted the EULA show it again.
    setContentView(R.layout.main);
    mMainView = findViewById(R.id.main_view);
    mTarotView = (TarotView) findViewById(R.id.tarot);
    mTarotView.SetTextView((TextView) findViewById(R.id.text));

    //StartSolitaire(savedInstanceState);
  }

  // Entry point for starting the game.
  //public void StartSolitaire(Bundle savedInstanceState) {
  @Override
  public void onStart() {
    super.onStart();

    mTarotView.InitGame();
   // AboutSplashScreen();
  }

  // Force show the help if this is the first time played. Sadly no one reads
  // it anyways.
  private void AboutSplashScreen() {
	  mTarotView.DisplayAbout();
  }

  @Override
  public boolean onCreateOptionsMenu(Menu menu) {
    super.onCreateOptionsMenu(menu);
    menu.add(0, MENU_NEW_GAME, 0, R.string.menu_newgame);
    menu.add(0, MENU_ABOUT, 0, R.string.menu_about);
    menu.add(0, MENU_QUIT, 0, R.string.menu_quit);
    return true;
  }

  @Override
  public boolean onOptionsItemSelected(MenuItem item) {
    switch (item.getItemId()) {
      case MENU_NEW_GAME:
    	 // mTarotView.RestartGame();
        break;
      case MENU_ABOUT:
    	  mTarotView.DisplayAbout();
        break;
      case MENU_QUIT:
        finish();
        break;
    }

    return false;
  }

  @Override
  protected void onPause() {
    super.onPause();
  }

  @Override
  protected void onStop() {
    super.onStop();
  }

  @Override
  protected void onResume() {
    super.onResume();
  }

  @Override
  public void onSaveInstanceState(Bundle outState) {
    super.onSaveInstanceState(outState);
  }

}
