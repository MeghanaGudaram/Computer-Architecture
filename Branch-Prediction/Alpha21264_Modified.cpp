/*
* Author: Meghana Gudaram
*/

#include<stdio.h>
#include<stdint.h>
#include<malloc.h>
#include<stdlib.h>
#include<math.h>

#define max( a, b ) ( ((a) > (b)) ? (a) : (b) )
#define min( a, b ) ( ((a) < (b)) ? (a) : (b) )

int PHT_l2l[1024];
int PHT_l2GS[32768];
int PHT_l2SEL[32768];

int GHR_l2GS;
int LHR_l2l[1024];

long int *PCBranch=(long int *) malloc(20000000*sizeof(long int));
char *PCBranchPredict=(char *) malloc(20000000*sizeof(char));


int level2lGS_BP()
{
  int missPenalty=0, PHT_l2lC=0, PHT_l2GSC=0, llindex=0, gindex=0, lindex=0;
  long int i=0;
  char Predict='T';

  while(PCBranch[i] != 0)
  {
    lindex = (PCBranch[i] >> 2) & 0x000003ff;
    llindex = (LHR_l2l[lindex & 0x000003ff] & 0x000003ff);
    gindex = ((GHR_l2GS ^ (PCBranch[i] >> 2)) & 0x00007fff);

    PHT_l2lC = PHT_l2l[llindex];
    PHT_l2GSC = PHT_l2GS[gindex];

    if(PHT_l2SEL[gindex]>1)
    {
      if(PHT_l2GSC >1)
      Predict='T';
      else
      Predict='N';
    }
    else
    {
      if(PHT_l2lC > 3)
      Predict= 'T';
      else    Predict= 'N';
    }
    if((PCBranchPredict[i]=='T' && Predict=='N') || (PCBranchPredict[i]=='N' && Predict=='T'))
    {
      missPenalty++;
    }
    if((PCBranchPredict[i]=='T' && Predict=='T' && PHT_l2GSC >1) || (PCBranchPredict[i]=='N' && Predict=='N' && PHT_l2GSC <=1))
      PHT_l2SEL[gindex]=min(3, PHT_l2SEL[gindex]++);
    else
      PHT_l2SEL[gindex]=max(0, PHT_l2SEL[gindex]--);
    if(PCBranchPredict[i]=='T')
    {
      PHT_l2l[llindex]=min(7, PHT_l2l[llindex]+1);
      PHT_l2GS[gindex]=min(3, PHT_l2GS[gindex]+1);
      LHR_l2l[lindex & 0x000003ff] = (LHR_l2l[lindex & 0x000003ff] << 1) + 0x00000001;
      GHR_l2GS = (GHR_l2GS << 1) + 0x00000001;
    }
    else
    {
      PHT_l2l[llindex]=max(0, PHT_l2l[llindex]-1);
      PHT_l2GS[gindex]=max(0, PHT_l2GS[gindex]-1);
      LHR_l2l[lindex & 0x000003ff] = LHR_l2l[lindex & 0x000003ff] << 1;
      GHR_l2GS = (GHR_l2GS << 1);
    }
      i++;
    }
    return missPenalty;
  }
  int main(int argc, char *argv[])
  {
    FILE *fin;
    fin=fopen(argv[1],"r");
    long int i=0;
    while(fscanf(fin,"%ld %c",&PCBranch[i],&PCBranchPredict[i])==2)
    {
      i++;
    }
    int MissPenalty2LLGS = level2lGS_BP();
    double f = (double)MissPenalty2LLGS/(double)i;
    printf("Miss Penalty Count for 2-level local Brance Prediction + Gshare: %d with total branch calls: %ld, Rate: %lf or %0.2lf%\n",MissPenalty2LLGS,i,f, f*100);
    return 0;
  }
