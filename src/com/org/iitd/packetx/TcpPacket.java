/**
 * This file describes the ICMP packet structure
 * @author Mainak Biswas
 */

package com.org.iitd.packetx;

public class TcpPacket extends Packet {
	private TCPHeader tcphdr;
	/**
	 * 
	 * @param iph IPv4 Header
	 * @param tcph TCP header
	 * @param data Data part
	 */
	public TcpPacket(IPHeader iph, TCPHeader tcph, Data data){
		super(iph, data);
		tcphdr = tcph;
	}
	
	public TCPHeader getTcpHeader(){
		return tcphdr;
	}
}
