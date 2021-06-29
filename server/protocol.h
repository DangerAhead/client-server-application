
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
#include <bits/stdc++.h>
#include <pthread.h>

using namespace std;

int sd,connfd,len;
struct sockaddr_in servaddr,cliaddr;
FILE *fp;
int baseInd=0,windowSize=1;
const int buffSize=2000;
const int seqSize=2000;
int procEnd=0;
int procEnd2=0;

pthread_mutex_t lock1;

struct packet
{
  int seq;
  int checksum;
  int len;
  char value[4030];
};

packet buffer[buffSize];
bool filled[buffSize];
bool acks[seqSize];

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
    return false;
  }
}

string recvFileName()
{
  string fileName;
  packet pkt;
  while(1)
  {
    recvfrom(sd, (char*)&pkt, sizeof(packet), 0, (sockaddr *)&cliaddr, (socklen_t*)&len);
    string str=pkt.value;
    
    if(checkPacket(pkt.checksum,pkt.value,pkt.len))
    {
      fileName=pkt.value;
      cout<<"filename recieved is "<<pkt.value<<endl;
      fp=fopen(fileName.c_str(),"r");
      if(fp==NULL)
      {
        cout<<"file doesn't exist"<<endl;
        for(int i=0;i<3000;i++)
        {
          packet temp;
          temp.seq=-3;
          temp.checksum=-3;
          sendto(sd, (char*)&temp, sizeof(temp), 0, (sockaddr *)&cliaddr, sizeof(cliaddr));
          exit(0);
        }
      }

      else
      {
        packet temp;
        temp.seq=-2;
        temp.checksum=-2;
        for(int i=0;i<40;i++)
        {
          sendto(sd, (char*)&temp, sizeof(temp), 0, (sockaddr *)&cliaddr, sizeof(cliaddr));
        }
      }
      
      break;
    }

    else
    {
      cout<<"filename recieved is damaged"<<endl;
    }
  }

  return fileName;

}

void* sendPackets(void *arg)
{
  int ind = *((int *) arg);
  free(arg);
  packet temp=buffer[ind];
  int seqInd=(ind%seqSize);
  while(acks[seqInd]!=true)
  {
    pthread_mutex_lock(&lock1);
    sendto(sd, (char*)&temp, sizeof(struct packet), 0, (struct sockaddr *)&cliaddr, sizeof(cliaddr));
    usleep(1000);
    pthread_mutex_unlock(&lock1);
  }

  pthread_mutex_lock(&lock1);

  while(acks[baseInd]!=false)
  {
    acks[baseInd]=false;
    filled[baseInd]=false;
    baseInd=(baseInd+1)%buffSize;
  }
  pthread_mutex_unlock(&lock1);

  pthread_exit(NULL);

}


void* sendPacketsFunc(void *arg)
{
  int curr=0;
  while(1)
  {
    if(procEnd==1 && filled[curr]==false)
    {
      while(baseInd!=curr)
      {}
      break;
    }

    int diff;

    if(curr>=baseInd)
    {
      diff=curr-baseInd;
    }

    else
    {
      diff=(seqSize-baseInd)+curr;
    }

    if(diff<windowSize && filled[curr]==true)
    {
      int *i = (int*)malloc(sizeof(*i));
      *i=curr;
      pthread_t tid;
      pthread_create(&tid, NULL, &sendPackets, (void *)i);
      pthread_detach(tid);
      curr=(curr+1)%buffSize;
    }

  }

  for(int i=0;i<300;i++)
  {
    packet pkt;
    pkt.seq=-1;
    pkt.checksum=-1;
    sendto(sd, (char*)&pkt, sizeof(packet), 0, (sockaddr *)&servaddr, sizeof(servaddr));
  }

  pthread_exit(NULL);
}


void* recieveAcks(void *arg)
{
  while(1)
  {
    packet pkt;
    
    recvfrom(sd, (char *)&pkt, sizeof(pkt), 0, (sockaddr *)&servaddr, (socklen_t*)&len);
    if(pkt.seq==pkt.checksum)
    {
      if(pkt.seq==-2)
      {
        packet temp;
        temp.seq=-2;
        sendto(sd, (char*)&temp, sizeof(temp), 0, (sockaddr *)&cliaddr, sizeof(cliaddr));
      }
      else if(pkt.seq==-1 && procEnd==1)
      {
        break;
      }
      else 
      {
        acks[pkt.seq]=true;
      }
    }
  }

  pthread_exit(NULL);
}


void* fillBuffer(void *arg)
{
  int curr=0;
  
  while(1)
  {
    if(filled[curr]==false)
    {
      packet pkt;
      int len=0;
      for(int i=0;i<2000;i++)
      {
        char c=getc(fp);
        if(feof(fp))
        {
          procEnd=1;
          break;
        }
        pkt.value[len]=c;
        len++;
      }

      pkt.seq=(curr)%seqSize;
      pkt.len=len;
      pkt.checksum=getCheckSum(pkt.value,len);
      buffer[curr]=pkt;
      filled[curr]=true;
      curr=(curr+1)%buffSize;
    }

    if(procEnd==1)
    {
      break;
    }

  }

  pthread_exit(NULL);
}