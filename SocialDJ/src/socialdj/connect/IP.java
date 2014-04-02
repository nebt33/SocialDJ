package socialdj.connect;

/**
 * Instance of ip addresses and if application is currently connected 
 * to an ip address.
 * @author Nathan
 *
 */
public class IP {
	String address = null;
	boolean selected = false;
	
	public IP(String address, boolean selected){
		super();
		this.address = address;
		this.selected = selected;
	}
	
	public String getAddress() {
		return address;
	}
	
	public boolean isSelected() {
		return selected;
	}
	
	public void setSelected(boolean selected) {
		this.selected = selected;
	}
}


