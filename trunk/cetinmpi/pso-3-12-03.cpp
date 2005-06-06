#include </opt/SUNWhpc/HPC4.0/include/mpi.h>
/*#include </opt/SUNWhpc/HPC4.0/include/mpi++.h>*/
#include "1AgentDoc.h"
#include "PSO_particle.h"
#include <vector>
#include <stdio.h>
#include <stdlib.h>
using std::vector;

#define WORKTAG		1
#define DIETAG		-1	

int master();
void slave(int myrank);

main( int argc, char  **argv)


{
    int			myrank;

    MPI_Init(&argc, &argv);	/* initialize MPI */
    MPI_Comm_rank(
	    MPI_COMM_WORLD,	/* always use this */
	    &myrank);		/* process rank, 0 thru N-1 */
	
srand( (unsigned)time( NULL ) );	
	printf("hello my rank is %d \n",myrank);	
    if (myrank == 0) {
	master();
    } else {
	slave(myrank);
    } 

    MPI_Finalize();		/* cleanup MPI */
}

int master()

{
	int CountEvaluations=0;
	FILE*  master_file;
	FILE* summary_file;
	master_file=fopen("master.txt", "wt");
	summary_file=fopen("summary.txt","wt");
	FILE* best_particle;
	best_particle=fopen("best.txt","wt");
	
	int SendArray[9899];//data to be sent as work order -- "binary sting"
    int			ntasks, rank, work;
    double		result;
    MPI_Status		status;

    MPI_Comm_size(
	    MPI_COMM_WORLD,	/* always use this */
	    &ntasks);		/* #processes in application */

	C1AgentDoc	masterAgent=C1AgentDoc();
	printf("Number of records=%d <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<",masterAgent.m_CaseRecords.RecordCount());

			int NumParticles=33;
			int n;
			vector<PSO_particle> theParticles; //need (40) here?
			PSO_particle TheBestOneEver=PSO_particle(masterAgent.m_oaNodes.size(),99);
			double TheBestFitnessEver=1.0e25;
			//create the particles and set the pointer to the parent array
fprintf(master_file,"creating n particles\n");
			for ( n=0;n<NumParticles;n++) //create n particles
			{
				PSO_particle tmpP=PSO_particle(masterAgent.m_oaNodes.size(),n);
				tmpP.p_PointersToAllParticles=&theParticles;
				theParticles.push_back(tmpP);
				//theParticles[n]=tmpP;
			}
fprintf(master_file,"creating particle neighborhoods\n");
			//define the neighborhoods of all the particles
			std::vector<PSO_particle>::iterator ip;
			for ( ip = theParticles.begin(); ip != theParticles.end(); ip++ ) 
			{
				PSO_particle& tmpP=(*ip);
				tmpP.DefineMyNeighborhood(2,tmpP.myIndex);//neighborhood size is 0/global on each side
			}

//the main loop of the PSO optimization

			vector<int> RankAndParticle;//(40);//ie : RankAndParticle[4]=22 means that process 4 has particle 22
			RankAndParticle.reserve(ntasks);	
			//RankAndParticxl will have a size equal to the number of procresses
			int SendCount,ReceiveCount;
			SendCount=0;ReceiveCount=0;	
			int count=0; //step counter for PSO Optimization, max steps at end of do/while loop
			double tmp_NetworkScore,tmp_BestFitness;
fprintf(master_file,"MAIN LOOP OF PSO OPTIMIZATION\n");
			do
			{
				ip = theParticles.begin();
				/* seed slaves */
				SendCount=0;
				ReceiveCount=0;	
				rank=1; //rank 0 is me
				int iParticle=0;	
				while (ip!=theParticles.end() && rank < ntasks)
				{
					PSO_particle& tmpP=(*ip);
printf("%x with %d arrows iprior to removing cycles SENDING IN SEED ROUND %d\n",&(*ip),tmpP.ArrowCount(),count);
					//if(!tmpP.IsCyclic())
					if(!tmpP.IsCyclic_new())
					//if(!tmpP.IsCyclic_Re_Move())
					{
fprintf(master_file,"\n in seed -----------ACYCLIC sending particle %d to rank %d in round %d\n",iParticle,rank,count);
printf("%x with %d arrows after removing cycles SENDING IN SEED ROUND %d\n",&(*ip),tmpP.ArrowCount(),count);
						for(int m=0;m<9900;m++) //fill work order to send
						{
							if(tmpP.X[m])
							{
								SendArray[m]=1;	
							}
							else
							{
								SendArray[m]=0;
							}	
//if(m<1332){
//printf("%d",SendArray[m]);}
						}
//printf("\n");
						MPI_Send(&SendArray,9899,MPI_INT,rank,iParticle,MPI_COMM_WORLD);
						CountEvaluations+=1;
						RankAndParticle[rank]=iParticle;
						//RankAndParticle.push_back(iParticle);
						rank+=1;
						SendCount+=1;
					}
					else
					{
fprintf(master_file,"\nin seed --------CYCLIC skipping  cyclic particle %d\n",iParticle);
						float CountArrows=0.0e0;
						for(int r=0;r<9900;r++)
						{
							if(tmpP.X[r])
							{
								CountArrows+=1.0e0;
								fprintf(master_file,"%d|",r);
							}
						}
						tmp_NetworkScore=1.0e-13;
						float ns=tmp_NetworkScore;
						tmpP.Fitness=(1.0/ns)*CountArrows;
					}
					iParticle+=1;	
					ip++;
				}//end while of seed processes

printf("\n\n\nDONE SEEDING processes\n\n");

				//are there still particles left to process?
printf("ENTERING MAIN PUMP\n");
				while (ip!=theParticles.end())
				{
printf("IN  MAIN PUMP waiting to receive...\n\n");
					MPI_Recv(&result,1,MPI_DOUBLE,MPI_ANY_SOURCE,MPI_ANY_TAG,MPI_COMM_WORLD,&status);
					ReceiveCount+=1;	
					int returningRank=status.MPI_SOURCE;
					PSO_particle& retP=theParticles[RankAndParticle[status.MPI_SOURCE]];
printf("received particle %d from rank %d with a fitness=%e\n",RankAndParticle[returningRank],returningRank,result);					
					retP.Fitness=result;
					tmp_BestFitness=retP.Fitness;
printf("Me Fitmess=%e NeighFitness=%e Previous Me Fitness=%e\n",retP.Fitness,retP.Fitness_N,retP.Fitness_P);
					//track best overall particle ever
					if (tmp_BestFitness<TheBestFitnessEver)
					{
						TheBestOneEver=retP;//uses copy operator
						TheBestFitnessEver=tmp_BestFitness;
						TheBestOneEver.myIndex=99;
printf("new best particle\n");
fprintf(master_file,"New Best Particle\n");
						for(int kk=0;kk<9900;kk++)
						{
							if(TheBestOneEver.X[kk])
							{
								printf("%d|",kk);
								fprintf(master_file,"%d|",kk);
							}	
						}
					}
//printf("Begin send new work order\n");					
					PSO_particle& tmpP=(*ip);
					//if(!tmpP.IsCyclic())
					if(!tmpP.IsCyclic_new())
					//if(!tmpP.IsCyclic_Re_Move())

					{
//printf("SENDING new work order for good acyclic particle\n\n");
						for(int m=0;m<9900;m++) //fill SendArray work order to send to slave
						{
							if(tmpP.X[m])
							{
								SendArray[m]=1;
							}
							else
							{
								SendArray[m]=0;
							}
						}
						MPI_Send(&SendArray,9899,MPI_INT,status.MPI_SOURCE,iParticle,MPI_COMM_WORLD);
						CountEvaluations+=1;
						SendCount+=1;	
						RankAndParticle[status.MPI_SOURCE]=iParticle;
					}
					else
					{
//printf("SKIPPING cyclic parrticle\n");
						float CountArrows=0.0e0;
                                                for(int r=0;r<9900;r++)
                                                {
                                                        if(tmpP.X[r])
                                                        {
                                                                CountArrows+=1.0e0;
                                                        }
                                                }
						tmpP.Fitness=1.0e26*CountArrows;
					}
					iParticle+=1;
					ip++;	
				}//end while of finish sending of all particles left after seed



printf("\nRECEIVING OUTSTANDING WORK for STEP  %d\n\n",count);
				//receive outstanding fitness calculations
				for(int j=0;j<(SendCount-ReceiveCount);j++)
				{
					//if(1==1)	
					if (MPI_SUCCESS==MPI_Recv(&result,1,MPI_DOUBLE,MPI_ANY_SOURCE,MPI_ANY_TAG,MPI_COMM_WORLD,&status));
					{
					//	MPI_Recv(&result,1,MPI_DOUBLE,MPI_ANY_SOURCE,MPI_ANY_TAG,MPI_COMM_WORLD,&status);
						int returningRank=status.MPI_SOURCE;
						PSO_particle& retP=theParticles[RankAndParticle[returningRank]];
						retP.Fitness=result;
						tmp_BestFitness=retP.Fitness;
printf("receiving particle %d from rank %d, returned fitness=%e\n",RankAndParticle[returningRank],returningRank,result);
printf("Me Fitmess=%e NeighFitness=%e Previous Me Fitness=%e\n",retP.Fitness,retP.Fitness_N,retP.Fitness_P);
						if (tmp_BestFitness < TheBestFitnessEver)
						{
							TheBestOneEver=retP;
							TheBestFitnessEver=tmp_BestFitness;
							TheBestOneEver.myIndex=99;
							fprintf(best_particle,"\nBest Particle in Round %d\n",count);
							printf("New Best Particle in Round %d \n",count);
							for(int jj=0;jj<9900;jj++)
							{
        							if(TheBestOneEver.X[jj])
        							{
        						       		 fprintf(best_particle,"%d|",jj);
									printf("%d|",jj);	
        							}
							}
							fprintf(best_particle,"\nFitness = %e\n",TheBestOneEver.Fitness);
							fprintf(summary_file,"%d %e\n",CountEvaluations,TheBestOneEver.Fitness);
printf("\n");

						}
					}
				}
				
printf("\n UPDATING BESTS \n");
				//update bests
				for ( ip = theParticles.begin(); ip != theParticles.end(); ip++ ) 
				{
					PSO_particle& tmpP=(*ip);
					tmpP.UpdateBests();
				}
printf("\nMOVING PARTICLES\n");
				//move the particles
				if(count<1500)
				{
					for ( ip = theParticles.begin(); ip != theParticles.end(); ip++ ) 
					{
						PSO_particle& tmpP=(*ip);
						tmpP.Move();
						//printf("moved particle %d\n",tmpP.myIndex);
					}
				}
				count+=1;

				//write this step to file fitness vs time
				//str_new.Format(_T("%e %d\n"), TheBestFitnessEver,time(&ltime));
				//fwrite(str_new,sizeof(str_new),1,f_PSO_outfile);


			}while(count<1501);


			printf("sending die to slaves");
			//tell slaves to die
			for(int k=1;k<ntasks;k++)
			{
				MPI_Send(0,0,MPI_INT,k,DIETAG,MPI_COMM_WORLD);
			}
printf ("BEST FITNESS__________________________=%e\n",TheBestOneEver.Fitness);
//print out true indices of best particle and
//evaluate the hamming distance from
//ALARM
//int original[46]={72,73,108,109,146,182,331,332,443,516,517,548,550,554,584,591,664,665,693,702,776,808,834,847,849,850,887,903,924,937,993,997,1068,1072,1104,1108,1176,1183,1203,1210,1231,1268,1270,1274,1304,1311};

//ASIA
int original[8]={7,14,31,37,39,43,47,54};

int arrows_in_original=8;

int c1,c2,ham;//ham counters
c1=0;c2=0;

printf("trues of best particle\n");
for(int j=0;j<9900;j++)
{
	if(TheBestOneEver.X[j])
	{
		c2+=1;//count arrows of pso bn
		printf("%d|",j);
		for(int i=0;i<arrows_in_original;i++)
		{
			if(j==original[i])
			{
				c1+=1;//count matches
				break;
			}
		}
	}
}
ham=(c2-c1)+(arrows_in_original-c1);

printf("\n");

fprintf(summary_file,"Number of Evaluations=%d",CountEvaluations);
fprintf(summary_file,"Hamming Distance=%d",ham);

fclose(master_file);
fclose(best_particle);
fclose(summary_file);
	return 1;
}

