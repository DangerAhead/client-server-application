
/*Vatsayayn Binay: 2018A7PS0305H
Satvik Arora : 2017B5A71671H
Hardik Jain: 2017B3A71113H
Viraj Singh: 2017B3A71099H
Shivali Ratra: 2017B4A71707H*/

#include "protocol.h"

using namespace std;




int main()
{
  

  sd = socket(AF_INET, SOCK_DGRAM, 0);

  if(sd==-1)
    {
      printf(" socket not created in server\n");
      exit(0);
    }
  else
    {
      printf("socket created in  server\n");
    }

  bzero(&servaddr, sizeof(servaddr));

  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = INADDR_ANY;
  servaddr.sin_port = htons(6000);
  memset(&(servaddr.sin_zero),'\0',8);
  if ( bind(sd, (struct sockaddr *)&servaddr, sizeof(servaddr)) != 0 )
    printf("Not binded\n");
  else
    printf("Binded\n");

  len=sizeof(cliaddr);
  string fileName=recvFileName();

  memset(filled,false,sizeof(filled));
  memset(acks,false,sizeof(acks));

  pthread_t ptid1;
  pthread_t ptid2;
  pthread_t ptid4;

  if (pthread_mutex_init(&lock1, NULL) != 0) {
        printf("\n mutex init has failed\n");
        return 1;
    }

  pthread_create(&ptid1, NULL, &fillBuffer, NULL);
  pthread_create(&ptid2, NULL, &recieveAcks, NULL);
  pthread_create(&ptid4, NULL, &sendPacketsFunc, NULL);

  pthread_join(ptid1,NULL);
  pthread_join(ptid2,NULL);
  pthread_join(ptid4,NULL);

  pthread_mutex_destroy(&lock1);

  cout<<"File sent successfully"<<endl;
  fclose(fp);

  for(int i=0;i<3000;i++)
  {
    packet temp;
    temp.seq=-1;
    temp.checksum=-1;
    sendto(sd, (char*)&temp, sizeof(temp), 0, (sockaddr *)&cliaddr, sizeof(cliaddr));
    exit(0);
  }


}