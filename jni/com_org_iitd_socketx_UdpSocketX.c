#include<jni.h>
#include "com_org_iitd_socketx_UdpSocketX.h"

#include <android/log.h>

//to print error log use __android_log_print(ANDROID_LOG_INFO, "MYPROG", "ajob errno = %d, %s", s, strerror(s));

#include <arpa/inet.h>
#include<string.h> 		//memset
#include<sys/socket.h>		//for socket ofcourse
#include<stdlib.h> 		//for exit(0);
#include<errno.h> 		//For errno - the error number
#include<netinet/udp.h>		//Provides declarations for udp header
#include<netinet/ip.h>		//Provides declarations for ip header

/* 
 96 bit (12 bytes) pseudo header needed for udp header checksum calculation 
 */
struct pseudo_header
{
	u_int32_t source_address;
	u_int32_t dest_address;
	u_int8_t placeholder;
	u_int8_t protocol;
	u_int16_t udp_length;
};

/*
 Generic checksum calculation function
 */
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

JNIEXPORT jint JNICALL Java_com_org_iitd_socketx_UdpSocketX_send_1udp_1packet
  (JNIEnv *env, jobject this, jint tos, jint id, jint frag, jint ttl, jstring sip, jstring dip, jint sport, jint dport, jstring data)
{
	const char *dataSend = (*env)->GetStringUTFChars(env, data, 0);
	const char *sourceIp = (*env)->GetStringUTFChars(env, sip, 0);
	const char *destIp = (*env)->GetStringUTFChars(env, dip, 0);
	
	if(s == -1)
	{
		s = socket (PF_INET, SOCK_RAW, IPPROTO_UDP);
		if(s == -1)
		{
			__android_log_print(ANDROID_LOG_INFO, "packetcrafter", "errno = %d, %s", s, "socket creation failed");
			//socket creation failed, may be because of non-root privileges
			(*env)->ReleaseStringUTFChars(env,dip, destIp);
			(*env)->ReleaseStringUTFChars(env,sip, sourceIp);
			(*env)->ReleaseStringUTFChars(env,data, dataSend);
			return -1;
		}
		//IP_HDRINCL to tell the kernel that headers are included in the packet
		int one = 1;
		const int *val = &one;
		int abc;

		if ((abc = setsockopt (s, IPPROTO_IP, IP_HDRINCL, val, sizeof (one))) < 0)
		{
			__android_log_print(ANDROID_LOG_INFO, "packetcrafter", "errno = %d, %s", abc, "setsockopt failed");
			(*env)->ReleaseStringUTFChars(env,dip, destIp);
			(*env)->ReleaseStringUTFChars(env,sip, sourceIp);
			(*env)->ReleaseStringUTFChars(env,data, dataSend);
			return -2;
		}
	}
	
	//Datagram to represent the packet
	char datagram[65536] , source_ip[32] , *dataToSend , *pseudogram;

	//zero out the packet buffer
	memset (datagram, 0, 65536);

	//IP header
	struct iphdr *iph = (struct iphdr *) datagram;

	//UDP header
	struct udphdr *udph = (struct udphdr *) (datagram + sizeof (struct ip));
	struct sockaddr_in sin;
	struct pseudo_header psh;

	//Data part
	dataToSend = datagram + sizeof(struct iphdr) + sizeof(struct udphdr);
	strcpy(dataToSend , dataSend);
	
	//some address resolution
	strcpy(source_ip , sourceIp);
	sin.sin_family = AF_INET;
	sin.sin_port = htons(dport);
	sin.sin_addr.s_addr = inet_addr (destIp);
	
	//Fill in the IP Header
	iph->ihl = 5;
	iph->version = 4;
	iph->tos = tos;
	iph->tot_len = sizeof (struct iphdr) + sizeof (struct udphdr) + strlen(dataToSend);
	iph->id = htons (id);
	iph->frag_off = ntohs(frag);
	iph->ttl = ttl;
	iph->protocol = IPPROTO_UDP;
	iph->check = 0;	
	iph->saddr = inet_addr ( source_ip );
	iph->daddr = sin.sin_addr.s_addr;
	
	//Ip checksum
	iph->check = csum ((unsigned short *) datagram, iph->tot_len);
	
	//UDP Header
	udph->source = htons (sport);
	udph->dest = htons (dport);
	udph->len = htons(sizeof(struct udphdr) + strlen(dataToSend) );
	udph->check = 0;	//leave checksum 0 now, filled later by pseudo header
	
	//Now the UDP checksum
	psh.source_address = inet_addr( source_ip );
	psh.dest_address = sin.sin_addr.s_addr;
	psh.placeholder = 0;
	psh.protocol = IPPROTO_UDP;
	psh.udp_length = htons(sizeof(struct udphdr) + strlen(dataToSend) );
	
	int psize = sizeof(struct pseudo_header) + sizeof(struct udphdr) + strlen(dataToSend);
	pseudogram = malloc(psize);	
	
	memcpy(pseudogram , (char*) &psh , sizeof (struct pseudo_header));
	memcpy(pseudogram + sizeof(struct pseudo_header) , udph , sizeof(struct udphdr) + strlen(dataToSend));
	
	udph->check = csum( (unsigned short*) pseudogram , psize);
	
	int err;
	if ((err = sendto (s, datagram, iph->tot_len ,	0, (struct sockaddr *) &sin, sizeof (sin))) < 0)
	{
		__android_log_print(ANDROID_LOG_INFO, "packetcrafter", "errno = %d, %s", err, "sending udp failed");
		(*env)->ReleaseStringUTFChars(env,dip, destIp);
		(*env)->ReleaseStringUTFChars(env,sip, sourceIp);
		(*env)->ReleaseStringUTFChars(env,data, dataSend);	
		return -3;
	}

	
	(*env)->ReleaseStringUTFChars(env,dip, destIp);
	(*env)->ReleaseStringUTFChars(env,sip, sourceIp);
	(*env)->ReleaseStringUTFChars(env,data, dataSend);
	return 0;	
}
	