void slave(int myrank)

{
   	FILE* slave_file;
	
	char buffer[12];	
	string filename;
	int bytesWritten=sprintf(buffer,"slave_%d.txt",myrank);
	filename.append(buffer,0,bytesWritten);
	slave_file=fopen(buffer,"wt");
 
	double		result;
	double fitness,netscore;
    int			work[9899];
    MPI_Status		status;

	C1AgentDoc slaveAgent=C1AgentDoc();
fprintf(slave_file,"SLAVE of rank %d CREATED AGENT\n",myrank);
PSO_particle myParticle=PSO_particle(slaveAgent.m_oaNodes.size(),-1);
fprintf(slave_file,"AND CREATED SLAVE WORKING PARTICLE----------------\n\n");



    for (;;) {

fprintf(slave_file,"\nSLAVE of rank %d WAITING\n",myrank);
	MPI_Recv(&work, 9899, MPI_INT, 0, MPI_ANY_TAG, //9899 is max size of binary string
		MPI_COMM_WORLD, &status);



/*
 * Check the tag of the received message.
 */
	if (status.MPI_TAG == DIETAG) {
	   	printf("\nSlave rank=%d DYING",myrank);	
		fclose(slave_file); 
		return;
	}
fprintf(slave_file,"\nSlave rank=%d received work order to calculate fitness of particle %d\n",myrank,status.MPI_TAG);


	for (int i=0;i<9900;i++)
	{
		if(work[i]==1)
		{
			myParticle.X[i]=true;
			fprintf(slave_file," %d |",i);
		}
		else
		{
			myParticle.X[i]=false;
		}	
	}
	myParticle.Make_PlusN_Particle();
	slaveAgent.RemoveAllArrows(true);	
	slaveAgent.ClearPermutations();
	slaveAgent.CalFirstLevelProbs();
	slaveAgent.ImprintNetworkFromPSO_particle(&myParticle);
	slaveAgent.LookForNewSegmentRecords();
	slaveAgent.CreateCPT();
	slaveAgent.UpdateNetwork();
	netscore=slaveAgent.CalculateNetworkScore();
	fitness=(1.0/netscore);
	result = fitness;

	MPI_Send(&result, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
	fprintf(slave_file,"\nresult of %e being sent by node rank=%d\n",fitness,myrank);
    }
}
