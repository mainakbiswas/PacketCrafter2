#include<jni.h>
#include "com_org_iitd_socketx_IcmpSocketX.h"

#include <android/log.h>

//to print error log use __android_log_print(ANDROID_LOG_INFO, "MYPROG", "ajob errno = %d, %s", s, strerror(s));
#include <string.h>
#include <arpa/inet.h>
#include<sys/socket.h>	//for socket ofcourse
#include<stdlib.h> //for exit(0);
#include<errno.h> //For errno - the error number
#include<netinet/ip_icmp.h>	//Provides declarations for tcp header
#include<netinet/ip.h>	//Provides declarations for ip header

/* 
 96 bit (12 bytes) pseudo header needed for icmp header checksum calculation 
 */
static unsigned short in_cksum(unsigned short *addr, int len)
{
	int nleft = len;
	int sum = 0;
	unsigned short *w = addr;
	unsigned short answer = 0;

	while (nleft > 1) {
		sum += *w++;
		nleft -= 2;
	}

	if (nleft == 1) {
		*(unsigned char *) (&answer) = *(unsigned char *) w;
		sum += answer;
	}
	
	sum = (sum >> 16) + (sum & 0xFFFF);
	sum += (sum >> 16);
	answer = ~sum;
	return (answer);
}

static unsigned short csum(unsigned short *ptr,int nbytes) 
{
	register long sum;
	unsigned short oddbyte;
	register short answer;
	
	sum=0;
	while(nbytes>1) {
		sum+=*ptr++;
		nbytes-=2;
	}
	if(nbytes==1) {
		oddbyte=0;
		*((u_char*)&oddbyte)=*(u_char*)ptr;
		sum+=oddbyte;
	}
	
	sum = (sum>>16)+(sum & 0xffff);
	sum = sum + (sum>>16);
	answer=(short)~sum;
	
	return(answer);
}

static int s = -1; //socket to send data out as well as to listen to for data

JNIEXPORT jint JNICALL Java_com_org_iitd_socketx_IcmpSocketX_send_1icmp_1packet
  (JNIEnv *env, jobject obj, jint tos, jint id, jint frag, jint ttl, jstring _sip, jstring _dip, jint type, jint code, jint hdr1, jint hdr2, jstring _data){
	
	char sip[32];
	char dip[32];
	int dport= 1234; //placeholder only
	
	const char *dataSend = (*env)->GetStringUTFChars(env, _data, 0);
	const char *sourceIp = (*env)->GetStringUTFChars(env, _sip, 0);
	const char *destIp = (*env)->GetStringUTFChars(env, _dip, 0);
	
	strcpy(sip, sourceIp);
	strcpy(dip, destIp);
	
	if(s == -1)
	{
		s = socket (PF_INET, SOCK_RAW, IPPROTO_ICMP);
		if(s == -1)
		{
			__android_log_print(ANDROID_LOG_INFO, "packetcrafter", "errno = %d, %s", s, "socket creation failed");
			//socket creation failed, may be because of non-root privileges
			(*env)->ReleaseStringUTFChars(env,_dip, destIp);
			(*env)->ReleaseStringUTFChars(env,_sip, sourceIp);
			(*env)->ReleaseStringUTFChars(env,_data, dataSend);
			return -1;
		}
		//IP_HDRINCL to tell the kernel that headers are included in the packet
		int one = 1;
		const int *val = &one;
		int abc;

		if ((abc = setsockopt (s, IPPROTO_IP, IP_HDRINCL, val, sizeof (one))) < 0)
		{
			__android_log_print(ANDROID_LOG_INFO, "packetcrafter", "errno = %d, %s", abc, "setsockopt failed");
			(*env)->ReleaseStringUTFChars(env,_dip, destIp);
			(*env)->ReleaseStringUTFChars(env,_sip, sourceIp);
			(*env)->ReleaseStringUTFChars(env,_data, dataSend);
			return -2;
		}
	}
	
	//Datagram to represent the packet
	char datagram[65536] , source_ip[32] , *data , *pseudogram;
	
	//zero out the packet buffer
	memset (datagram, 0, 65536);
	
	//IP header
	struct iphdr *iph = (struct iphdr *) datagram;
	
	//ICMP header
	struct icmp *icmph = (struct icmp *) (datagram + sizeof (struct iphdr));
	struct sockaddr_in sin;
		
	//Data part
	data = datagram + sizeof(struct iphdr) + sizeof(struct icmp) - 10*2;
	strcpy(data , dataSend);
	
	//some address resolution
	strcpy(source_ip , sip);
	sin.sin_family = AF_INET;
	sin.sin_port = htons(dport);
	sin.sin_addr.s_addr = inet_addr (dip);
	
	//Fill in the IP Header
        
	iph->ihl = 5;
	iph->version = 4;
	iph->tos = tos;
	iph->tot_len = sizeof (struct iphdr) + sizeof (struct icmp) + strlen(data) - 10*2;
	iph->id = htons (id);	//Id of this packet
	iph->frag_off = ntohs(frag);
	iph->ttl = ttl;
	iph->protocol = IPPROTO_ICMP;
	iph->check = 0;		//Set to 0 before calculating checksum
	iph->saddr = inet_addr ( source_ip );	//Spoof the source ip address
	iph->daddr = sin.sin_addr.s_addr;
	
	//Ip checksum
	iph->check = csum ((unsigned short *) datagram, iph->tot_len);
	
	//ICMP Header
	icmph->icmp_type = type;
	icmph->icmp_code = code;
	icmph->icmp_id = htons(hdr1);
	icmph->icmp_seq = htons(hdr2);
	icmph->icmp_cksum = 0;
	icmph->icmp_cksum = in_cksum((unsigned short *)icmph, sizeof(struct icmp) - 10*2 + strlen(data));
	
	
	int err;
	if ((err = sendto (s, datagram, iph->tot_len ,	0, (struct sockaddr *) &sin, sizeof (sin))) < 0)
	{
		__android_log_print(ANDROID_LOG_INFO, "packetcrafter", "errno = %d, %s", err, "sending icmp failed");
		(*env)->ReleaseStringUTFChars(env,dip, destIp);
		(*env)->ReleaseStringUTFChars(env,sip, sourceIp);
		(*env)->ReleaseStringUTFChars(env,data, dataSend);	
		return -3;
	}

	(*env)->ReleaseStringUTFChars(env,_dip, destIp);
	(*env)->ReleaseStringUTFChars(env,_sip, sourceIp);
	(*env)->ReleaseStringUTFChars(env,_data, dataSend);
	return 0;
}

