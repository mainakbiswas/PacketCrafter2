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


