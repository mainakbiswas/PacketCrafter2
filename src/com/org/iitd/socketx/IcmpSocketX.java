package com.org.iitd.socketx;

import com.org.iitd.packetx.Data;
import com.org.iitd.packetx.ICMPHeader;
import com.org.iitd.packetx.IPHeader;
import com.org.iitd.packetx.IcmpPacket;

public class IcmpSocketX {
	private IPHeader iphdr;
	private ICMPHeader icmphdr;
	private boolean preset;
	
	/**
	 * create a raw ICMP socket to send out raw udp packets 
	 * as well as sniff icmp packets reaching machine. 
	 */
	public IcmpSocketX(){
		preset = false;
	}
	
	/**
	 * Create a ICMP socket with preset values. Can be used to send raw packets as well
	 * @param iph The IP header 
	 * @param icmph The ICMP header
	 */
	public IcmpSocketX(IPHeader iph, ICMPHeader icmph){
		preset = true;
		iphdr = iph;
		icmphdr = icmph;
	}
	
	private native int send_icmp_packet(int tos, int id, int frag, int ttl, String sadd, String dadd, int type, int code, int head1, int head2, String data);
	
	/**
	 * send a fully defined packet
	 * @param packet packet to send
	 * @return send status
	 */
	public int sendPacket(IcmpPacket packet){
		if(!preset){
			iphdr = packet.getIPHeader();
			icmphdr = packet.getIcmpHeader();
		}
		return send_icmp_packet(packet.getIPHeader().getTos(),
				packet.getIPHeader().getId(),
				packet.getIPHeader().getFragOff(),
				packet.getIPHeader().getTtl(),
				packet.getIPHeader().getSourceIp(),
				packet.getIPHeader().getDestIp(),
				packet.getIcmpHeader().getType(),
				packet.getIcmpHeader().getcode(),
				packet.getIcmpHeader().getHeader1(),
				packet.getIcmpHeader().getHeader2(),
				packet.getData().getContent());
	}
	
	/**
	 * Send data for a icmp socket with preset values 
	 * @param data data to send, Are ip header and icmp header defined? dangerous
	 * @return send status(0 for success)
	 */
	public int sendData(Data data){
		iphdr.setId( iphdr.getId() + 1 );
		IcmpPacket packet = new IcmpPacket(iphdr, icmphdr, data);
		return sendPacket(packet);
	}
	
	private native IcmpPacket get_icmp_packet();
	
	/**
	 * Return any icmp packet that reaches machine, blocking call
	 * Used for sniffing network.
	 * @return a ICMP packet captured
	 */
	public IcmpPacket getIcmpPacket(){
		//form a icmp packet by calling get_icmp_packet and return;
		IcmpPacket ipack = get_icmp_packet();
		return ipack;
	}
	
	/**
	 * To be called only when initialised with the predefined values. 
	 * Used to listen for an expected reply packet is response to the previously sent packet.
	 * blocking call, cannot be called from main thread in android, see asynctask for more details
	 * @return a icmp packet with corresponding headers
	 */
	public IcmpPacket getReplyIcmpPacket(){
		IcmpPacket ipack;
		while(true){
			ipack = getIcmpPacket();
			if(ipack.getIPHeader().getDestIp().equals(iphdr.getSourceIp()) //sender ip equals dest ip of rcvd packet
					&& ipack.getIPHeader().getSourceIp().equals(iphdr.getDestIp()))
			{
				return ipack;
			}
		}
	}
	

	/**
	 * tells whether ipv4 header and and icmp header is initialized and 
	 * sendData and getReplyPaket function can be used safely.
	 * @return true if properly setup with right values
	 */
	public boolean isPreset(){
		return preset;
	}
	
	public IPHeader getPresetIpHeader(){
		return iphdr;
	}
	
	public ICMPHeader getPresetIcmpHeader(){
		return icmphdr;
	}
}
