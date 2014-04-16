package socialdj.library;

public class Song {
	private String id;
	private String songTitle;
	private String artistName;
	private String albumName;
	private String songDuration;
	
	
	public void setSongTitle(String songTitle) {
		this.songTitle = songTitle;
	}
	
	public void setAlbumName(String albumName) {
		this.albumName = albumName;
	}
	
	public void setArtistName(String artistName) {
		this.artistName = artistName;
	}
	
	public void setSongDuration(String songDuration) {
		this.songDuration = songDuration;
	}
	
	public void setSongId(String id) {
		this.id = id;
	}
	
	public String getAlbumName() {
		return albumName;
	}
	
	public String getSongTitle() {
		return songTitle;
	}
	
	public String getArtistName() {
		return artistName;
	}
	
	public String getSongDuration() {
		return songDuration;
	}
	
	public String getSongId() {
		return id;
	}
}
