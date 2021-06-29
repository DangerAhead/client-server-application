
/*Vatsayayn Binay: 2018A7PS0305H
Satvik Arora : 2017B5A71671H
Hardik Jain: 2017B3A71113H
Viraj Singh: 2017B3A71099H
Shivali Ratra: 2017B4A71707H*/

#include "protocol.h"

using namespace std;
using namespace std::chrono;




int main()
{
	
  sd = socket(AF_INET, SOCK_DGRAM, 0);

	if(sd==-1)
  {
    printf(" socket not created in client\n");
    exit(0);
  }
  else
  {
    printf("socket created in  client\n");
  }

  len=sizeof(cliaddr);
  memset(&servaddr, 0, sizeof(servaddr));
  bzero(&servaddr, sizeof(servaddr));

  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = INADDR_ANY;
  servaddr.sin_port = htons(6000);
  cout<<"Enter filename: ";
  string fileName;
  cin>>fileName;
  sendFileName(fileName);
  cout<<"filename ack recieved"<<endl;
  fp1=fopen(fileName.c_str(),"w");
  
  memset(filled,false,sizeof(filled));
  
  auto start = high_resolution_clock::now();
  pthread_t ptid1;
  pthread_t ptid2;
  pthread_create(&ptid1, NULL, &writeToFile, NULL);
  pthread_create(&ptid2, NULL, &recvPackets, NULL);

  pthread_join(ptid1,NULL);
  pthread_join(ptid2,NULL);
  auto stop = high_resolution_clock::now();
  auto duration = duration_cast<microseconds>(stop - start);
  for(int i=0;i<300;i++)
  {
    packet pkt;
    pkt.seq=-1;
    pkt.checksum=-1;
    sendto(sd, (char*)&pkt, sizeof(packet), 0, (sockaddr *)&servaddr, sizeof(servaddr));
  }

  cout<<"file recieved successfully"<<endl;
  cout << "Time taken to recieve: "<< duration.count() << " microseconds" << endl;

  fclose(fp1);


  

}