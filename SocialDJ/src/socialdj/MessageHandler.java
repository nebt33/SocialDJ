package socialdj;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.List;

import socialdj.library.Song;

public class MessageHandler implements Runnable {

    boolean serverExist = false;
	String currentServer = null;
	String nonActiveIP = "0.0.0.0";
	
    //List for views
	static List<Song> songs = Collections.synchronizedList(new ArrayList<Song>());
		/*List<Artist> artists = Collections.synchronizedList(new ArrayList<Artist>());
		  List<Album> albums = Collections.synchronizedList(new ArrayList<Album>());*/
    //static List<QueueElement> queueElements = Collections.synchronizedList(new ArrayList<QueueElements>());
	
	public static List<Song> getSongs() {
		return songs;
	}
	
	@Override
	public void run() {
		if(!ConnectedSocket.getConnectedIP().equals(nonActiveIP))
			  serverExist = true;
		
		while(serverExist) {
	
			//read in data
			BufferedReader in = null;
			try {
				in = new BufferedReader( 
						new InputStreamReader(ConnectedSocket.getSocket().getInputStream()));

				String inputLine = null; 
				while ((inputLine = in.readLine()) != null) 
				{ 
					List<String> temp = Arrays.asList(inputLine.split("\\|"));
					String operation = temp.get(0);
					//---determine operation
					switch(operation) {
					case "download_starting":
						break;
					case "download_fail":
						break;
					case "download_success":
						break;
					case "new_song":
						break;
					case "song_info":
						songInfo(inputLine);
						break;
					case "forget_song":
						forgetSong(inputLine);
						break;
					case "new_album":
						break;
					case "album_info":
						break;
					case "forget_album":
						forgetAlbum(inputLine);
						break;
					case "new_artist":
						break;
					case "artist_info":
						break;
					case "forget_artist":
						forgetArtist(inputLine);
						break;
					case "add_bottom":
						addBottom(inputLine);
						break;
					case "remove_top":
						removeTop(inputLine);
						break;
					case "score":
						score(inputLine);
						break;
					case "playing":
						break;
					case "paused":
						break;
					case "skip":
						break;
					}


				}
			} catch (IOException e) {e.printStackTrace();}

			//server was changed
			if(!ConnectedSocket.getConnectedIP().equals(currentServer))
				serverExist = false;
		}
		
		//clear data
		songs.clear();
		/*
		 * artists.clear();
		 * albums.clear();
		 * queueElements.clear();
		 */
		
	}
	
	/**
	 * Remove song because it no longer exists.
	 * ->Queue and Cached list of songs
	 * @param inputLine
	 */
	public void forgetSong(String inputLine) {
		List<String> temp = Arrays.asList(inputLine.split("\\|"));
		String id = temp.get(1);
		//remove song from cache songs
		synchronized(songs) {
			for(Song s : songs) {
				if(Integer.parseInt(s.getSongId()) == Integer.parseInt(id)) {
					songs.remove(s);
					break;
				}
			}
		}
		
		//remove song from queue if in queue
		/*synchronized(queueElements) {
		    for(QueueElements q : queueElements) {
			  if(Integer.parseInt(q.getSongId()) == Integer.parseInt(id))
				songs.remove(q);
			}
		}*/
	}
	
	/**
	 * Remove album because it no longer exists
	 * -> Cached list of albums
	 * @param inputLine
	 */
	public void forgetAlbum(String inputLine) {
		List<String> temp = Arrays.asList(inputLine.split("\\|"));
		String id = temp.get(1);
		
		/*synchronized(albums) {
		    for(Album a: albums) {
			  if(Integer.parseInt(a.getAlbumId()) == Integer.parseInt(id)) {
				albums.remove(a);
				break;
			  }
		  }
		}*/
	}
	
	/**
	 * Remove artist because it no longer exists
	 * @param inputLine
	 */
	public void forgetArtist(String inputLine) {
		List<String> temp = Arrays.asList(inputLine.split("\\|"));
		String id = temp.get(1);
		
		/*synchronized (artists) {
			for(Artist a: artists) {
				if(Integer.parseInt(a.getArtistId()) == Integer.parseInt(id)) {
					artists.remove(a);
					break;
				}
			}
		}*/
	}
	
	/**
	 * Add a song to the queue
	 * @param inputLine
	 */
	public void addBottom(String inputLine) {
		List<String> temp = Arrays.asList(inputLine.split("\\|"));
		String id = temp.get(1);
		boolean inserted = true;
		
		//QueueElement q = new QueueElement();
		  //once queueElement is written, assign correct values-------------------------------------------

		//song will always be cached because server sends song information first
		/*synchronized (queueElements) {
		    for(QueueElement q: queueElements) {
		      if(Integer.parseInt(q.getSongId()) > Integer.parseInt(id)){
			   queueElements.add(q);
			   inserted = false;
			   break;
		      }
	        }
	      }*/
		
		/*if(!inserted) {
			synchronized (queueElements) {
				queueElementes.add(q);
			}
		}*/
	}
	
	/**
	 * Remove song that has finished playing from queue
	 * @param inputLine
	 */
	public void removeTop(String inputLine) {
		List<String> temp = Arrays.asList(inputLine.split("\\|"));
		String id = temp.get(1);
		
		/*synchronized (queueElements) {
		    for(QueueElement q: queueElements) {
			  if(Integer.parseInt(q.getSongId()) == Integer.parseInt(id)){
				queueElements.remove(q);
				break;
			  }
		  }
		}*/
	}
	
	/**
	 * Update score of song in the queue
	 * @param inputLine
	 */
	public void score(String inputLine) {
		List<String> temp = Arrays.asList(inputLine.split("\\|"));
		String id = temp.get(1);
		String score = temp.get(2);
		
		/*synchronized (queueElements) {
		    for(QueueElement q : queueElements) {
			  if(Integer.parseInt(q.getSongId()) == Integer.parseInt(id)) {
				q.setScore(score);
				break;
			  }
		  }
		}*/
	}
	
	public void songInfo(String inputLine) {
		//split inputLine up
		List<String> temp = Arrays.asList(inputLine.split("\\|"));
		Song song = new Song();
		String metaItem = "";
		for(String s : temp) {
			if(s.equalsIgnoreCase("song_id"))
				song.setSongId(s);
			else if(s.equalsIgnoreCase("album"))
				metaItem = s;
			else if(s.equalsIgnoreCase("artist"))
				metaItem = s;
			else if(s.equalsIgnoreCase("title"))
				metaItem = s;
			else if(s.equalsIgnoreCase("duration"))
				metaItem = s;
			else if(metaItem.equalsIgnoreCase("album"))
				song.setAlbumName(s);
			else if(metaItem.equalsIgnoreCase("artist"))
				song.setArtistName(s);
			else if(metaItem.equalsIgnoreCase("title"))
				song.setSongTitle(s);
			else if(metaItem.equalsIgnoreCase("duration"))
				song.setSongDuration(s);
		}
		//search list for song to enter into correct spot
		synchronized (songs) {
			Song previousSong = null;
			Song nextSong = null;
			boolean inserted = false;
			for(int i = 0; i < songs.size(); i++) {
				if(Integer.parseInt(song.getSongId()) > Integer.parseInt(songs.get(i).getSongId())) {
					previousSong = songs.get(i);
					songs.set(i,song);
					inserted = true;
				}
				//re order list based on id
				else if(inserted) {
					nextSong = songs.get(i);
					songs.set(i, previousSong);
				}
				else if(i < songs.size() - 1)
					songs.add(song);
			}
		}
	}
}
