package socialdj.library;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.ObjectOutputStream;
import java.util.ArrayList;

import socialdj.ConnectedSocket;
import socialdj.SendMessage;
import socialdj.config.R;
import android.content.Context;
import android.database.DataSetObserver;
import android.view.LayoutInflater;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.ListAdapter;
import android.widget.TextView;

public class CustomSongAdapter implements ListAdapter {

	private static ArrayList<SongRow> allSongs;

	private LayoutInflater mInflater;

	public CustomSongAdapter(Context context, ArrayList<SongRow> row) {
		allSongs = row;
		mInflater = LayoutInflater.from(context);
	}

	public int getCount() {
		return allSongs.size();
	}

	public SongRow getItem(int position) {
		return allSongs.get(position);
	}

	public long getItemId(int position) {
		return position;
	}

	public View getView(int position, View convertView, ViewGroup parent) {
		ViewHolder holder;
		if (convertView == null) {
			convertView = mInflater.inflate(R.layout.all_songs, null);
			holder = new ViewHolder();
			holder.songTitle = (TextView) convertView.findViewById(R.id.songTitle);
			holder.artistName = (TextView) convertView
					.findViewById(R.id.artistName);
			holder.songDuration = (TextView) convertView.findViewById(R.id.songDuration);
			holder.addQButton = (Button) convertView.findViewById(R.id.AddQButton);

			convertView.setTag(holder);
		} else {
			holder = (ViewHolder) convertView.getTag();
		}

		holder.songTitle.setText(allSongs.get(position).getSongTitle());
		holder.artistName.setText(allSongs.get(position).getArtistName());
		holder.songDuration.setText(allSongs.get(position).getSongDuration());
		
		final int currentlyClicked = position;

		
		holder.addQButton.setOnClickListener(new OnClickListener() {

			@Override
			public void onClick(View v) {
				Thread thread = new Thread() {
					@Override
					public void run() {
						System.out.println("Clicked Button: " + getItem(currentlyClicked).getSongTitle());
						SendMessage message = new SendMessage("add", getItem(currentlyClicked).getSongTitle(), getItem(currentlyClicked).getArtistName());
						try {
							ObjectOutputStream out = new ObjectOutputStream(ConnectedSocket.getSocket().getOutputStream());
							//out.writeObject(message);
							//out.close();
							
							ByteArrayOutputStream bos = new ByteArrayOutputStream();
							out = new ObjectOutputStream(bos);
							out.writeObject(message);
							out.close();
						} catch (IOException e) {e.printStackTrace();}
						
					}
				};
				thread.start();
			}
			
		});
		
		

		return convertView;
	}

	static class ViewHolder {
		TextView songTitle;
		TextView artistName;
		TextView songDuration;
		Button addQButton;
	}

	@Override
	public int getItemViewType(int arg0) {
		// TODO Auto-generated method stub
		return 0;
	}

	@Override
	public int getViewTypeCount() {
		// TODO Auto-generated method stub
		return 1;
	}

	@Override
	public boolean hasStableIds() {
		// TODO Auto-generated method stub
		return false;
	}

	@Override
	public boolean isEmpty() {
		// TODO Auto-generated method stub
		return false;
	}

	@Override
	public void registerDataSetObserver(DataSetObserver arg0) {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void unregisterDataSetObserver(DataSetObserver arg0) {
		// TODO Auto-generated method stub
		
	}

	@Override
	public boolean areAllItemsEnabled() {
		// TODO Auto-generated method stub
		return false;
	}

	@Override
	public boolean isEnabled(int arg0) {
		// TODO Auto-generated method stub
		return false;
	}
}
