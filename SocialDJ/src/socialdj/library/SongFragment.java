package socialdj.library;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

import socialdj.ConnectedSocket;
import socialdj.SendMessage;
import socialdj.config.R;
import android.app.Activity;
import android.os.AsyncTask;
import android.os.Bundle;
import android.support.v4.app.ListFragment;
import android.view.LayoutInflater;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.ViewGroup;
import android.widget.AbsListView;
import android.widget.AbsListView.OnScrollListener;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.TextView;

/**
 * Fragment for songs.  Show all songs database holds.
 * @author Nathan
 *
 */
public class SongFragment extends ListFragment {

	private CustomSongAdapter adapter = null;
	//size of what list will be
	private int totalSizeToBe = 0;
	//if the data is still sending
	boolean isLoading = false;
	//size of next amount of songs
	private static final int BLOCK_SIZE = 100;
	//starts thread to load next amount of data 
	private static final int LOAD_AHEAD_SIZE = 50;
	private static final int INCREMENT_TOTAL_MINIMUM_SIZE = 100;
	private static final String PROP_TOP_ITEM = "top_list_item";


	@Override
	public void onActivityCreated(Bundle savedInstanceState) {
		super.onActivityCreated(savedInstanceState);

		//Populate list
		adapter = new CustomSongAdapter(getActivity(), R.layout.all_songs, new ArrayList<SongRow>());

		//asynchronously initial list
		GetSongTask task = new GetSongTask();
		//task.execute(new Integer[] {0, BLOCK_SIZE});

		setListAdapter(adapter);
		getListView().setOnScrollListener(new SongListScrollListener());

		if(savedInstanceState != null) {
			//Restore last state from top list position
			int listTopPosition = savedInstanceState.getInt(PROP_TOP_ITEM, 0);

			//load elements to get to the top of the list
			task = new GetSongTask();
			if(listTopPosition > BLOCK_SIZE) {
				//download asynchronously inital list
				task.execute(new Integer[] {BLOCK_SIZE, listTopPosition + BLOCK_SIZE, listTopPosition});
			}
		}
	}

	@Override
	public void onSaveInstanceState(Bundle state) {
		super.onSaveInstanceState(state);
		int listPosition = getListView().getFirstVisiblePosition();
		if (listPosition > 0) {
			state.putInt(PROP_TOP_ITEM, listPosition);
		}
	}

	/**
	 * Listener which handles the endless list.  It is responsible for
	 * determining when the network calls will be asynchronously.
	 */
	class SongListScrollListener implements OnScrollListener {

		@Override
		public void onScroll(AbsListView view, int firstVisibleItem,
				int visibleItemCount, int totalItemCount) {
			//load more elements if there is LOAD_AHEAD_SIZE left in the list being displayed
			boolean loadMore = firstVisibleItem + visibleItemCount >= totalItemCount - LOAD_AHEAD_SIZE;

			/*
			 * Only get more results if the list achieves a min size. This is to avoid
			 * that this method is called each time the loadMore is reached and scroll
			 * pressed
			 */
			if(loadMore && totalSizeToBe <= totalItemCount) {
				totalSizeToBe += INCREMENT_TOTAL_MINIMUM_SIZE;
				//calls more elements
				GetSongTask task = new GetSongTask();
				task.execute(new Integer[] {totalItemCount, BLOCK_SIZE+totalItemCount});
			}
		}

		@Override
		public void onScrollStateChanged(AbsListView view, int scrollState){

		}
	}

	/**
	 * Asynchronous call.  This class is responsible for calling the network for more songs
	 * and managing the isLoading boolean.
	 */
	class GetSongTask extends AsyncTask<Integer, Void, List<SongRow>> {
		private static final int TOP_ITEM_INDEX = 2;

		//position to scroll list to
		private int listTopPosition = 0;

		@Override 
		public void onPreExecute() {
			isLoading = true;
		}

