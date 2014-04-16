package socialdj;

/**
 * Simple class that will be used throughout android to send metaItems and their values
 * to the server.  Created just for simplicity sake and reusing class over and over instead of
 * duplicating code.
 * @author Nathan
 *
 */
public class MetaItem {
  String metaItem;
  String value;
  
  public void setMetaItem(String metaItem) {
	  this.metaItem = metaItem;
  }
  
  public void setValue(String value) {
	  this.value = value;
  }
  
  public String getMetaItem() {
	  return metaItem;
  }
  
  public String getValue() {
	  return value;
  }
}
