/**
 * This file describes the UDP packet structure
 * @author Mainak Biswas
 */

package com.org.iitd.packetx;

public class UdpPacket extends Packet {
	private UDPHeader udphdr;
	/**
	 * construct a packet 
	 * @param iph IPv4 Header
	 * @param udph Udp header
	 * @param data data part
	 */
	public UdpPacket(IPHeader iph, UDPHeader udph, Data data){
		super(iph, data);
		udphdr = udph;
	}
	//required for jni simplification
	public UdpPacket(int tos, int id, int frag, int ttl, String sadd, String dadd, int sport, int dport, String data){
		super(new IPHeader(tos, id, frag, ttl, sadd, dadd), new Data(data));
		udphdr = new UDPHeader(sport, dport);
	}
	
	public UDPHeader getUdpHeader(){
		return udphdr;
	}
}
