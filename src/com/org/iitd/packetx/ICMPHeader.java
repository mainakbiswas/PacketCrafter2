/**
 * This file describes the ICMP header structure
 * @author Mainak Biswas
 */

package com.org.iitd.packetx;

public class ICMPHeader {
	private int type;
	private int code;
	private int header1; 	//first 16bits of the 32bits following type, code and chksum
	private int header2; 	//second 16 bits of the 32bits following type, code and chksum
	public ICMPHeader(int type,int code,int header1, int header2){
		this.type = type;
		this.code = code;
		this.header1 = header1;
		this.header2 = header2;
	}
	public void setCode(int code){
		this.code = code;
	}
	public void setType(int type){
		this.type = type;
	}
	public void setHeader1(int h){
		header1 = h;
	}
	public void setHeader2(int h){
		header2 = h;
	}
	public int getType(){
		return type;
	}
	public int getcode(){
		return code;
	}
	public int getHeader1(){
		return header1;
	}
	public int getHeader2(){
		return header2;
	}
}
