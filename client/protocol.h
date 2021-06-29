
/*Vatsayayn Binay: 2018A7PS0305H
Satvik Arora : 2017B5A71671H
Hardik Jain: 2017B3A71113H
Viraj Singh: 2017B3A71099H
Shivali Ratra: 2017B4A71707H*/

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include<bits/stdc++.h>
#include<stdio.h>
#include<string.h>

using namespace std;

int sd,connfd,len;
struct sockaddr_in servaddr,cliaddr;
ofstream fp;
FILE *fp1;
const int bufferSize=2000,seqSize=2000;
int baseInd=0,seqNo=0;
int procEnd=0;
int fileAck=0;

struct packet
{
  int seq;
  int checksum;
  int len;
  char value[4030];
};

packet buffer[bufferSize];
int windSize=1;
bool filled[bufferSize];

int getCheckSum(char s[],int l)
{
  int curr=0;
  for(int i=0;i<l;i++)
  {
    curr+=s[i];
  }

  return curr;
}

bool checkPacket(int checksum,char str[],int l)
{
  int curr=0;
  
  for(int i=0;i<l;i++)
  {
    curr+=str[i];
  }
  
  if(curr==checksum)
  {
    return true;
  }

  else
  {
    cout<<"damaged"<<endl;
    return false;
  }
}

void* recvFileAck(void *arg)
{
  while(1)
  {
    packet temp;
    recvfrom(sd, (char*)&temp, sizeof(packet), 0, (sockaddr *)&cliaddr, (socklen_t*)&len);

    if(temp.seq==-3)
    {
      cout<<"file doesn't exist on server"<<endl;
      exit(0);
    }
    else
    {
      fileAck=1;
      break;
    }
  }

  pthread_exit(NULL);

}

void sendFileName(string fileName)
{
  packet pkt;
  pkt.checksum=getCheckSum((char *)fileName.c_str(),fileName.length());
  pkt.seq=-2;
  pkt.len=fileName.length();
  strcpy(pkt.value,fileName.c_str());

  pthread_t ptid1;
  pthread_create(&ptid1, NULL, &recvFileAck, NULL);
  pthread_detach(ptid1);


  while(fileAck!=1)
  {
    sendto(sd, (char*)&pkt, sizeof(packet), 0, (sockaddr *)&servaddr, sizeof(servaddr));
  }
}

void* recvPackets(void *arg)
{
  while(1)
  {
    packet pkt;
    recvfrom(sd, (char*)&pkt, sizeof(packet), 0, (sockaddr *)&cliaddr, (socklen_t*)&len);
    if(pkt.seq==pkt.checksum && pkt.seq==-1)
    {
      procEnd=1;
      break;
    }


    if(pkt.seq>=0 && checkPacket(pkt.checksum,pkt.value,pkt.len))
    {

      int diff;
      int curr=pkt.seq;

      if(curr>=baseInd)
      {
        diff=curr-baseInd;
      }

      else
      {
        diff=(seqSize-baseInd)+curr;
      }

      if(diff<windSize)
      {
        buffer[curr]=pkt;
        filled[curr]=true;
      }

      packet temp;
      temp.seq=pkt.seq;
      temp.checksum=pkt.seq;
      sendto(sd, (char*)&temp, sizeof(packet), 0, (sockaddr *)&servaddr, sizeof(servaddr));

    }
  }

  pthread_exit(NULL);
}

void* writeToFile(void *arg)
{
  while(1)
  {
    if(filled[baseInd]==false && procEnd==1)
    {
      break;
    }

    if(filled[baseInd]==true)
    {
      fwrite(buffer[baseInd].value,sizeof(char),buffer[baseInd].len,fp1);
      filled[baseInd]=false;
      baseInd=(baseInd+1)%bufferSize;
      seqNo=(seqNo+1)%seqSize;
    }
  }

  pthread_exit(NULL);

}