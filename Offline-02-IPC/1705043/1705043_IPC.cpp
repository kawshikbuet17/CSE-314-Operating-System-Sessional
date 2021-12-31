#include<cstdio>
#include<pthread.h>
#include<semaphore.h>
#include<queue>
#include <unistd.h>
#include<bits/stdc++.h>
using namespace std;

#define FileIO \
    freopen("input.txt", "r", stdin); \
    freopen("output.txt", "w", stdout);

int M,N,P,w,x,y,z;
clock_t time_req;
struct args {
    char* name;
    int num;
};

//queue
queue<int> passenger;
queue<int>* securityQueue;
queue<int> boardingQueue;
queue<int> vipPassenger;
queue<int> returnPassenger;
queue<int> specialPassenger;

//semaphore
sem_t kioskEmpty;
sem_t kioskFull;
sem_t boardingEmpty;
sem_t boardingFull;
sem_t specialKioskEmpty;
sem_t specialKioskFull;
sem_t* securityBeltEmpty;
sem_t* securityBeltFull;
sem_t boardingVipFull;
sem_t boardingVipEmpty;

//mutex
pthread_mutex_t boardingMtx;
pthread_mutex_t mtxVip;
pthread_mutex_t mtxSpecialKiosk;
pthread_mutex_t mtxKioskSecurity;
pthread_mutex_t mtxEntryKiosk;
pthread_mutex_t mtxSecurityBoarding;
map<int, int> mp;



int GetTime(){
	return (int)(clock()-time_req)/CLOCKS_PER_SEC;
}

void * EnterPassenger(void * arg){
	default_random_engine generator;
	poisson_distribution<int> distribution(3);

	for(int i=1; i<=20; i++){
		sem_wait(&kioskEmpty);
		int number = distribution(generator);
		sleep(number%10);
		pthread_mutex_lock(&mtxEntryKiosk);
		passenger.push(i);
		mp[i]=(i%4==0);
		cout<<"Passenger "<<i<<" has arrived at airport at time "<<GetTime()<<endl;
		pthread_mutex_unlock(&mtxEntryKiosk);

		sem_post(&kioskFull);
	}
}

void * KioskProduce(void * arg){
	int item = ((struct args*)arg)->num;
	int r = rand()%M;
	sem_wait(&securityBeltEmpty[r]);
	sleep(w);
	pthread_mutex_lock(&mtxKioskSecurity);
	securityQueue[r].push(item);
	cout<<"Passenger "<<item<<" has finished check in at time "<<GetTime()<<endl;
	pthread_mutex_unlock(&mtxKioskSecurity);
	sem_post(&securityBeltFull[r]);
}

void * SpecialKiosk(void * arg){
	while(true){
		sem_wait(&specialKioskFull);
		pthread_mutex_lock(&mtxSpecialKiosk);
		if(!specialPassenger.empty()){
			int item = specialPassenger.front();
			specialPassenger.pop();
			sleep(w);
			cout<<"Passenger "<<item<<" has finished check [Special] in at time "<<GetTime()<<endl;
			vipPassenger.push(item);
		}
		pthread_mutex_unlock(&mtxSpecialKiosk);
		sem_post(&specialKioskEmpty);
	}
}


void * KioskFunc(void * arg){
	while(true){
		sem_wait(&kioskFull);
		pthread_mutex_lock(&mtxEntryKiosk);
		if(!passenger.empty()){
			int item = passenger.front();
			passenger.pop();
			int val;
			sem_getvalue(&kioskFull, &val);
			cout<<"Passenger "<<item<<" has started self check at kiosk "<<val<<" at time "<<GetTime()<<endl;
			struct args* a = (struct args *)malloc(sizeof(struct args));
			a->name="KioskProduce";
			a->num=item;
			if(mp[item]==1){
				cout<<"Passenger "<<item<<" has sent VIP Tunnel at time "<<GetTime()<<endl;
				vipPassenger.push(item);
			}
			else{
				pthread_t thread;
				pthread_create(&thread, NULL, KioskProduce,(void*)a);
			}
		}
		pthread_mutex_unlock(&mtxEntryKiosk);
		sem_post(&kioskEmpty);
	}
}

pthread_mutex_t mtx_transfer;


void * Transfer_VIP_Passenger_From_Right(void * arg){
	while(true){
		sleep(z);
		// pthread_mutex_lock(&mtx_transfer);
		sem_wait(&specialKioskEmpty);
		pthread_mutex_lock(&mtxSpecialKiosk);
		while(!returnPassenger.empty()){
			int item = returnPassenger.front();
			returnPassenger.pop();
			cout<<"Passenger "<<item<<" sent to Special Kiosk at time "<<GetTime()<<endl;
			specialPassenger.push(item);
		}
		pthread_mutex_unlock(&mtxSpecialKiosk);
		sem_post(&specialKioskFull);
		// pthread_mutex_unlock(&mtx_transfer);
	}
}

