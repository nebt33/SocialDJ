package socialdj.library;

public class SongRow {
	private String songTitle;
	private String artistName;
	private String songDuration;
	
	public void setSongTitle(String songTitle) {
		this.songTitle = songTitle;
	}
	
	public void setArtistName(String artistName) {
		this.artistName = artistName;
	}
	
	public void setSongDuration(String songDuration) {
		this.songDuration = songDuration;
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
}
