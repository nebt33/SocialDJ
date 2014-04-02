package socialdj.library;

import socialdj.config.R;

import android.os.Bundle;
import android.support.v4.app.Fragment;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;

/**
 * Fragment for songs.  Show all songs database holds.
 * @author Nathan
 *
 */
public class SongFragment extends Fragment {

	@Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
            Bundle savedInstanceState) {
 
        View rootView = inflater.inflate(R.layout.queue, container, false);
         
        return rootView;
    }
}
