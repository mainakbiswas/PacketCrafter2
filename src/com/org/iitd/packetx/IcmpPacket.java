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
	
	public IcmpPacket(int tos, int id, int frag, int ttl, String sadd, String dadd, int type, int code, int hdr1, int hdr2, String data){
		super(new IPHeader(tos, id, frag, ttl, sadd, dadd), new Data(data));
		icmphdr = new ICMPHeader(type, code, hdr1, hdr2);
	}
}
