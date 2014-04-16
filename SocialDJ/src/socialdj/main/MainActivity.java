package socialdj.main;

import java.net.InetSocketAddress;
import java.util.ArrayList;

import socialdj.ConnectedSocket;
import socialdj.MessageHandler;
import socialdj.NavDrawerItem;
import socialdj.NavDrawerListAdapter;
import socialdj.config.R;
import socialdj.connect.ConnectActivity;
import socialdj.library.LibraryFragmentActivity;
import socialdj.queue.QueueFragment;
import android.app.Fragment;
import android.app.FragmentManager;
import android.content.Intent;
import android.content.SharedPreferences;
import android.content.res.Configuration;
import android.content.res.TypedArray;
import android.os.AsyncTask;
import android.os.Bundle;
import android.support.v4.app.ActionBarDrawerToggle;
import android.support.v4.app.FragmentActivity;
import android.support.v4.widget.DrawerLayout;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ListView;
import android.widget.Toast;

/**
 * MainActivity that will start up when application is called.  
 *   -holds navigation drawer
 *   -automatically tries to connect to most recent server that application
 *     was priorly connected to.
 * @author Nathan
 *
 */
public class MainActivity extends FragmentActivity {
    private DrawerLayout mDrawerLayout;
    private ListView mDrawerList;
    private ActionBarDrawerToggle mDrawerToggle;
 
    // nav drawer title
    private CharSequence mDrawerTitle;
 
    // used to store app title
    private CharSequence mTitle;
 
    // slide menu items
    private String[] navMenuTitles;
    private TypedArray navMenuIcons;
 
    private ArrayList<NavDrawerItem> navDrawerItems;
    private NavDrawerListAdapter adapter;
    
  //standard port for socket
  	private static int standardPort = 8888;
  	//standard port for not active ip address on network
  	private static String nonActiveIp = "0.0.0.0";
 
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        
        //connect to saved server on startup
        ConnectTask connect = new ConnectTask();
		SharedPreferences settings = getSharedPreferences("connected", MODE_PRIVATE);
		connect.execute(settings.getString("currentlyConnected", nonActiveIp));
 
        mTitle = mDrawerTitle = getTitle();
 
        // load slide menu items
        navMenuTitles = getResources().getStringArray(R.array.nav_drawer_items);
 
        // nav drawer icons from resources
        navMenuIcons = getResources()
                .obtainTypedArray(R.array.nav_drawer_icons);
 
        mDrawerLayout = (DrawerLayout) findViewById(R.id.drawer_layout);
        mDrawerList = (ListView) findViewById(R.id.list_slidermenu);
 
        navDrawerItems = new ArrayList<NavDrawerItem>();
 
        // adding nav drawer items to array
        // Queue
        navDrawerItems.add(new NavDrawerItem(navMenuTitles[0], navMenuIcons.getResourceId(0, -1)));
        // Library
        navDrawerItems.add(new NavDrawerItem(navMenuTitles[1], navMenuIcons.getResourceId(1, -1)));
        // Connect
        navDrawerItems.add(new NavDrawerItem(navMenuTitles[2], navMenuIcons.getResourceId(2, -1)));
         
 
        // Recycle the typed array
        navMenuIcons.recycle();
 
        mDrawerList.setOnItemClickListener(new SlideMenuClickListener());
 
        // setting the nav drawer list adapter
        adapter = new NavDrawerListAdapter(getApplicationContext(),
                navDrawerItems);
        mDrawerList.setAdapter(adapter);
 
        // enabling action bar app icon and behaving it as toggle button
        getActionBar().setDisplayHomeAsUpEnabled(true);
        getActionBar().setHomeButtonEnabled(true);
 
        mDrawerToggle = new ActionBarDrawerToggle(this, mDrawerLayout,
                R.drawable.ic_drawer, //nav menu toggle icon
                R.string.app_name, // nav drawer open - description for accessibility
                R.string.app_name // nav drawer close - description for accessibility
        ) {
            public void onDrawerClosed(View view) {
                getActionBar().setTitle(mTitle);
                // calling onPrepareOptionsMenu() to show action bar icons
                invalidateOptionsMenu();
            }
 
            public void onDrawerOpened(View drawerView) {
                getActionBar().setTitle(mDrawerTitle);
                // calling onPrepareOptionsMenu() to hide action bar icons
                invalidateOptionsMenu();
            }
        };
        mDrawerLayout.setDrawerListener(mDrawerToggle);
 
