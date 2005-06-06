//#include </usr/local/mpich/include/mpi.h>
#include </usr/local/include/mpi.h>
/*#include </opt/SUNWhpc/HPC4.0/include/mpi++.h>*/
#include "1AgentDoc.h"
#include "PSO_particle.h"
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
using std::vector;

#define WORKTAG		1
#define DIETAG		1000	

int master(char* fn_case,int s_cases,int s_nodesizes,int s_nodenames,int pass_pso_steps,int pass_neighsize,int pass_NumParticles);
//int master(char* fn_case,int s_cases,char* fn_nodesizes,int s_nodesizes,char* fn_nodenames,int s_nodenames,int pass_pso_steps,int pass_neighsize,int pass_NumParticles);
void slave(int myrank,char* cases_filename);
//void slave(int myrank,char* cases_filename,char* nodesizes_filename,char* nodenames_filename);
//bool CheckParticle(C1AgentDoc TestAgent, int *work);

main( int argc, char  **argv)  {
	int myrank;

 	MPI_Init(&argc, &argv);     /* initialize MPI */
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);  /* always use this for MPI_COMM */  /* process rank, 0 thru N-1 */

	int pass_pso_steps;
	int pass_neighsize;
	int pass_NumParticles;
    char* cases_filename;
        ////char* nodenames_filename;
        ////char* nodesizes_filename;
	int i_cases;
	int i_nodesizes;
	int i_nodenames;
	printf("argc=%d\n",argc);

    if(argc!=5){
		printf("Usage: %s pso_steps particles neighsize cases_filename\n",argv[0]);
		//printf("Usage: %s pso_steps particles neighsize cases_filename nodenames_filename nodesizes_filename\n",argv[0]);
        printf("USING DEFAULTS\n");
        pass_pso_steps=100;
        pass_NumParticles=4;
        pass_neighsize=0;           //0 is global
        i_cases=9;
        i_nodenames=13;
        i_nodesizes=13;
    }
	else{
		pass_NumParticles=atoi(argv[2]);
        pass_neighsize=atoi(argv[3]);      //0 is global
        pass_pso_steps=atoi(argv[1]);
		cases_filename=argv[4];
		printf("argv cases filename = %s\n",argv[4]);
        i_nodenames=13;
        ////nodenames_filename=argv[5];
        i_nodesizes=13;
        ///nodesizes_filename=argv[6];
    }


	srand( (unsigned)time( NULL ) );	
	printf("hello my rank is %d \n",myrank);	

    if (myrank == 0 && argc==5) {
		master(cases_filename,i_cases,i_nodesizes,i_nodenames,pass_pso_steps,pass_neighsize,pass_NumParticles);
		//master(cases_filename,i_cases,nodesizes_filename,i_nodesizes,nodenames_filename,i_nodenames,pass_pso_steps,pass_neighsize,pass_NumParticles);
		printf("\nMaster is finished\n\n");
    }
 
    else if(myrank == 0 && argc!=5) {
		master("simpletest.dat",i_cases,i_nodesizes,i_nodenames,pass_pso_steps,pass_neighsize,pass_NumParticles);
		//master("cases.dat",i_cases,"nodesizes.dat",i_nodesizes,"nodenames.dat",i_nodenames,pass_pso_steps,pass_neighsize,pass_NumParticles);
		printf("\nMaster is finished\n\n");
	} 
    else if(argc==5) {
		slave(myrank,cases_filename);
		//slave(myrank,cases_filename,nodesizes_filename,nodenames_filename);
		//printf("Slave is finished\n"); 
    } 
    else {
		slave(myrank,"simpletest.dat");
		//slave(myrank,"cases.dat","nodesizes.dat","nodenames.dat");
        //printf("Slave is finished\n"); 
	} 

    MPI_Finalize();		/* cleanup MPI */

}  // End of Main