void * Transfer_VIP_Passenger_From_Left(void * arg){

	while(true){
		sleep(z);
		// pthread_mutex_lock(&mtx_transfer);
		pthread_mutex_lock(&mtxSecurityBoarding);
		while(!vipPassenger.empty()){
			int item = vipPassenger.front();
			vipPassenger.pop();
			cout<<"Passenger "<<item<<" crossed VIP Tunnel at time "<<GetTime()<<endl;
			boardingQueue.push(item);
		}
		pthread_mutex_unlock(&mtxSecurityBoarding);
		sem_post(&boardingFull);
		// pthread_mutex_unlock(&mtx_transfer);
	}
}


void * SecurityProduce(void * arg){
	int item = ((struct args*)arg)->num;
	sleep(x);
	pthread_mutex_lock(&mtxSecurityBoarding);
	boardingQueue.push(item);
	
	cout<<"Passenger "<<item<<" has crossed the security check at time "<<GetTime()<<endl;
	pthread_mutex_unlock(&mtxSecurityBoarding);
	sem_post(&boardingFull);
}

void * SecurityFunc(void * arg){
	string name = ((struct args*)arg)->name;
	int num = ((struct args*)arg)->num;
	while(true){
		sem_wait(&securityBeltFull[num]);
		pthread_mutex_lock(&mtxKioskSecurity);
		if(!securityQueue[num].empty()){
			int item = securityQueue[num].front();
			securityQueue[num].pop();
			cout<<"Passenger "<<item<<" has started waiting for security check in belt "<<num<<endl;
			struct args* a = (struct args *)malloc(sizeof(struct args));
			a->name="SecurityProduce";
			a->num=item;
			pthread_t thread;
			pthread_create(&thread, NULL, SecurityProduce,(void*)a);
		}
		pthread_mutex_unlock(&mtxKioskSecurity);
		sem_post(&securityBeltEmpty[num]);
	}
}

void * BoardingFunc(void * arg){
	while(true){
		sem_wait(&boardingFull);
		pthread_mutex_lock(&mtxSecurityBoarding);
		if(!boardingQueue.empty()){
			int item = boardingQueue.front();
			boardingQueue.pop();
			if(GetTime()%5==0){
				cout<<"Passenger "<<item<<" [Lost] at time "<<GetTime()<<endl;
				returnPassenger.push(item);
			}else{
				cout<<"Passenger "<<item<<" has started waiting to be boarded at time "<<GetTime()<<endl;
				sleep(y);
				cout<<"Passenger "<<item<<"  has boarded the plane at time "<<GetTime()<<endl;
			}
		}
		pthread_mutex_unlock(&mtxSecurityBoarding);
	}
}

void initializeSecurityElements(){
	securityBeltEmpty = new sem_t[M];
	securityBeltFull = new sem_t[M];
	securityQueue = new queue<int>[M];

	for(int i=0; i<M; i++){
		struct args* a = (struct args *)malloc(sizeof(struct args));
		pthread_t thread;
		a->name = "Security";
		a->num = i;
		sem_init(&securityBeltEmpty[i], 0, P);
		sem_init(&securityBeltFull[i], 0, 0);
		pthread_create(&thread, NULL, SecurityFunc, (void*)a);
	}
}

int main(void)
{
	time_req = clock();
	srand(time(0));
	FileIO;
	cin>>M>>N>>P>>w>>x>>y>>z;
	cout<<M<<" "<<N<<" "<<P<<" "<<"\n"<<w<<" "<<x<<" "<<y<<" "<<z<<endl;
	pthread_t thread1;
	pthread_t thread2;
	pthread_t thread3;

	char* message = "Nothing";
	
	sem_init(&kioskEmpty, 0, M);
	sem_init(&kioskFull, 0, 0);
	sem_init(&boardingEmpty, 0, 10);
	sem_init(&boardingFull, 0, 0);
	sem_init(&specialKioskEmpty,0,M);
	sem_init(&specialKioskFull,0,0);
	sem_init(&boardingVipFull,0,0);

	pthread_mutex_init(&mtx_transfer, NULL);
	pthread_mutex_init(&mtxEntryKiosk,NULL);
	pthread_mutex_init(&mtxKioskSecurity,NULL);
	pthread_mutex_init(&mtxSecurityBoarding,NULL);
	pthread_mutex_init(&mtxVip,NULL);
	pthread_mutex_init(&mtxSpecialKiosk,NULL);

	pthread_create(&thread3, NULL, EnterPassenger,(void*)message);
	pthread_create(&thread1, NULL, KioskFunc,(void*)message);
	initializeSecurityElements();
	pthread_create(&thread2, NULL, BoardingFunc,(void*)message);
	pthread_t thread4;
	pthread_create(&thread4, NULL, Transfer_VIP_Passenger_From_Left,(void*)message);
	pthread_t thread5;
	pthread_create(&thread5, NULL, Transfer_VIP_Passenger_From_Right, (void*)message);

	pthread_t thread6;
	pthread_create(&thread6, NULL, SpecialKiosk, (void*)message);
	while(1);
	pthread_exit(NULL);
	return 0;
}