		@Override
		protected List<SongRow> doInBackground(Integer... params) {
			List<SongRow> results = new ArrayList<SongRow>();

			if(params.length > TOP_ITEM_INDEX)
				listTopPosition = params[TOP_ITEM_INDEX];

			//excute network call
			PrintWriter out = null;
			try {
				out = new PrintWriter(ConnectedSocket.getSocket().getOutputStream());
			} catch (IOException e) {e.printStackTrace();}
			out.write("ALLSONGS|" + params[0] + "|" + params[1] + "\n");
			out.flush();
			BufferedReader in = null;
			try {
				in = new BufferedReader( 
						new InputStreamReader(ConnectedSocket.getSocket().getInputStream()));

				String inputLine; 
				while ((inputLine = in.readLine()) != null) 
				{ 
					if(inputLine.equalsIgnoreCase("ENDOPERATION"))
						break;
					List<String> temp = Arrays.asList(inputLine.split("\\|"));

					SongRow row = new SongRow();
					row.setSongTitle(temp.get(0));
					row.setArtistName(temp.get(1));
					row.setSongDuration(temp.get(2));
					results.add(row);
				}
			} catch (IOException e) {e.printStackTrace();}

			return results;
		}

		@Override
		protected void onPostExecute(List<SongRow> result) {
			adapter.setNotifyOnChange(true);
			for(SongRow item: result) {
				//check if item was added
				synchronized(adapter) {
					if(!adapter.contains(item)) {
						adapter.add(item);
					}
				}
			}

			//loading is done
			isLoading = false;

			//update top list item
			if(listTopPosition > 0) {
				getListView().setSelection(listTopPosition);
			}
		}
	}

	/**
	 * Adapter which handles the list to be displayed
	 * @author Nathan
	 *
	 */
	class CustomSongAdapter extends ArrayAdapter<SongRow> {
		private final Activity context;
		private final List<SongRow> items;
		private final int rowViewId;

		public CustomSongAdapter(Activity context, int rowViewId, List<SongRow> items) {
			super(context, rowViewId, items);
			this.context = context;
			this.items = items;
			this.rowViewId = rowViewId;
		}

		public boolean contains(SongRow item) {
			for(SongRow r: items){
				if(r.getSongTitle().equals(item.getSongTitle()) && r.getArtistName().equals(item.getArtistName())) {
					return true;
				}
			}
			return false;
		}

		public SongRow getItemAt(int index) {
			return items.get(index);
		}

		@Override 
		public View getView(int position, View convertView, ViewGroup parent) {
			TextView songTitle;
			TextView artistName;
			TextView songDuration;
			Button addQButton;

			View rowView = convertView;

			/*
			 * Inflate row using the layout.
			 */
			LayoutInflater inflater = context.getLayoutInflater();
			rowView = inflater.inflate(rowViewId, null, true);
			songTitle = (TextView) rowView.findViewById(R.id.songTitle);
			artistName =  (TextView) rowView.findViewById(R.id.artistName);
			songDuration = (TextView) rowView.findViewById(R.id.songDuration);
			addQButton = (Button) rowView.findViewById(R.id.AddQButton);

			songTitle.setText(items.get(position).getSongTitle());
			artistName.setText(items.get(position).getArtistName());
			songDuration.setText(items.get(position).getSongDuration());

			final int currentlyClicked = position;
			addQButton.setOnClickListener(new OnClickListener() {

				@Override
				public void onClick(View v) {
					Thread thread = new Thread() {
						@Override
						public void run() {
							SendMessage message = new SendMessage("add", getItem(currentlyClicked).getSongTitle(), getItem(currentlyClicked).getArtistName());
							try {
								PrintWriter out = new PrintWriter(ConnectedSocket.getSocket().getOutputStream());
								out.write(message.prepareMessage()+"\n");
								out.flush();
							} catch (IOException e) {e.printStackTrace();}

						}
					};
					thread.start();
				}

			});

			return rowView;
		}

	}
}
