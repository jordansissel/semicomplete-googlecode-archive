#include "PSO_particle.h"
#include <math.h>


PSO_particle::PSO_particle()
{
//printf("particle default constructor\n");
}



PSO_particle::PSO_particle(int Nodes,int passmyIndex) //Nodes is number of nodes in network

{

	myIndex=passmyIndex;	
	m_MAX_ARROWS=2*Nodes;
	ParticleDimension=Nodes*(Nodes-1); //count, not an array index
	ParticleDimensionPlusN=Nodes*Nodes;
	N=Nodes; //count, NOT in index
	int i;
	int CountFailed=0;


	//initialize N/2 arrows (heuristic guess of arrow count initialization)
	float Wanted_RAND_MAX=(float)(ParticleDimension-1); 
	float percent;
//	srand( (unsigned)time( NULL ) );
	do
	{
		for ( i=0;i<ParticleDimension;i++)//no arrows
		{//no arrows
			X[i]=false;//no arrows
		}//no arrows

		for ( i=0;i<N/2;i++)
		{
			percent=(float)rand()/RAND_MAX;
			//int index=rand();
			int index=(int)(percent*Wanted_RAND_MAX);
			X[index]=true;
		}

		Make_PlusN_Particle();
		CountFailed+=1;
		if(CountFailed>10)
		{
			return;
		}
	}while(IsCyclic());
	//initialize neighborhood and individual bests
	for (i=0;i<ParticleDimension;i++)
	{
		XN[i]=X[i];
		XP[i]=X[i];
		//V[i]=0.0f;
	}
	//randomize initial velocities
	for (i=0;i<ParticleDimension;i++)
	{
		V[i]=8.0f*(float)rand()/(float)RAND_MAX;
		if( rand()<RAND_MAX/2 ){
			V[i]=-1.0*V[i];
		}
	}
	Fitness=1.0e30;
	Fitness_P=1.0e30;
	Fitness_N=1.0e30;



}

PSO_particle::~PSO_particle()
{
}

bool PSO_particle::IsCyclic_original()
{
	bool temp_PlusN[9999];
	for (int k=0;k<N*N;k++)
	{
		temp_PlusN[k]=X_PlusN[k];
	}

	int NumberOfSets=1;
	while(NumberOfSets>0)
	{
		NumberOfSets=0;
		for (int i=0;i<N;i++)
		{
			bool AllFalseBlock=true; //a node with no parents
			for (int j=0;j<N;j++)
			{
				if (temp_PlusN[i*N+j])	
				{
					AllFalseBlock=false;
					break;
				}
			}
			if (AllFalseBlock)
			{
				for (int m=0;m<N;m++)
				{
					if (temp_PlusN[m*N+i])
					{
						NumberOfSets+=1;
						temp_PlusN[m*N+i]=false;
					}
				}
			}
		}	
	}

	//are they all zeros now? if they are then the network is not cyclic
	for ( int k=0;k<N*N;k++)
	{
		if(temp_PlusN[k])
		{
			CyclicPlusNIndex=k;
			return true;
		}
	}
	return false;
}
bool PSO_particle::IsCyclic_new()
{
	int stuckCount=0;
	while(IsCyclic())
	{
		int k=CyclicPlusNIndex;
		//eliminate this cycle by deleting this arrow from the particle
                        int NodeBlock=(int)(k/N);
                        int SpacesIn=k-(NodeBlock*N);
                        int PlainParticleIndex=NodeBlock*(N-1)+SpacesIn;
                        if(SpacesIn>NodeBlock)
                        {
                                PlainParticleIndex-=1;
                        }
                        X[PlainParticleIndex]=false;
                        Make_PlusN_Particle();
		stuckCount+=1;
		//if(stuckCount>3)
		//{
		//	return false;
		//}
	}

        Make_PlusN_Particle();
        while(ArrowCount()>m_MAX_ARROWS )
        {
              Prune1Arrow();
        }
	Make_PlusN_Particle();

	return false;
}
bool PSO_particle::IsCyclic_Re_Move()
{
        int stuckCount=0;
        while(IsCyclic())
        {
        	for (int i=0;i<ParticleDimension;i++)
        	{
                	X[i]=XP[i];
		}
		Move();
		stuckCount+=1;
		if(stuckCount>300)
		{
			IsCyclic_new();
		}
	}
        return false;
}

bool PSO_particle::TooManyArrows()
{
	int CountArrows=0;
	for (int i=0;i<ParticleDimension;i++)
	{
		if (X[i])
		{
			CountArrows+=1;
		}
		if (CountArrows>m_MAX_ARROWS)
		{
			return true;
		}
	}
	return false;

}