typedef struct pack{
	unsigned int tos;
	unsigned int id;
	unsigned int frag;
	unsigned int ttl;
	char sadd[500];
	char dadd[500];
	unsigned int type;
	unsigned int code;
	unsigned int hdr1;
	unsigned int hdr2;
	char data[70000];
}icmp_packet;

static icmp_packet process_icmp_packet(char Buffer[] , int Size){
	icmp_packet result;
	
	unsigned short iphdrlen;
	struct iphdr *iph = (struct iphdr *)Buffer;
        iphdrlen = iph->ihl*4;
        
        struct icmp *icmph = (struct icmp*)(Buffer + iphdrlen);
        
        //get iphdr params
        struct sockaddr_in source,dest;
        
        memset(&source, 0, sizeof(source));
	source.sin_addr.s_addr = iph->saddr;
	
	memset(&dest, 0, sizeof(dest));
	dest.sin_addr.s_addr = iph->daddr;
	
	result.tos = (unsigned int)iph->tos;
	result.id = ntohs(iph->id);
	result.frag = (unsigned int)iph->frag_off;
	result.ttl = (unsigned int)iph->ttl;
	strcpy(result.sadd,inet_ntoa(source.sin_addr));
	strcpy(result.dadd, inet_ntoa(dest.sin_addr));
	
	//get icmp parameters
	result.type = icmph->icmp_type;
	result.code = icmph->icmp_code;
	result.hdr1 = ntohs(icmph->icmp_id);
	result.hdr2 = ntohs(icmph->icmp_seq);
	
	struct icmphdr *measure;
	
	strncpy(result.data, Buffer + iphdrlen + sizeof(measure) + 4, ( Size - sizeof(measure) - iph->ihl * 4 ) - 4);
	result.data[(Size - sizeof(measure) - iph->ihl * 4 ) - 4 + 1] = '\0'; //ensure null termination
        
        return result;
}

JNIEXPORT jobject JNICALL Java_com_org_iitd_socketx_IcmpSocketX_get_1icmp_1packet
  (JNIEnv *env, jobject obj){
	unsigned char buffer[65536];
	if(s == -1){
		s = socket (PF_INET, SOCK_RAW, IPPROTO_ICMP);
		if(s == -1)
		{
			__android_log_print(ANDROID_LOG_INFO, "packetcrafter", "errno = %d, %s", s, "socket creation failed");
			return (*env)->NewGlobalRef(env, NULL);
		}
	}
	struct sockaddr saddr;
	int saddr_size = sizeof saddr;
	int data_size = recvfrom(s , buffer , 65536 , 0 , &saddr , &saddr_size);
        if(data_size <0 )
        {
        	__android_log_print(ANDROID_LOG_INFO, "packetcrafter", "Recvfrom error , failed to get packets\n");
        	return (*env)->NewGlobalRef(env, NULL);
        }
        
        //Now process the packet and get all fields.
        icmp_packet pack = process_icmp_packet(buffer,data_size);
        
        __android_log_print(ANDROID_LOG_INFO, "\n\npacketcrafter", "Recv data: %s from: %s\n\n",pack.data,pack.sadd);
        
        //create the class object and return
        jclass icmpClass = (*env)->FindClass(env, "com/org/iitd/packetx/IcmpPacket");
        jmethodID cid = (*env)->GetMethodID(env, icmpClass, 
                                       "<init>", "(IIIILjava/lang/String;Ljava/lang/String;IIIILjava/lang/String;)V");
        jobject result = (*env)->NewObject(env, icmpClass, cid, pack.tos, pack.id, pack.frag, pack.ttl, (*env)->NewStringUTF(env, pack.sadd), (*env)->NewStringUTF(env, pack.dadd), pack.type, pack.code, pack.hdr1, pack.hdr2, (*env)->NewStringUTF(env, pack.data));
        return result;
}
