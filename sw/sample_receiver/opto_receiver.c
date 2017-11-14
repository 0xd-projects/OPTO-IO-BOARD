
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<arpa/inet.h>
#include<sys/socket.h>

#define BUFLEN 2        // Max length of buffer
#define PORT   55000    // The port on which to listen for incoming data


void die(char *s)
{
    perror(s);
    exit(1);
}


int main(void)
{

  static union                         // union holds discrete data
  {
    unsigned short value;              // allows access to all discrete data at once.
    struct
    {
        unsigned char d9  : 1; // J3
        unsigned char d10 : 1;
        unsigned char d11 : 1;
        unsigned char d12 : 1;

        unsigned char d13 : 1;  // J4
        unsigned char d14 : 1;
        unsigned char d15 : 1;
        unsigned char d16 : 1;

        unsigned char d1  : 1; // J1
        unsigned char d2  : 1;
        unsigned char d3  : 1;
        unsigned char d4  : 1;

        unsigned char d5  : 1; //J2
        unsigned char d6  : 1;
        unsigned char d7  : 1;
        unsigned char d8  : 1;
    } bits;
  } var;

    struct sockaddr_in si_me, si_other;

    int s, i, slen = sizeof(si_other) , recv_len;
    char buf[BUFLEN];

    //create a UDP socket
    if ((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        die("socket");
    }

    // zero out the structure
    memset((char *) &si_me, 0, sizeof(si_me));

    si_me.sin_family      = AF_INET;
    si_me.sin_port        = htons(PORT);
    si_me.sin_addr.s_addr = htonl(INADDR_ANY);

    //bind socket to port
    if( bind(s , (struct sockaddr*)&si_me, sizeof(si_me) ) == -1)
    {
        die("bind");
    }


   printf("\n\n Received data will be printed on change. \n\n");

    //keep listening for data
    while(1)
    {

	static unsigned short last = 1;

        //try to receive some data, this is a blocking call
        if ((recv_len = recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen)) == -1)
        {
            die("recvfrom()");
        }

        // copy the received data into the union
        memcpy(&var.value, buf, 2);

        // deterine if the data has changed from the last received 
        if( var.value != last )
        {

          printf("Data change from  %s:%d  Data\n", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port));

          printf("\n J1 \n");
          printf("  D1: %d", var.bits.d1);
          printf("  D2: %d", var.bits.d2);
          printf("  D3: %d", var.bits.d3);
          printf("  D4: %d", var.bits.d4);

          printf("\n\n J2 \n");
          printf("  D5: %d", var.bits.d5);
          printf("  D6: %d", var.bits.d6);
          printf("  D7: %d", var.bits.d7);
          printf("  D8: %d", var.bits.d8);

          printf("\n\n J3 \n");
          printf("  D9: %d", var.bits.d9);
          printf(" D10: %d", var.bits.d10);
          printf(" D11: %d", var.bits.d11);
          printf(" D12: %d", var.bits.d12);

          printf("\n\n J4 \n");
          printf(" D13: %d", var.bits.d13);
          printf(" D14: %d", var.bits.d14);
          printf(" D15: %d", var.bits.d15);
          printf(" D16: %d", var.bits.d16);
          printf("\n\n\n");

          last = var.value;
      }

    }

    close(s);
    return 0;
}