void PSO_particle::Make_PlusN_Particle()
{
	int CountArrows=0;
	for (int i=0;i<N;i++)
	{
		for (int j=0;j<N;j++)
			if (i==j)
			{
				X_PlusN[i*N+j]=false;
			}
			else if ( i < j )
			{
				X_PlusN[i*N+j]=X[i*(N-1)+j-1];
			}
			else if( i > j )
			{
				X_PlusN[i*N+j]=X[i*(N-1)+j];
			}
	}
}

void PSO_particle::DefineMyNeighborhood(int SizeOfNeighborhood,int pass_myIndex)
{
//printf("defining neighborhood of particle\n");	
	myIndex=pass_myIndex;
	if (SizeOfNeighborhood==0 || SizeOfNeighborhood>p_PointersToAllParticles->size())
	{
		std::vector<PSO_particle>::iterator ip;
		for ( ip = p_PointersToAllParticles->begin(); ip != p_PointersToAllParticles->end(); ip++ ) 
		//for(int i=0;i<p_PointersToAllParticles->size();i++)
		{
			if((*ip).myIndex!=myIndex)//add everyone but me
			{
				p_PointersToNeighborhoodParticles.push_back(&(*ip));
		
			}
		}
	}
	else
	{
		for(int i=1;i<=SizeOfNeighborhood;i++)
		{//SizeOfNeighborhood is the number of particles on each side of this particle
			//so the number of neighbors is 2*Size
			int NeighborIndex=myIndex + i;
			if (NeighborIndex>SizeOfNeighborhood-1)
			{
				NeighborIndex	=NeighborIndex-SizeOfNeighborhood;
			}
			p_PointersToNeighborhoodParticles.push_back( &( (*p_PointersToAllParticles)[NeighborIndex]) );
			
			NeighborIndex=myIndex - i;
			if (NeighborIndex<0)
			{
				NeighborIndex	=NeighborIndex+SizeOfNeighborhood;
			}
			p_PointersToNeighborhoodParticles.push_back( &( (*p_PointersToAllParticles)[NeighborIndex]) );

		}
	}
}

void PSO_particle::Move()
{
//printf("BEGIN PSO particle(%x) move  ARROWS BEFORE= %d\n",this,ArrowCount());
//print out first 100 dimensions
for(int i=0;i<100;i++){
        if(X[i]){
                //printf("1|");
        }else{
                //printf("0|");
        }
}//printf("\n");
	float Wanted_RAND_MAX=(float)(1); 
	float percent,phi1,phi2,rho;
	//srand( (unsigned)time( NULL ) );
	float sigmoid;
	for (int i=0;i<ParticleDimension;i++)
	{
		percent=(float)rand()/(float)RAND_MAX;
		phi1=percent*Wanted_RAND_MAX;

		percent=(float)rand()/(float)RAND_MAX;
		phi2=percent*Wanted_RAND_MAX;

		percent=(float)rand()/(float)RAND_MAX;
		rho=percent*1.0f; //for sigmoid comparison

		//V[i]=V[i]+2.7*phi1*( (float)XP[i] - (float)X[i] ) + 1.3*phi2*( (float)XN[i] - (float)X[i] );
		V[i]=V[i]+phi1*( (float)XP[i] - (float)X[i] ) + phi2*( (float)XN[i] - (float)X[i] );


		//maximum velocity impose
		if( V[i] < -8.0f )
		{
			V[i]=-8.0f;
		}
		else if( V[i] > 8.0f )
		{
			V[i]=8.0f;
		}

		sigmoid=1.0f/(1.0f+ exp(-V[i]));
		if(rho<sigmoid)
		{
			X[i]=true;
		}
		else
		{
			X[i]=false;
		}


	}
	
//	Make_PlusN_Particle();
//        IsCyclic_new();

//printf("AFTER MOVE BUT BEFORE PRUNE ARROWS  %d\n",ArrowCount());
//	while(ArrowCount()>m_MAX_ARROWS )
//	{
//		Prune1Arrow();
//	}

	Make_PlusN_Particle();
//

//print out first 100 dimensions
//for(int i=0;i<100;i++){
//        printf("%e|",V[i]);
//}printf("\n");
//print out average velocity of the swarm and standard deviation
double AverageVelocity,SigVelocity;
AverageVelocity=0;SigVelocity=0;
for(int i=0;i<N;i++){
        AverageVelocity+=V[i]/(N);
}
for(int i=0;i<N;i++){
        SigVelocity+=(V[i]-AverageVelocity)*(V[i]-AverageVelocity)/N;
}
SigVelocity=sqrt(SigVelocity);
//printf("Average Velocity=%e  Sig velocity=%e\n",AverageVelocity,SigVelocity);
//for(int i=0;i<100;i++){
//        if(X[i]){
//                printf("1|");
//        }else{
//                printf("0|");
//        }
//}printf("\n");
//printf("END PSO Particle(%x) MOVE ARROWS AFTER=  %d\n",this, ArrowCount());

}

