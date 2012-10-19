/**
 * This file describes the generalised packet superclass
 * @author Mainak Biswas
 */

package com.org.iitd.packetx;

public abstract class Packet {
	private IPHeader iphdr;
	private Data data;
	/**
	 * constructs the ip header and data part of the packet
	 * @param iph IP header for the packet
	 * @param dat Data for the packet
	 */
	public Packet(IPHeader iph, Data dat){
		iphdr = iph;
		data = dat;
	}
	
	public IPHeader getIPHeader(){
		return iphdr;
	}
	
	public Data getData(){
		return data;
	}
}
