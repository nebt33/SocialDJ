package socialdj;

import java.io.IOException;
import java.io.PrintWriter;
import java.io.Serializable;
import java.util.ArrayList;


/**
 * Message sender to server
 * song database:
		download_song|url
		list_songs|[|metaitem|value]|start|count
		list_albums|[|metaitem|value]|start|count
		list_artists|[|metaitem|value]|start|count
		delete_song|song_id
	queue:
		queue_add|song_id
	vote|song_id|value   //value +1, -1
	playback:
	play
	pause
	skip
	
	metaitems are one of: artist, album, title
 * @author Nathan
 *
 */
public class SendMessage implements Runnable {
	private String downloadSong = "download_song";
	private String listSongs = "list_songs";
	private String listAlbums = "list_albums";
	private String listArtists = "list_artists";
	private String deleteSong = "delete_song";
	private String addSong = "queue_add";
	private String vote = "vote";
	private String play = "play";
	private String pause = "pause";
	private String skip = "skip";
	private String message;
	
	public void prepareMessageDownloadSong(String url) {
		message = (downloadSong + "|" + url);
	}
	
	public void prepareMessageListSongs(ArrayList<MetaItem> metaItems, String start, String count) {
		message = (listSongs + "|");
		for(MetaItem m: metaItems)
			message += (m.getMetaItem() + "|" + m.getValue() + "|");
		message += (start + "|" + count);
	}
	
	public void prepareMessageListAlbums(ArrayList<MetaItem> metaItems, String start, String count) {
		message = (listAlbums + "|");
		for(MetaItem m: metaItems)
			message += (m.getMetaItem() + "|" + m.getValue() + "|");
		message += (start + "|" + count);
	}
	
	public void prepareMesssageListArtists(ArrayList<MetaItem> metaItems, String start, String count) {
		message = (listArtists + "|");
		for(MetaItem m: metaItems)
			message += (m.getMetaItem() + "|" + m.getValue() + "|");
		message += (start + "|" + count);
	}
	
	public void prepareMessageDeleteSong(String id) {
		message = (deleteSong + "|" + id);
	}
	
	public void prepareMessageAddSong(String id) {
		message = (addSong + "|" + id);
	}
	
	public void prepareMessageVote(String id, String value) {
		message = (vote + "|" + id + "|" + value);
	}
	
	public void prepareMessagePlay() {
		message = (play);
	}
	
	public void prepareMessagePause() {
		message = (pause);
	}
	
	public void prepareMessageSkip() {
		message = (skip);
	}

	@Override
	public void run() {
		PrintWriter out;
		try {
			out = new PrintWriter(ConnectedSocket.getSocket().getOutputStream());
			out.write(message+"\n");
			out.flush();
		} catch (IOException e) {e.printStackTrace();}
	}
}