//structure to pass around packet fields.
typedef struct packet{
	unsigned int tos;
	unsigned int id;
	unsigned int frag;
	unsigned int ttl;
	char sadd[500];
	char dadd[500];
	unsigned int sport;
	unsigned int dport;
	char data[70000];
}udp_packet;

static udp_packet process_udp_packet(char Buffer[] , int Size){
	udp_packet result;
	
	unsigned short iphdrlen;
	struct iphdr *iph = (struct iphdr *)Buffer;
        iphdrlen = iph->ihl*4;
        
        struct udphdr *udph = (struct udphdr*)(Buffer + iphdrlen);
        
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
	
        //get udphdr params
        result.sport = ntohs(udph->source);
	result.dport =  ntohs(udph->dest);
	
	strncpy(result.data, 4 + Buffer + iphdrlen + sizeof udph, ( Size - sizeof udph - iph->ihl * 4 ) - 4);
	result.data[(Size - sizeof udph - iph->ihl * 4 ) - 4 + 1] = '\0'; //ensure null termination
        
        return result;
}
	
JNIEXPORT jobject JNICALL Java_com_org_iitd_socketx_UdpSocketX_get_1udp_1packet(JNIEnv *env, jobject obj){
	unsigned char buffer[65536];
	if(s == -1){
		s = socket (PF_INET, SOCK_RAW, IPPROTO_UDP);
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
        	
        }
        //Now process the packet and get all fields.
        udp_packet pack = process_udp_packet(buffer , data_size);
        
        //test
        __android_log_print(ANDROID_LOG_INFO, "\n\npacketcrafter", "Recv data: %s from: %s\n\n",pack.data,pack.sadd);
        //endtest
        //create the class object and return
        jclass udpClass = (*env)->FindClass(env, "com/org/iitd/packetx/UdpPacket");
        //jclass udp = (*env)->NewWeakGlobalRef(env, udpClass);
        jmethodID cid = (*env)->GetMethodID(env, udpClass, //udp,
                                       "<init>", "(IIIILjava/lang/String;Ljava/lang/String;IILjava/lang/String;)V");
        jobject result = (*env)->NewObject(env, udpClass, cid, pack.tos, pack.id, pack.frag, pack.ttl, (*env)->NewStringUTF(env, pack.sadd), (*env)->NewStringUTF(env, pack.dadd), pack.sport, pack.dport, (*env)->NewStringUTF(env, pack.data));
        return result;
}
