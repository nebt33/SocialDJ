package socialdj.library;

import java.util.ArrayList;

import socialdj.config.R;
import android.os.Bundle;
import android.support.v4.app.Fragment;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ListView;

/**
 * Fragment for songs.  Show all songs database holds.
 * @author Nathan
 *
 */
public class SongFragment extends Fragment {

	@Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
            Bundle savedInstanceState) {
		
		ArrayList<SongRow> allSongs = getAllSongs();

		View rootView = inflater.inflate(R.layout.song_main, container, false);
        
        ListView lv = (ListView)rootView.findViewById(R.id.songListView);
        lv.setAdapter(new CustomSongAdapter(getActivity(), allSongs ));
         
        return rootView;
    }
	
	public ArrayList<SongRow> getAllSongs() {
		//test function for now
		ArrayList<SongRow> allSongs = new ArrayList<SongRow>();

		for(int i = 0; i < 30; i++) {
			SongRow row = new SongRow();
			row.setSongTitle(Integer.toString(i));
			row.setArtistName(Integer.toString(i));
			allSongs.add(row);
		}
		return allSongs;
	}
}
