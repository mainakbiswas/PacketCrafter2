/**
 * This file describes the IPv4 header structure.
 * Only those parts which can be varied by our API appears in the file.
 * @author Mainak Biswas
 */

package com.org.iitd.packetx;

public class IPHeader {
	private int tos = 0;       		//ECN bits etc
	private int id=65981;			//Id of this packet
	private int frag_off = 0;  		//DF,MF flags cum fragmentation offset
	private int ttl = 255;			//time to live in number of hops defaulted to 255
	private String saddr;			//The source ip address to be written on packets
	private String daddr; 			//Destnation IP address
	
	/**
	 * construct IPV4header
	 * @param tos type of service (ECN bit setting etc)
	 * @param id unique id of each ip packet sent out of this machine
	 * @param frag_off DF,MF flags cum fragmentation offset
	 * @param ttl time to live in number of hops
	 * @param saddr The source ip address to be written on packets 
	 * 				<p> (in case of self ip, should pass the obtained system value) 
	 * @param daddr The source ip address to be written on packets
	 */
	
	public IPHeader(int tos, int id, int frag_off, int ttl, String saddr, String daddr){
		this.tos = tos;
		this.id = id;
		this.frag_off = frag_off;
		this.ttl = ttl;
		this.saddr = saddr;
		this.daddr = daddr;
	}
	
	public int getTos(){
		return tos;
	}
	
	public int getId(){
		return id;
	}
	
	public int getFragOff(){
		return frag_off;
	}
	
	public int getTtl(){
		return ttl;
	}
	
	public String getSourceIp(){
		return saddr;
	}
	
	public String getDestIp(){
		return daddr;
	}
	
	public void setTos(int tos){
		this.tos = tos;
	}
	
	public void setId(int id){
		this.id = id;
	}
	
	public void setFragOff(int frag){
		this.frag_off = frag;
	}
	
	public void setTtl(int ttl){
		this.ttl = ttl;
	}
	
	public void setSourceIp(String ip){
		this.saddr = ip;
	}
	
	public void setDestIp(String ip){
		this.daddr = ip;
	}
}
