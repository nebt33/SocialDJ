package socialdj.queue;

import socialdj.config.R;

import android.os.Bundle;
import android.app.Fragment;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;

/**
 * Fragment of the queue.  
 * @author Nathan
 *
 */
public class QueueFragment extends Fragment {

	@Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
            Bundle savedInstanceState) {
 
        View rootView = inflater.inflate(R.layout.queue, container, false);
         
        return rootView;
    }
	
	//look into addFooterView for the fixed view at bottom of Q
}