void PSO_particle::UpdateBests()
{
	if (Fitness<Fitness_P) //less error in new position
	{
		Fitness_P=Fitness;
		for (int i=0;i<ParticleDimension;i++)
		{
			XP[i]=X[i];
		}
	}

	//for each paritcle in my neighborhood
	std::list<PSO_particle*>::iterator ip;
	for ( ip = p_PointersToNeighborhoodParticles.begin(); ip != p_PointersToNeighborhoodParticles.end(); ip++ ) 
	{
		//dereference of ip is a pointer. need to deref IT to get particle
		PSO_particle& neighbor=*(*ip);
//printf("Updating Bests Fitness_N=%e neighbor's fitness=%e\n",Fitness_N,neighbor.Fitness);
		if(neighbor.Fitness < Fitness_N)
		{
			Fitness_N=neighbor.Fitness;
			for (int k=0;k<ParticleDimension;k++)
			{
				XN[k]=neighbor.X[k];
			}
		}
	}
}
void PSO_particle::operator =(const PSO_particle &pParticle)
{
	
	for (int i=0;i<pParticle.ParticleDimension;i++)
	{
		X[i]=pParticle.X[i];
		V[i]=pParticle.V[i];
		XN[i]=pParticle.XN[i];
		XP[i]=pParticle.XP[i];
	}
	this->Fitness=pParticle.Fitness;
	this->Fitness_N=pParticle.Fitness_N;
	this->Fitness_P=pParticle.Fitness_P;
	this->m_MAX_ARROWS=pParticle.m_MAX_ARROWS;
	this->myIndex=pParticle.myIndex;
	this->N=pParticle.N;
	this->p_PointersToAllParticles=pParticle.p_PointersToAllParticles;
	this->p_PointersToNeighborhoodParticles=pParticle.p_PointersToNeighborhoodParticles;
	this->ParticleDimension=pParticle.ParticleDimension;
	this->ParticleDimensionPlusN=pParticle.ParticleDimensionPlusN;
	


	Make_PlusN_Particle();	

}
PSO_particle::PSO_particle(const PSO_particle &pParticle)
{
	
	for (int i=0;i<pParticle.ParticleDimension;i++)
	{
		X[i]=pParticle.X[i];
		V[i]=pParticle.V[i];
		XN[i]=pParticle.XN[i];
		XP[i]=pParticle.XP[i];
	}
	this->Fitness=pParticle.Fitness;
	this->Fitness_N=pParticle.Fitness_N;
	this->Fitness_P=pParticle.Fitness_P;
	this->m_MAX_ARROWS=pParticle.m_MAX_ARROWS;
	this->myIndex=pParticle.myIndex;
	this->N=pParticle.N;
	this->p_PointersToAllParticles=pParticle.p_PointersToAllParticles;
	this->p_PointersToNeighborhoodParticles=pParticle.p_PointersToNeighborhoodParticles;
	this->ParticleDimension=pParticle.ParticleDimension;
	this->ParticleDimensionPlusN=pParticle.ParticleDimensionPlusN;
	


	Make_PlusN_Particle();	

}

void PSO_particle::Prune1Arrow()
{
	float Wanted_RAND_MAX=(float)(ParticleDimension-1); 
	float percent;
	bool woohoo=false;
	//srand( (unsigned)time( NULL ) );
	do
	{
		percent=(float)rand()/(float)RAND_MAX;
		int index=(int)(percent*Wanted_RAND_MAX);
		if(X[index])
		{
			X[index]=false;
			woohoo=true;
		}
	}while(!woohoo);
	return;


}