int master(char* fn_case,int s_cases,int s_nodesizes,int s_nodenames,int pass_pso_steps,int pass_neighsize,int pass_NumParticles)  {
//int master(char* fn_case,int s_cases,char* fn_nodesizes,int s_nodesizes,char* fn_nodenames,int s_nodenames,int pass_pso_steps,int pass_neighsize,int pass_NumParticles)

	printf("cases file= %s\n",fn_case);
	FILE*  master_file;
    FILE* summary_file;
	FILE* best_particle;
    master_file=fopen("master.txt", "wt");

	int pso_steps;
	int neighsize;
	int NumParticles;
	char best_filename[255];
	char summary_filename[255];

	pso_steps=pass_pso_steps;	
    NumParticles=pass_NumParticles;
    neighsize=pass_neighsize;                 //0 is global

	sprintf(best_filename,"best_%d_%d_%d.txt",pso_steps,NumParticles,neighsize);
	sprintf(summary_filename,"best_%d_%d_%d.txt",pso_steps,NumParticles,neighsize);
	////printf("nodenames file= %s\n",best_filename);
	printf("summary file= %s\n",summary_filename);
	
	summary_file=fopen(summary_filename,"wt");
    best_particle=fopen(best_filename,"wt");
	printf("summary file= %s\n",summary_filename);

	double *TwoBitFitnesses;   //can track single arc network fitnesses for up to 100 nodes
	double *CrossCorrelations; //pairwise cross correlations
	TwoBitFitnesses = new double[10000];
	CrossCorrelations = new double[1369];//just big enough for ALARM

	for (int i=0;i<10000;i++){
		TwoBitFitnesses[i]=0.0;
	}
	
	for (int i=0;i<1369;i++){
		CrossCorrelations[i]=10.0;
    }
	
	double LO_2bit_significance=1.0e-01;
	bool ExecuteLocalOptimizer=false;
	int CountEvaluations=0;
	int SendArray[14999]; //data to be sent as work order -- "binary sting"
	//int SendDArray[14999] = {0};
    int	ntasks, rank, work;
    double result;

    MPI_Status		status;
    MPI_Comm_size(
	    MPI_COMM_WORLD,	/* always use this */
	    &ntasks);		/* #processes in application */
      
    printf("INITING MASTER AGENT\n");

	C1AgentDoc	masterAgent=C1AgentDoc(fn_case);
	
	//C1AgentDoc	masterAgent=C1AgentDoc(fn_nodesizes,fn_nodenames);
	printf("Number of records=%d <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<",masterAgent.m_CaseRecords.RecordCount());

	//int pso_steps=1500;
	int n;
	vector<PSO_particle> theParticles; //need (40) here?
	PSO_particle TheBestOneEver=PSO_particle(masterAgent.m_oaNodes.size(),99);
	double TheBestFitnessEver=1.0e25;
	//create the particles and set the pointer to the parent array

	printf("creating n particles\n");

	for ( n=0;n<NumParticles;n++) { //create n particles
		PSO_particle tmpP=PSO_particle(masterAgent.m_oaNodes.size(),n);
		tmpP.p_PointersToAllParticles=&theParticles;
		theParticles.push_back(tmpP);
		//theParticles[n]=tmpP;
	}
	
	//fprintf(master_file,"creating particle neighborhoods\n");
	//define the neighborhoods of all the particles
	std::vector<PSO_particle>::iterator ip;
	
	for ( ip = theParticles.begin(); ip != theParticles.end(); ip++ ) {
		PSO_particle& tmpP=(*ip);
		tmpP.DefineMyNeighborhood(neighsize,tmpP.myIndex);//neighborhood size is 0/global on each side
	}

	//the main loop of the PSO optimization
	printf("the main loop of the PSO optimization");
	vector<int> RankAndParticle;//(40);//ie : RankAndParticle[4]=22 means that process 4 has particle 22
	RankAndParticle.reserve(ntasks);	
	//RankAndParticxl will have a size equal to the number of procresses
	int SendCount,ReceiveCount;
	SendCount=0;ReceiveCount=0;	
	int count=0; //step counter for PSO Optimization, max steps at end of do/while loop
	double tmp_NetworkScore,tmp_BestFitness;
	//fprintf(master_file,"MAIN LOOP OF PSO OPTIMIZATION\n");
	
	do {
		ip = theParticles.begin();
		// 
		// seed slaves 
		//
		SendCount=0;
		ReceiveCount=0;	
		rank=1; //rank 0 is me
		int iParticle=0;
		
		while (ip!=theParticles.end() && rank < ntasks) {
			PSO_particle& tmpP=(*ip);
				
			//----------------------------------------------------------------	
			//preprocess particle, remove 2 bit cyles (LOCAL OPTINMIZER)
			//more complex preferential removal of 2 bit cycles LOCAL OPTIMIZER GOES HERE
			//----------------------------------------------------------------	
            int CountUncorrelatedPairs=0; 
			
			while(tmpP.Has2bitCycle() && ExecuteLocalOptimizer)  {
				int arrow1,arrow2,k;
                int NumOfNodes=masterAgent.m_oaNodes.size();
                arrow1=tmpP.bit[0];arrow2=tmpP.bit[1];	 //these arrow indexes are PlusN indexes
														 //if( TwoBitFitnesses[arrow1] == 0.0){
														 //single arc nets NOT calculated
				if( CrossCorrelations[arrow1] == 10.0) { //single arc nets NOT calculated	
	                                                	 //need to calculate 2 single arc network fitness values
					for(int m=0;m<15000;m++) { //fill work order to send
						SendArray[m]=0;
                    }
                    
					SendArray[arrow1]=1;
					int TagAs2BitCalculation=42;
					int TagAsCrossCorrCalc=43;
						
					printf("Master Send 1..TasAsCrossCorrCalc..\n");

					MPI_Send(&SendArray,14999,MPI_INT,rank,TagAsCrossCorrCalc,MPI_COMM_WORLD);
					//printf(" Master is waiting to receive\n");
					MPI_Recv(&result,1,MPI_DOUBLE,rank,MPI_ANY_TAG,MPI_COMM_WORLD,&status);
				    //printf("Master Received\n");

					CrossCorrelations[arrow1]=fabs(result);
					CrossCorrelations[arrow2]=fabs(result);
                                                }
					int NodeBlock,SpacesIn,PlainParticleIndex;
					printf("%e	%d	%d\n", CrossCorrelations[arrow1],arrow1,arrow2);
					
					if( CrossCorrelations[arrow1]<.01)  {
						CountUncorrelatedPairs+=1;	
						//convert to plain particle index
						k=arrow1;		
                        NodeBlock=(int)(k/NumOfNodes);
                        SpacesIn=k-(NodeBlock*NumOfNodes);
                        PlainParticleIndex=NodeBlock*(NumOfNodes-1)+SpacesIn;
                          
						if(SpacesIn>NodeBlock){
							PlainParticleIndex-=1;
                        }
                          
						tmpP.X[PlainParticleIndex]=false;
						k=arrow2;           
                        NodeBlock=(int)(k/NumOfNodes);
                        SpacesIn=k-(NodeBlock*NumOfNodes);
                        PlainParticleIndex=NodeBlock*(NumOfNodes-1)+SpacesIn;
                           
						if(SpacesIn>NodeBlock) {
						PlainParticleIndex-=1;
                        }
                            
						tmpP.X[PlainParticleIndex]=false;

					}
					else { //break the cycle randomly
						if(rand()/RAND_MAX>RAND_MAX/2) {
							k=arrow1;
						}
						else {
							k=arrow2;
						}	
						NodeBlock=(int)(k/NumOfNodes);
                        SpacesIn=k-(NodeBlock*NumOfNodes);
                        PlainParticleIndex=NodeBlock*(NumOfNodes-1)+SpacesIn;
                        
						if(SpacesIn>NodeBlock) {
							PlainParticleIndex-=1;
                        }
                        tmpP.X[PlainParticleIndex]=false;
					} // For else :  break the cycle rondomly

				} // For if
				
				//---------------------------------------------------------------------------
				//	printf("removed %d uncorrelated pairs in seed\n",CountUncorrelatedPairs);	
				//---------------------------------------------------------------------------
			
				if(!tmpP.IsCyclic())  {  //use this to allow cyclic particles in the swarm
					//if(!tmpP.IsCyclic_new()) //usr this to recursively randomly (doing 2 bits first , but still random) remove cycles
					//if(!tmpP.IsCyclic_Re_Move()) //use this to keep re moving a particle until an acyclic particle results
					//this could be dangerous as it may be highly probable to create cyclic particles
					//fprintf(master_file,"\n in seed -----------ACYCLIC sending particle %d to rank %d in round %d\n",iParticle,rank,count);
					//printf("%x with %d arrows after removing cycles SENDING IN SEED ROUND %d\n",&(*ip),tmpP.ArrowCount(),count);
				
					for(int m=0;m<15000;m++) { //fill work order to send
						if(tmpP.X[m]) {
							SendArray[m]=1;	
						}
						else {
							SendArray[m]=0;
						}
					
					//if(m<1332){
					//printf("%d",SendArray[m]);}
				
					}   // For for
			
					//printf("Master Send 2..Particle..\n");
					MPI_Send(&SendArray,14999,MPI_INT,rank,iParticle,MPI_COMM_WORLD);
					//printf("Master Sent");
					CountEvaluations+=1;
					RankAndParticle[rank]=iParticle;
					//RankAndParticle.push_back(iParticle);
					rank+=1;
					SendCount+=1;
				} // For if
				else {
				//simple let fly of cyclic particles
				//fprintf(master_file,"\nin seed --------CYCLIC skipping  cyclic particle %d\n",iParticle);
				float CountArrows=0.0e0;
		
				for(int r=0;r<15000;r++) {
					if(tmpP.X[r]) {
						CountArrows+=1.0e0;
						//fprintf(master_file,"%d|",r);
					}
				}
			
				tmp_NetworkScore=1.0e-13;
				float ns=tmp_NetworkScore;
				tmpP.Fitness=(1.0/ns)*CountArrows;
			}

			iParticle+=1;	
			ip++;
		} //end while of seed processes

		printf("\nDONE SEEDING processes\n\n");
		//are there still particles left to process?
		printf("ENTERING MAIN PUMP...\n");
	
		while (ip!=theParticles.end()) {
			//printf("Debug point..\n\n");
			//printf("IN  MAIN PUMP waiting to receive...Master is waiting to receive 2 \n\n");
			MPI_Recv(&result,1,MPI_DOUBLE,MPI_ANY_SOURCE,MPI_ANY_TAG,MPI_COMM_WORLD,&status);
			//printf("Master received 2\n");
			ReceiveCount+=1;	
			int returningRank=status.MPI_SOURCE;
			PSO_particle& retP=theParticles[RankAndParticle[status.MPI_SOURCE]];
			//printf("received particle %d from rank %d with a fitness=%e\n",RankAndParticle[returningRank],returningRank,result);					
			retP.Fitness=result;
			tmp_BestFitness=retP.Fitness;
			//printf("Me Fitmess=%e NeighFitness=%e Previous Me Fitness=%e\n",retP.Fitness,retP.Fitness_N,retP.Fitness_P);
			//track best overall particle ever
		
			if (tmp_BestFitness<TheBestFitnessEver) {
				TheBestOneEver=retP;//uses copy operator
				TheBestFitnessEver=tmp_BestFitness;
				TheBestOneEver.myIndex=99;
				//printf("new best particle\n");
				//fprintf(master_file,"New Best Particle\n");
			
				for(int kk=0;kk<15000;kk++) {
					if(TheBestOneEver.X[kk]) {
						printf("%d|",kk);
						//fprintf(master_file,"%d|",kk);
					}   // For if	
				} // For for kk
			} // For if 
			
			//printf("Begin send new work order\n");					
			PSO_particle& tmpP=(*ip);
				
			//----------------------------------------------------------------
			//preprocess particle, remove 2 bit cyles (LOCAL OPTINMIZER)
			//more complex preferential removal of 2 bit cycles LOCAL OPTIMIZER GOES HERE
			int CountUncorrelatedPairs=0; 	

			while(tmpP.Has2bitCycle() && ExecuteLocalOptimizer) {
				int arrow1,arrow2,k;
				int NumOfNodes=masterAgent.m_oaNodes.size();
				arrow1=tmpP.bit[0];arrow2=tmpP.bit[1];   //these arrow indexes are PlusN indexes
												     //if( TwoBitFitnesses[arrow1] == 0.0){//single arc nets NOT calculated
				if( CrossCorrelations[arrow1] == 10.0) { //single arc nets NOT calculated
                                                     //need to calculate 2 single arc network fitness values
					for(int m=0;m<15000;m++) {	         //fill work order to send
						SendArray[m]=0;
					} 
                    
					SendArray[arrow1]=1;
					int TagAs2BitCalculation=42;
					int TagAsCrossCorrCalc=43;
					//printf("Send 3..TagAsCrossCorrCalc..\n");
					MPI_Send(&SendArray,14999,MPI_INT,returningRank,TagAsCrossCorrCalc,MPI_COMM_WORLD);
					MPI_Recv(&result,1,MPI_DOUBLE,returningRank,MPI_ANY_TAG,MPI_COMM_WORLD,&status);
					CrossCorrelations[arrow1]=fabs(result);
					CrossCorrelations[arrow2]=fabs(result);
				} // For if CrossCorrelations

				int NodeBlock,SpacesIn,PlainParticleIndex;
			
				if( CrossCorrelations[arrow1]<.01) {
					CountUncorrelatedPairs+=1;
					//convert to plain particle index
					k=arrow1;
					NodeBlock=(int)(k/NumOfNodes);
					SpacesIn=k-(NodeBlock*NumOfNodes);
					PlainParticleIndex=NodeBlock*(NumOfNodes-1)+SpacesIn;
                
				if(SpacesIn>NodeBlock) {
					PlainParticleIndex-=1;
                }
                
				tmpP.X[PlainParticleIndex]=false;
                k=arrow2;
                NodeBlock=(int)(k/NumOfNodes);
                SpacesIn=k-(NodeBlock*NumOfNodes);
                PlainParticleIndex=NodeBlock*(NumOfNodes-1)+SpacesIn;
                
				if(SpacesIn>NodeBlock) {
					PlainParticleIndex-=1;
                }
                
				tmpP.X[PlainParticleIndex]=false;
            }
            
			else { //break the cycle radomly
				if(rand()/RAND_MAX>RAND_MAX/2) {        
					k=arrow1;      
                }
                else{
					k=arrow2;      
                }
                NodeBlock=(int)(k/NumOfNodes);
                SpacesIn=k-(NodeBlock*NumOfNodes);
                PlainParticleIndex=NodeBlock*(NumOfNodes-1)+SpacesIn;
                
				if(SpacesIn>NodeBlock) {
					PlainParticleIndex-=1;
                }
                
				tmpP.X[PlainParticleIndex]=false;
            }
		} 
		//-------------------------------------------------------------------------
		//printf("removed %d uncorrelated pairs in pump\n",CountUncorrelatedPairs);
		//-------------------------------------------------------------------------

		if(!tmpP.IsCyclic()) {
			//if(!tmpP.IsCyclic_new())
			//if(!tmpP.IsCyclic_Re_Move())
			//printf("SENDING new work order for good acyclic particle\n\n");
			for(int m=0;m<15000;m++)  { //fill SendArray work order to send to slave	
				if(tmpP.X[m]) {
					SendArray[m]=1;
				}
				else {
					SendArray[m]=0;
				}
			}
			//printf(" Master Send 4..Particle...\n");
						
			MPI_Send(&SendArray,14999,MPI_INT,returningRank,iParticle,MPI_COMM_WORLD);
						
			//	MPI_Send(&SendArray,14999,MPI_INT,returningRank,iParticle,MPI_COMM_WORLD);
			//printf("Master sent 4..");
			CountEvaluations+=1;
			SendCount+=1;	
			RankAndParticle[returningRank]=iParticle;
		}
		else {

			//printf("SKIPPING cyclic parrticle\n");
			float CountArrows=0.0e0;
			
			for(int r=0;r<15000;r++) {
				if(tmpP.X[r]) {
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
	
	for(int j=0;j<(SendCount-ReceiveCount);j++) {
		//if(1==1)	
		
		if (MPI_SUCCESS==MPI_Recv(&result,1,MPI_DOUBLE,MPI_ANY_SOURCE,MPI_ANY_TAG,MPI_COMM_WORLD,&status)); {
			//	MPI_Recv(&result,1,MPI_DOUBLE,MPI_ANY_SOURCE,MPI_ANY_TAG,MPI_COMM_WORLD,&status);
			//printf("Master received\n\n");
			int returningRank=status.MPI_SOURCE;
			PSO_particle& retP=theParticles[RankAndParticle[returningRank]];
			retP.Fitness=result;
			tmp_BestFitness=retP.Fitness;
			//printf("receiving particle %d from rank %d, returned fitness=%e\n",RankAndParticle[returningRank],returningRank,result);
			//printf("Me Fitness=%e NeighFitness=%e Previous Me Fitness=%e\n",retP.Fitness,retP.Fitness_N,retP.Fitness_P);
			
			if (tmp_BestFitness < TheBestFitnessEver)	{
				TheBestOneEver=retP;
				TheBestFitnessEver=tmp_BestFitness;
				TheBestOneEver.myIndex=99;
				fprintf(best_particle,"\nBest Particle in Round %d\n",count);
				//printf("New Best Particle in Round %d \n",count);
				
				for(int jj=0;jj<15000;jj++) {
        			if(TheBestOneEver.X[jj]) {
        				fprintf(best_particle,"%d|",jj);
						printf("%d|",jj);	
        			}
				}
			
				fprintf(best_particle,"\nFitness = %e\n",TheBestOneEver.Fitness);
				fprintf(summary_file,"%d %e\n\n",CountEvaluations,TheBestOneEver.Fitness);
			}
		}
	}
				
	printf("\n\nUPDATING BESTS \n");
	//update bests
	for ( ip = theParticles.begin(); ip != theParticles.end(); ip++ )  {
		PSO_particle& tmpP=(*ip);
		tmpP.UpdateBests();
	}
	
	printf("\nMOVING PARTICLES\n\n");
	//move the particles
	if(count<pso_steps+1) {
		for ( ip = theParticles.begin(); ip != theParticles.end(); ip++ )  {
			PSO_particle& tmpP=(*ip);
			tmpP.Move();
			//printf("moved particle %d\n",tmpP.myIndex);
		}
	}

	count+=1;
	//write this step to file fitness vs time
	//str_new.Format(_T("%e %d\n"), TheBestFitnessEver,time(&ltime));
	//fwrite(str_new,sizeof(str_new),1,f_PSO_outfile);

}while(count<pso_steps);

printf("sending die to slaves\n");
//tell slaves to die

for(int k=1;k<ntasks;k++)	{
	//printf("Send 5..Die...\n");
	MPI_Send(0,0,MPI_INT,k,DIETAG,MPI_COMM_WORLD);
}

printf ("BEST FITNESS__________________________=%e\n",TheBestOneEver.Fitness);
//print out true indices of best particle and
//evaluate the hamming distance from
//ALARM
//int original[46]={72,73,108,109,146,182,331,332,443,516,517,548,550,554,584,591,664,665,693,702,776,808,834,847,849,850,887,903,924,937,993,997,1068,1072,1104,1108,1176,1183,1203,1210,1231,1268,1270,1274,1304,1311};
//ALARM OUR DATA SET

int original[46]={433,443,470,476,509,550,584,623,650,656,702,727,734,774,792,793,812,828,829,848,865,886,901,922,942,961,997,1030,1035,1056,1070,1084,1095,1107,1109,1146,1161,1183,1197,1219,1237,1255,1263,1291,1311,1330};

//ASIA
//int original[8]={7,14,31,37,39,43,47,54};

//CAR
//int original[20]={6,7,10,40,57,60,61,106,124,125,212,222,238,247,275,301,302,303,304,305};

//CAR20
//int original[31]={114,120,133,134,140,153,154,160,173,174,180,193,194,200,213,214,220,233,253,273,279,280,293,313,319,320,333,353,359,360,373};

int arrows_in_original=46;
int c1,c2,ham;//ham counters
c1=0;c2=0;
printf("trues of best particle\n\n");

for(int j=0;j<15000;j++) {
	if(TheBestOneEver.X[j]) {
		c2+=1;//count arrows of pso bn
		printf("%d|",j);
		for(int i=0;i<arrows_in_original;i++) {
			if(j==original[i]) {
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

delete [] TwoBitFitnesses;

fclose(master_file);
fclose(best_particle);
fclose(summary_file);
return 0;

} // End of Master




void slave(int myrank,char* cases_filename) {
//void slave(int myrank,char* cases_filename,char* nodesizes_filename,char* nodenames_filename)
//FILE* slave_file;
	
char buffer[12];	
//	string filename;
//	int bytesWritten=sprintf(buffer,"slave_%d.txt",myrank);
//	filename.append(buffer,0,bytesWritten);
//	slave_file=fopen(buffer,"wt");
 
double		result;
double fitness,netscore;
int			work[14999];
MPI_Status		status;
C1AgentDoc slaveAgent=C1AgentDoc(cases_filename);
//C1AgentDoc slaveAgent=C1AgentDoc(nodesizes_filename,nodenames_filename);
printf("SLAVE of rank %d CREATED AGENT\n",myrank);
PSO_particle myParticle=PSO_particle(slaveAgent.m_oaNodes.size(),-1);
printf("AND CREATED SLAVE WORKING PARTICLE----------------\n\n");


for (;;) {

	//printf("\nSLAVE of rank %d WAITING\n",myrank);
	MPI_Recv(&work, 14999, MPI_INT, 0, MPI_ANY_TAG, //14999 is max size of binary string
	MPI_COMM_WORLD, &status);
	//printf("Slave received\n");

/*
 * Check the tag of the received message.
 */
	if (status.MPI_TAG == DIETAG) {
	   	//printf("\nSlave rank=%d DYING\n",myrank);	
		//fclose(slave_file); 
		return;
	}
	//printf("\nSlave rank=%d received work order to calculate fitness of particle %d\n",myrank,status.MPI_TAG);
	//printf("Preparing work..\n");

	for (int i=0;i<15000;i++) {
		if(work[i]==1) {
			myParticle.X[i]=true;
			//fprintf(slave_file," %d |",i);
			//printf("myParticle.X[i] = true \n");
		}
		else {
			//	printf("myParticle.X[i] = false \n");
			myParticle.X[i]=false;
		}	
	}

	myParticle.Make_PlusN_Particle();
	slaveAgent.RemoveAllArrows(true);	
	slaveAgent.ClearPermutations();
	//printf("Calculating first level probs\n");
	slaveAgent.CalFirstLevelProbs();
	//printf("Imprinting Network\n");
	slaveAgent.ImprintNetworkFromPSO_particle(&myParticle);

	//printf("Valid Network %d \n", slaveAgent.Valid_Network());

	if(slaveAgent.Valid_Network()) {
		//printf("Network is valid\n");
		//printf("Looking for New SegmentRecords\n");
		slaveAgent.LookForNewSegmentRecords();
		//printf("Creating CPTS\n");
		slaveAgent.CreateCPT();
		//printf("Updating Network\n");
		slaveAgent.UpdateNetwork();

		if(status.MPI_TAG==42) {
			netscore=slaveAgent.CalculateNetworkScore(1);//return jason special scoring
			//printf("Calculating score with %d\n",1);
		}
		else if(status.MPI_TAG==43) {
			netscore=slaveAgent.CalculateNetworkScore(2);//return correlation coefficient
			//printf("Calculating score with %d\n",2);
		}
		else {
			netscore=slaveAgent.CalculateNetworkScore(0);//calculate normally	
			//printf("Calculating score with %d\n",3);
		}
	}
	else {
		//printf("not a valid network \n");
		/*slaveAgent.RemoveAllArrows(true);
		slaveAgent.ClearPermutations();
		slaveAgent.CalFirstLevelProbs();
		slaveAgent.LookForNewSegmentRecords();
		slaveAgent.CreateCPT();
		slaveAgent.UpdateNetwork();
		netscore=slaveAgent.CalculateNetworkScore(0); */
	
		netscore = 1.0e-13f;
	}

	fitness=(1.0/netscore);
	result = fitness;

	if(status.MPI_TAG==43){
		result=netscore;
	}
	
	//printf("Slave is sending the result\n");
	MPI_Send(&result, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
	//printf("\nresult of %e is sent by node rank=%d\n",fitness,myrank);
} // End of Infinite For Loop

} // End of Slave

/*
bool CheckParticle(C1AgentDoc TestAgent, int *work)
{
	
	PSO_particle myParticle=PSO_particle(TestAgent.m_oaNodes.size(),-1);

	
	for (int i=0;i<15000;i++)
	{
		if(work[i]==1)
		{
			myParticle.X[i]=true;
		}
		else
		{
			myParticle.X[i]=false;
		}	
	}

	myParticle.Make_PlusN_Particle();
	TestAgent.RemoveAllArrows(true);	
	TestAgent.ClearPermutations();
	//printf("Calculating first level probs\n");
	//slaveAgent.CalFirstLevelProbs();
	//printf("Imprinting Network\n");
	TestAgent.ImprintNetworkFromPSO_particle(&myParticle);
	
	if(TestAgent.Valid_Network())
	{
		printf("Network is valid in master\n");
		return true;
	}
	else
	{
		printf("Network is NOT valid..in master\n");
		return false;
	}
}
*/