        if (savedInstanceState == null) {
            // on first time display view for first nav item
            displayView(0);
        }
    }
 
    /**
     * Slide menu item click listener
     * */
    private class SlideMenuClickListener implements
            ListView.OnItemClickListener {
        @Override
        public void onItemClick(AdapterView<?> parent, View view, int position,
                long id) {
            // display view for selected nav drawer item
            displayView(position);
        }
    }
 
    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // toggle nav drawer on selecting action bar app icon/title
        if (mDrawerToggle.onOptionsItemSelected(item)) {
            return true;
        }
        // Handle action bar actions click
        switch (item.getItemId()) {
        case R.id.action_settings:
            return true;
        default:
            return super.onOptionsItemSelected(item);
        }
    }
 
    /***
     * Called when invalidateOptionsMenu() is triggered
     */
    @Override
    public boolean onPrepareOptionsMenu(Menu menu) {
        // if nav drawer is opened, hide the action items
        boolean drawerOpen = mDrawerLayout.isDrawerOpen(mDrawerList);
        //menu.findItem(R.id.action_settings).setVisible(!drawerOpen);
        return super.onPrepareOptionsMenu(menu);
    }
 
    /**
     * Diplaying fragment view for selected nav drawer list item
     * */
    private void displayView(int position) {
        // update the main content by replacing fragments
        Fragment fragment = null;
        FragmentManager fragmentManager = getFragmentManager();
        switch (position) {
        case 0:
            fragment = new QueueFragment();
            fragmentManager.popBackStack();
            break;
        case 1:
        	Intent intent = new Intent(getApplicationContext(), LibraryFragmentActivity.class);
        	startActivity(intent);
            break;
        case 2:
        	intent = new Intent(getApplicationContext(), ConnectActivity.class);
        	startActivity(intent);
        	break;
 
        default:
            break;
        }
 
        if (fragment != null) {
            /*FragmentManager fragmentManager = getFragmentManager();
            fragmentManager.beginTransaction()
                    .replace(R.id.frame_container, fragment).commit();*/
            fragmentManager.beginTransaction().add(R.id.frame_container, fragment).addToBackStack("fragback").commit();

            // update selected item and title, then close the drawer
            mDrawerList.setItemChecked(position, true);
            mDrawerList.setSelection(position);
            setTitle(navMenuTitles[position]);
            mDrawerLayout.closeDrawer(mDrawerList);
        } else {
            // error in creating fragment
            Log.e("MainActivity", "Error in creating fragment");
        }
    }
 
    @Override
    public void setTitle(CharSequence title) {
        mTitle = title;
        getActionBar().setTitle(mTitle);
    }
 
    /**
     * When using the ActionBarDrawerToggle, you must call it during
     * onPostCreate() and onConfigurationChanged()...
     */
 
    @Override
    protected void onPostCreate(Bundle savedInstanceState) {
        super.onPostCreate(savedInstanceState);
        // Sync the toggle state after onRestoreInstanceState has occurred.
        mDrawerToggle.syncState();
    }
 
    @Override
    public void onConfigurationChanged(Configuration newConfig) {
        super.onConfigurationChanged(newConfig);
        // Pass any configuration change to the drawer toggls
        mDrawerToggle.onConfigurationChanged(newConfig);
    }
    
    /**
	 * Used to connect to previous server remembered by application if possible on create
	 * @author Nathan
	 */
	public class ConnectTask extends AsyncTask<String, String, String> {
		boolean connected = false;
		String ipAddress = nonActiveIp;
		
		@SuppressWarnings("static-access")
		@Override
		protected String doInBackground(String... params) {
			//tries to connect to server associated with radio button
			//Socket socket = null;
			ConnectedSocket socket = new ConnectedSocket();
			if(!(params[0].equals(nonActiveIp))){
				try {
					System.out.println("Saved server :" + params[0]);
					socket.connect(new InetSocketAddress(params[0].trim(), standardPort), 2000);
				} catch (Exception e) {
					Log.v("ConvertView", String.valueOf("No server by that ip can be connected to at start of application"));
				}
				if(socket.isConnected()) {
					ipAddress = params[0].trim();
					socket.setConnectedIP(ipAddress);
					connected = true;
				}
				else {
					//call connect class to connect to a server
					Intent intent = new Intent(getApplicationContext(), ConnectActivity.class);
		        	startActivity(intent);
				}
			}
			return null;
		}
		
		@Override
		  protected void onPostExecute(String result) {
			if(connected) {
				Toast.makeText(getApplicationContext(), "Connected to Server: " + ipAddress, Toast.LENGTH_SHORT).show();

				//call new handler for server
				MessageHandler task = new MessageHandler();
				new Thread(task).start();
		    }
		  }
		
	}

 
}