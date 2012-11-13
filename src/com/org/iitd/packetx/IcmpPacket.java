/**
 * This file describes the ICMP packet structure
 * @author Mainak Biswas
 */

package com.org.iitd.packetx;

public class IcmpPacket extends Packet {
	private ICMPHeader icmphdr;
	/**
	 * construct a packet 
	 * @param iph IPv4 Header
	 * @param icmph ICMP header
	 * @param data data part
	 */
	public IcmpPacket(IPHeader iph, ICMPHeader icmph, Data data){
		super(iph, data);
		icmphdr = icmph;
	}
	
	public ICMPHeader getIcmpHeader(){
		return icmphdr;
	}
}
