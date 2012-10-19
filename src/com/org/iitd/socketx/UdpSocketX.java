/**
 * This file describes the UDP socket structure
 * @author Mainak Biswas
 */
 
package com.org.iitd.socketx;

import com.org.iitd.packetx.Data;
import com.org.iitd.packetx.IPHeader;
import com.org.iitd.packetx.UDPHeader;
import com.org.iitd.packetx.UdpPacket;

public class UdpSocketX {
	private IPHeader iphdr;
	private UDPHeader udphdr;
	private boolean preset;
	/**
	 * create a raw UDP socket to send out raw udp packets 
	 * as well as sniff udp packets reaching machine. 
	 */
	public UdpSocketX(){
		preset = false;
	}
	/**
	 * Create a UDP socket with preset values.
	 * @param iph
	 * @param udph
	 */
	public UdpSocketX(IPHeader iph, UDPHeader udph){
		preset = true;
		iphdr = iph;
		udphdr = udph;
	}
	
	//to be defined
	private native int send_udp_packet(int tos, int id, int frag, int ttl, String sadd, String dadd, int sport, int dport, String data);
	
	/**
	 * send a fully defined packet
	 * @param packet packet to send
	 * @return send status
	 */
	public int sendPacket(UdpPacket packet){
		return send_udp_packet(packet.getIPHeader().getTos(),
				packet.getIPHeader().getId(),
				packet.getIPHeader().getFragOff(),
				packet.getIPHeader().getTtl(),
				packet.getIPHeader().getSourceIp(),
				packet.getIPHeader().getDestIp(),
				packet.getUdpHeader().getSourcePort(),
				packet.getUdpHeader().getDestPort(),
				packet.getData().getContent());
	}
	
	/**
	 * Send data for a udp socket with preset values 
	 * @param data data to send, Are ip header and udp header defined? dangerous
	 * @return send status(0 for success)
	 */
	public int sendData(Data data){
		iphdr.setId( iphdr.getId() + 1 );
		UdpPacket packet = new UdpPacket(iphdr, udphdr, data);
		return sendPacket(packet);
	}
	
	//native method to get udp packet
	private native UdpPacket get_udp_packet();
	/**
	 * Return any udp packet that reaches machine, blocking call
	 * Used for sniffing network.
	 * @return a UDP packet captured
	 */
	public UdpPacket getUdpPacket(){
		//form a udp packet by calling get_udp_packet and return;
		UdpPacket upack = get_udp_packet();
		return upack;
	}
	
	/**
	 * To be called only when initialised with the predefined values,blocking call
	 * @return a udp packet with corresponding headers
	 */
	public UdpPacket getReplyUdpPacket(){
		UdpPacket upack;
		while(true){
			upack = getUdpPacket();
			if(upack.getIPHeader().getDestIp().equals(iphdr.getSourceIp()) //sender ip equals dest ip of rcvd packet
					&& upack.getIPHeader().getSourceIp().equals(iphdr.getDestIp())
					&& upack.getUdpHeader().getDestPort() == udphdr.getSourcePort()
					&& upack.getUdpHeader().getSourcePort() == udphdr.getDestPort())
			{
				return upack;
			}
		}
	}
	
	/**
	 * tells whether ipv4 header and and udp header is initialized and 
	 * sendData and getReplyPaket function can be used safely.
	 * @return true if properly setup with right values
	 */
	public boolean isPreset(){
		return preset;
	}
	
	public IPHeader getPresetIpHeader(){
		return iphdr;
	}
	
	public UDPHeader getPresetUdpHeader(){
		return udphdr;
	}
}
