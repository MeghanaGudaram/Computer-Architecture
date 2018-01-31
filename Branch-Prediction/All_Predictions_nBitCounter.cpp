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

int PHT_l1[1024];
int PHT_l2G[1024];
int PHT_l2GS[1024];
int PHT_l2l[1024];
int GHR_l2G;
int GHR_l2GS;
int LHR_l2l[128];

long int *PCBranch=(long int *) malloc(20000000*sizeof(long int));
char *PCBranchPredict=(char *) malloc(20000000*sizeof(char));

int level1_BP(int n)
{
  int missPenalty=0, PHT_l1C=0, index=0, maxN=((int) pow(2,n)) - 1;
  long int i=0;
  while(PCBranch[i] != 0)
  {
    index = (PCBranch[i] >> 2) & 0x000003ff;
    PHT_l1C=PHT_l1[index];
    if((PCBranchPredict[i]=='T' && PHT_l1C<=maxN/2) || (PCBranchPredict[i]=='N' && PHT_l1C>maxN/2))
    {
      missPenalty++;
    }
    if(PCBranchPredict[i]=='T')
      PHT_l1[index]=min(maxN, PHT_l1[index]+1);
    else
      PHT_l1[index]=max(0, PHT_l1[index]-1);

      i++;
    }
    return missPenalty;
  }

  int level2G_BP(int n)
  {
    int missPenalty=0, PHT_l2GC=0, index=0, maxN=((int) pow(2,n)) - 1;
    long int i=0;
    while(PCBranch[i] != 0)
    {
      index = GHR_l2G & 0x000003ff;
      PHT_l2GC = PHT_l2G[index];
      if((PCBranchPredict[i]=='T' && PHT_l2GC<=maxN/2) || (PCBranchPredict[i]=='N' && PHT_l2GC>maxN/2))
      {
        missPenalty++;
      }
      if(PCBranchPredict[i]=='T')
      {
        PHT_l2G[index]=min(maxN, PHT_l2G[index]+1);
        GHR_l2G = (GHR_l2G << 1) + 0x00000001;
      }
      else
      {
        PHT_l2G[index]=max(0, PHT_l2G[index]-1);
        GHR_l2G = GHR_l2G << 1;
      }

      i++;
    }
    return missPenalty;
  }

  int level2GS_BP(int n)
  {
    int missPenalty=0, PHT_l2GSC=0, index=0, maxN=((int) pow(2,n)) - 1;
    long int i=0;
    while(PCBranch[i] != 0)
    {
      index = (GHR_l2GS ^ (PCBranch[i] >> 6)) & 0x000003ff;
      PHT_l2GSC = PHT_l2GS[index];
      if((PCBranchPredict[i]=='T' && PHT_l2GSC<=maxN/2) || (PCBranchPredict[i]=='N' && PHT_l2GSC>maxN/2))
      {    missPenalty++;
      }
      if(PCBranchPredict[i]=='T')
      {
        PHT_l2GS[index]=min(maxN, PHT_l2GS[index]+1);
        GHR_l2GS = (GHR_l2GS << 1) + 0x00000001;
      }
      else
      {
        PHT_l2GS[index]=max(0, PHT_l2GS[index]-1);
        GHR_l2GS = GHR_l2GS << 1;
      }

      i++;
    }
    return missPenalty;
  }

  /* 2-Level Local Predictor with 2-Bit counter for Pattern History Table    Size of PHT: 1024, Width of LHR: 10 (History of 10 predictions for a branch), Size of LHR: 128 (7 bits of PC)   */
  int level2l_BP(int n)
  {
    int missPenalty=0, PHT_l2lC=0, index=0, lindex=0, maxN=((int) pow(2,n)) - 1;
    long int i=0;
    while(PCBranch[i] != 0)
    {
      lindex = (PCBranch[i] >> 9) & 0x0000007f;
      index = LHR_l2l[lindex & 0x0000007f] & 0x000003ff;
      PHT_l2lC = PHT_l2l[index];
      if((PCBranchPredict[i]=='T' && PHT_l2lC<=maxN/2) || (PCBranchPredict[i]=='N' && PHT_l2lC>maxN/2))
      {
        missPenalty++;
      }
      if(PCBranchPredict[i]=='T')
      {
        PHT_l2l[index]=min(maxN, PHT_l2l[index]+1);
        LHR_l2l[lindex & 0x0000007f] = (LHR_l2l[lindex & 0x0000007f] << 1) + 0x00000001;
      }
      else
      {
        PHT_l2l[index]=max(0, PHT_l2l[index]-1);
        LHR_l2l[lindex & 0x0000007f] = LHR_l2l[lindex & 0x0000007f] << 1;
      }

      i++;
    }
    return missPenalty;
  }

  int main(int argc, char *argv[])
  {
    FILE *fin;  fin=fopen(argv[1],"r");
    long int i=0;
    while(fscanf(fin,"%ld %c",&PCBranch[i],&PCBranchPredict[i])==2)
    {
      i++;
    }
    int n;
    printf("Enter n value for counter: ");
    scanf("%d",&n);
    int MissPenalty1L = level1_BP(n);
    int MissPenalty2LG = level2G_BP(n);
    int MissPenalty2LGS = level2GS_BP(n);
    int MissPenalty2LL = level2l_BP(n);
    double f= (double)MissPenalty1L/(double)i;
    printf("Miss Penalty Count for 1-level Branch Prediction: %d with total branch calls: %ld, Rate: %lf or %0.2lf%\n",MissPenalty1L,i,f, f*100);
    f = (double)MissPenalty2LG/(double)i;
    printf("Miss Penalty Count for 2-level Global Branch Prediction: %d with total branch calls: %ld, Rate: %lf or %0.2lf%\n",MissPenalty2LG,i,f, f*100);
    f = (double)MissPenalty2LGS/(double)i;
    printf("Miss Penalty Count for 2-level G-Share Branch Prediction: %d with total branch calls: %ld, Rate: %lf or %0.2lf%\n",MissPenalty2LGS,i,f, f*100);
    f = (double)MissPenalty2LL/(double)i;
    printf("Miss Penalty Count for 2-level local Branch Prediction: %d with total branch calls: %ld, Rate: %lf or %0.2lf%\n",MissPenalty2LL,i,f, f*100);
    return 0;
}