int PSO_particle::ArrowCount()
{
	int CountArrows=0;
	for (int i=0;i<ParticleDimension;i++)
	{
		if (X[i])
		{
			CountArrows+=1;
		}
		
	}
	return CountArrows;

}
bool PSO_particle::IsCyclic()
{
        bool temp_PlusN[9999];
        for (int k=0;k<N*N;k++)
        {
                temp_PlusN[k]=X_PlusN[k];
        }

        int NumberOfSets=1;
        while(NumberOfSets>0)
        {
                NumberOfSets=0;
                for (int i=0;i<N;i++)
                {
                        bool AllFalseBlock=true; //a node with no parents
                        for (int j=0;j<N;j++)
                        {
                                if (temp_PlusN[i*N+j])
                                {
                                        AllFalseBlock=false;
                                        break;
                                }
                        }
                        if (AllFalseBlock)
                        {
                                for (int m=0;m<N;m++)
                                {
                                        if (temp_PlusN[m*N+i])
                                        {
                                                NumberOfSets+=1;
                                                temp_PlusN[m*N+i]=false;
                                        }
                                }
                        }
                }
        }

	//count arrows participating in cycles
        //and find , if there exists, a 2 bit cycle index
        int CountCycleArrows=0;
        int Count2bitCycleArrows;
        for ( int k=0;k<N*N;k++)
        {
                if(temp_PlusN[k]){
                        CountCycleArrows+=1;
                        //does this arrow have a 2 bit partneer?
                        int q=N*(k-((int)(k/N)*N))+(int)(k/N);//q is the 2 bit arrow index in the plusN partic
                        if(temp_PlusN[q]){
                                Count2bitCycleArrows+=1;//add another 2 bit arrow
                        }
                }
        }
        //if the count of 2 bit indices is non-zero then return a random 2 bit index

        if(Count2bitCycleArrows>0){
                //generate a random number based on Count2bitCycleArrows
                int gn=(int)(  (float)rand()/(float)RAND_MAX*Count2bitCycleArrows+1);

                if(gn==0){gn=1;}if(gn>Count2bitCycleArrows){gn=Count2bitCycleArrows;}

                Count2bitCycleArrows=1;
                for ( int k=0;k<N*N;k++){
                        int q=N*(k-((int)(k/N)*N))+(int)(k/N);//q is the 2 bit arrow index in the plusN partic
                        if(temp_PlusN[k] && temp_PlusN[q] ){
                                if(gn==Count2bitCycleArrows ){
                                        CyclicPlusNIndex=k;
//printf("IsCyclic returned %d as 2 BIT cyle index\n",CyclicPlusNIndex);
                                        return true;
                                }
                                Count2bitCycleArrows+=1;
                        }
                }
        }

	//only higher order cycles exist

        if(CountCycleArrows>0){
                //generate a random number based on CountCycleArrows
                int gn=(int)(  (float)rand()/(float)RAND_MAX*CountCycleArrows+1);

                if(gn==0){gn=1;}if(gn>CountCycleArrows){gn=CountCycleArrows;}

                CountCycleArrows=1;
                for ( int k=0;k<N*N;k++){
                        if(temp_PlusN[k] ){
                                if(gn==CountCycleArrows){
                                        CyclicPlusNIndex=k;
//printf("IsCyclic found %d PlusN true bits : Set CyclicPlusNIndex to %d RETURNIN TRUE\n",CountCycleArrows,CyclicPlusNIndex);
                                        return true;
                                }
                                CountCycleArrows+=1;
                        }
                }
//printf("IsCyclic found %d PlusN true bits : Set CyclicPlusNIndex to %d RETURNING FALSE\n",CountCycleArrows,CyclicPlusNIndex);
        }
        return false;
}
bool PSO_particle::Has2bitCycle()
{
	Make_PlusN_Particle();
	bit[0]=0;bit[1]=0;

	bool temp_PlusN[9999];
        for (int k=0;k<N*N;k++){
                temp_PlusN[k]=X_PlusN[k];
        }
	//prune all arrows except those participating in cycles
        int NumberOfSets=1;
        while(NumberOfSets>0){
                NumberOfSets=0;
                for (int i=0;i<N;i++){
                        bool AllFalseBlock=true; //a node with no parents
                        for (int j=0;j<N;j++){
                                if (temp_PlusN[i*N+j]){
                                        AllFalseBlock=false;
                                        break;
                                }
                        }
                        if (AllFalseBlock){
                                for (int m=0;m<N;m++){
                                        if (temp_PlusN[m*N+i]){
                                                NumberOfSets+=1;
                                                temp_PlusN[m*N+i]=false;
                                        }
                                }
                        }
                }
        }
	 //count arrows participating in cycles
        //and find , if there exists, a 2 bit cycle index
        int CountCycleArrows=0;
        int Count2bitCycleArrows;
        for ( int k=0;k<N*N;k++)
        {
                if(temp_PlusN[k]){
                        CountCycleArrows+=1;
                        //does this arrow have a 2 bit partner?
                        int q=N*(k-((int)(k/N)*N))+(int)(k/N);//q is the 2 bit arrow index in the plusN partic
                        if(temp_PlusN[q]){
                                Count2bitCycleArrows+=1;//add another 2 bit arrow
				bit[0]=k;bit[1]=q;
				return true;
                        }
                }
        }
	return false;

	

}
