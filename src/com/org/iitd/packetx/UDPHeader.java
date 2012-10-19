/**
 * This file describes the UDP header structure
 * @author Mainak Biswas
 */

package com.org.iitd.packetx;

public class UDPHeader {
	private int sport;
	private int dport;
	
	/**
	 * Create UDP header
	 * @param sport source port
	 * @param dport destination port
	 */
	public UDPHeader(int sport, int dport){
		this.sport = sport;
		this.dport = dport;
	}
	
	public int getSourcePort(){
		return sport;
	}
	
	public int getDestPort(){
		return dport;
	}
	
	public void setSourcePort(int port){
		this.sport = port;
	}
	
	public void setDestPort(int port){
		this.dport = port;
	}
}
