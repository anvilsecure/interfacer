#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string.h>
#include <net/if.h>

// Define a prototype for the setmyinterface function
void setmyinterface(int sockfd);
struct ifreq ifr;
int ifr_init = 0;
// Function pointers for the original functions
int (*original_socket)(int domain, int type, int protocol) = NULL;
int (*original_setsockopt)(int sockfd, int level, int optname, const void *optval, socklen_t optlen) = NULL;

void ensurehooks()
{
    if(!ifr_init) {
        const char *interface_name = getenv("INTERFACE_NAME");
        if(interface_name)
        {
            memset(&ifr, 0, sizeof(ifr));
            strncpy(ifr.ifr_name, interface_name, strlen(interface_name));
            original_socket = (int (*)(int, int, int)) dlsym(RTLD_NEXT, "socket");
            original_setsockopt = (int (*)(int, int, int, const void *, socklen_t)) dlsym(RTLD_NEXT, "setsockopt");
        }
        ifr_init = 1;
    }
}

// Hooked socket() function
int socket(int domain, int type, int protocol) {
    ensurehooks();
    int sockfd = original_socket(domain, type, protocol);
    setmyinterface(sockfd);
    return sockfd;
}

// Hooked setsockopt() function
int setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen) {
    ensurehooks();
    if (optname == SO_BINDTODEVICE) {
        setmyinterface(sockfd);
    }

    return original_setsockopt(sockfd, level, optname, optval, optlen);
}

void setmyinterface(int sockfd) {
    if (original_setsockopt(sockfd, SOL_SOCKET, SO_BINDTODEVICE, (void*)&ifr, sizeof(ifr)) < 0) {
        perror("Error setting SO_BINDTODEVICE");
    } else {
        printf("Socket %d bound to interface %s\n", sockfd, ifr.ifr_name);
    }
}