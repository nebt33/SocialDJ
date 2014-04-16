package socialdj;

import java.io.IOException;
import java.net.InetSocketAddress;
import java.net.Socket;
import java.net.SocketException;

public class ConnectedSocket {
  private static Socket socket = null;
  static String ipAddress = null;

  public ConnectedSocket() {
	  socket = new Socket();
	  ipAddress = "0.0.0.0";
  }
  
  public static Socket getSocket() {
	  return socket;
  }
  
  public void setConnectedIP(String ipAddress) {
	  this.ipAddress = ipAddress;
  }
  
  public static String getConnectedIP(){
	  return ipAddress;
  }
  
  public static void close() {
	  try {
		  socket.close();
	} catch (IOException e) {e.printStackTrace();}

  }

  public static void connect(InetSocketAddress inetSocketAddress, int timeout) {
	  try {
		socket.connect(inetSocketAddress, timeout);
	} catch (IOException e) {e.printStackTrace();}
  }

  public static boolean isConnected() {
	  return socket.isConnected();
  }
}
