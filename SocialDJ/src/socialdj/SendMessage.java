package socialdj;

import java.io.Serializable;


/**
 * Message sender to server
 * Add song to Queue      - (add, song_title, artist_name)
 * Remove song from Queue - (remove, song_title, artist_name)
 * downVote               - (downVote, song_title, artist_name)
 * upVote                 - (upVote, song_title, artist_name)
 * Play music             - (play, empty String, empty String)
 * Pause music            - (pause, empty String, empty String)
 * Skip music             - (skip, empty String, empty String)
 * @author Nathan
 *
 */
public class SendMessage implements Serializable {
	private String operation;
	private String song;
	private String artist;
	
	public SendMessage(String operation, String song, String artist){
		this.operation = operation;
		this.song = song;
		this.artist = artist;
	}
	
	public String getOperation(){
		return operation;
	}
	
	public String getSong(){
		return song;
	}
	
	public String getArtist(){
		return artist;
	}
}
