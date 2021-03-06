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

#define TO_BE_BOARDED 20

int M,N,P,w,x,y,z;
int boarderCnt=0;


struct args {
    char* name;
    int num;
	int belt;
};

struct timespec start, finish;
double elapsed;

//queue
queue<int> passenger;
queue<int>* securityQueue;
queue<int> boardingQueue;
queue<int> specialPassenger;

queue<int> kioskNumber;

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
pthread_mutex_t mtxSpecialKiosk;
pthread_mutex_t mtxKioskSecurity;
pthread_mutex_t mtxEntryKiosk;
pthread_mutex_t mtxSecurityBoarding;

map<int, int> mp;


pthread_mutex_t mtx_print;


int GetTime(){
	clock_gettime(CLOCK_MONOTONIC, &finish);
	elapsed = (finish.tv_sec - start.tv_sec);
	elapsed += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;
	return (int)elapsed;
}

void * EnterPassenger(void * arg){
	int item = ((struct args*)arg)->num;
	sem_wait(&kioskEmpty);	
	pthread_mutex_lock(&mtxEntryKiosk);
	passenger.push(item);
	pthread_mutex_unlock(&mtxEntryKiosk);
	sem_post(&kioskFull);
}


void * GeneratePassenger(void * arg){
	default_random_engine generator;
	poisson_distribution<int> distribution(3);
	for(int i=1; i<=TO_BE_BOARDED; i++){
		int number = distribution(generator);
		sleep(number%10);
		// sleep(1);
		struct args* a = (struct args *)malloc(sizeof(struct args));
		a->num=i;
		mp[i]=(i%4==0);
		pthread_mutex_lock(&mtx_print);
		if(mp[i]==1){
			cout<<"Passenger "<<i<<" (VIP) has arrived at airport at time "<<GetTime()<<endl;
		}
		else{
			cout<<"Passenger "<<i<<" has arrived at airport at time "<<GetTime()<<endl;
		}
		
		pthread_mutex_unlock(&mtx_print);
		pthread_t thread;
		pthread_create(&thread, NULL, EnterPassenger, (void*)a);
	}
}


void * KioskProduce(void * arg){
	int item = ((struct args*)arg)->num;
	int r = rand()%N;

	sleep(w);
	
	pthread_mutex_lock(&mtx_print);
	cout<<"Passenger "<<item<<" has finished check in at time "<<GetTime()<<endl;
	pthread_mutex_unlock(&mtx_print);
	sem_post(&kioskEmpty);

	pthread_mutex_lock(&mtx_print);
	cout<<"Passenger "<<item<<" has started waiting for security check in belt "<<r+1<<" from time "<<GetTime()<<endl;
	pthread_mutex_unlock(&mtx_print);

	sem_wait(&securityBeltEmpty[r]);
	
	pthread_mutex_lock(&mtxKioskSecurity);
	
	securityQueue[r].push(item);
	
	pthread_mutex_unlock(&mtxKioskSecurity);
	sem_post(&securityBeltFull[r]);
}
pthread_mutex_t mtx_extradb;
pthread_mutex_t mtx_db;
pthread_mutex_t mtx_rc;
int rc = 0;

void * SpecialSendToVipChannelThread(void * arg){
	int item = ((struct args*)arg)->num;

	pthread_mutex_lock(&mtx_print);
	cout<<"Passenger "<<item<<" has started crossing VIP Channel at time "<<GetTime()<<endl;
	pthread_mutex_unlock(&mtx_print);

	sleep(z);
	pthread_mutex_lock(&mtxSecurityBoarding);
	boardingQueue.push(item);
	pthread_mutex_lock(&mtx_print);
	cout<<"Passenger "<<item<<" has crossed the VIP Channel at time "<<GetTime()<<endl;
	pthread_mutex_unlock(&mtx_print);
	pthread_mutex_unlock(&mtxSecurityBoarding);
	sem_post(&boardingFull);
	pthread_mutex_lock(&mtx_rc);
	rc = rc - 1;
	if(rc==0){
		pthread_mutex_unlock(&mtx_extradb);
		pthread_mutex_unlock(&mtx_db);
	}
		
	pthread_mutex_unlock(&mtx_rc);
}

void * SpecialSendToVipChannel(void * arg){
	int item = ((struct args*)arg)->num;

	pthread_mutex_lock(&mtx_rc);
	rc = rc + 1;
	if(rc==1){
		pthread_mutex_lock(&mtx_db);
		pthread_mutex_lock(&mtx_extradb);
	}
		
	pthread_mutex_unlock(&mtx_rc);

	pthread_t thread;
	pthread_create(&thread, NULL, SpecialSendToVipChannelThread,(void*)arg);
}

void * SpecialKiosk(void * arg){
	while(true){
        if(boarderCnt==TO_BE_BOARDED)break;

		sem_wait(&specialKioskFull);
		pthread_mutex_lock(&mtxSpecialKiosk);
		int item = specialPassenger.front();
		specialPassenger.pop();
		pthread_mutex_lock(&mtx_print);
		cout<<"Passenger "<<item<<" has started self-check at kiosk (Special) at time "<<GetTime()<<endl;
		pthread_mutex_unlock(&mtx_print);
		pthread_mutex_unlock(&mtxSpecialKiosk);

		sleep(w);

		struct args* a = (struct args *)malloc(sizeof(struct args));
		a->num=item;

		pthread_mutex_lock(&mtx_print);
		cout<<"Passenger "<<item<<" has finished check (Special) in at time "<<GetTime()<<endl;
		pthread_mutex_unlock(&mtx_print);

		pthread_t thread;
		pthread_create(&thread, NULL, SpecialSendToVipChannel,(void*)a);
	
	}
}

void * SendToVipChannelThread(void * arg){
	int item = ((struct args*)arg)->num;
	
	pthread_mutex_lock(&mtx_print);
	cout<<"Passenger "<<item<<" has started crossing VIP Channel at time "<<GetTime()<<endl;
	pthread_mutex_unlock(&mtx_print);
	
	sleep(z);
	

	pthread_mutex_lock(&mtx_rc);
	rc = rc - 1;
	if(rc==0){
		pthread_mutex_unlock(&mtx_extradb);
		pthread_mutex_unlock(&mtx_db);
		// cout<<"MTX_DB UNLOCK "<<GetTime()<<endl;
	}
	pthread_mutex_unlock(&mtx_rc);
	pthread_mutex_lock(&mtxSecurityBoarding);
	boardingQueue.push(item);
	pthread_mutex_lock(&mtx_print);
	cout<<"Passenger "<<item<<" has crossed the VIP Channel at time "<<GetTime()<<endl;
	cout<<"Passenger "<<item<<" has started waiting to be boarded at time "<<GetTime()<<endl;
	pthread_mutex_unlock(&mtx_print);
	pthread_mutex_unlock(&mtxSecurityBoarding);
	sem_post(&boardingFull);

}

void * SendToVipChannel(void * arg){
	int item = ((struct args*)arg)->num;
	sleep(w);
	sem_post(&kioskEmpty);
	pthread_mutex_lock(&mtx_print);
	cout<<"Passenger "<<item<<" has finished check in at time "<<GetTime()<<endl;
	pthread_mutex_unlock(&mtx_print);

	pthread_mutex_lock(&mtx_rc);
	rc = rc + 1;
	if(rc==1){
		pthread_mutex_lock(&mtx_db);
		pthread_mutex_lock(&mtx_extradb);
		// cout<<"MTX_DB LOCK "<<GetTime()<<endl;
	}
		
	pthread_mutex_unlock(&mtx_rc);

	pthread_t thread;
	pthread_create(&thread, NULL, SendToVipChannelThread, (void*)arg);
	
}

void * KioskFunc(void * arg){
	while(true){
        if(boarderCnt==TO_BE_BOARDED)break;
		sem_wait(&kioskFull);
		pthread_mutex_lock(&mtxEntryKiosk);
		int item = passenger.front();
		passenger.pop();
		pthread_mutex_unlock(&mtxEntryKiosk);
		int val = kioskNumber.front();
		kioskNumber.pop();
		// sem_getvalue(&kioskEmpty, &val);
		pthread_mutex_lock(&mtx_print);
		cout<<"Passenger "<<item<<" has started self-check at kiosk "<<val<<" at time "<<GetTime()<<endl;
		pthread_mutex_unlock(&mtx_print);
		struct args* a = (struct args *)malloc(sizeof(struct args));
		a->name="KioskProduce";
		a->num=item;
		
		// if(1==1){
		if(mp[item]==1){
			pthread_t thread;
			pthread_create(&thread, NULL, SendToVipChannel, (void*)a);
		}
		else{
			pthread_t thread;
			pthread_create(&thread, NULL, KioskProduce,(void*)a);
		}
		kioskNumber.push(val);
	}
}

int wc = 0;
pthread_mutex_t mtx_wc;

void * SendRightToLeftThread(void * arg){
	int item = ((struct args*)arg)->num;

	pthread_mutex_lock(&mtx_print);
	cout<<"Passenger "<<item<<" has started crossing VIP Channel at time "<<GetTime()<<endl;
	pthread_mutex_unlock(&mtx_print);

	sleep(z);
	pthread_mutex_lock(&mtxSpecialKiosk);
	pthread_mutex_lock(&mtx_print);
	cout<<"Passenger "<<item<<" has crossed VIP Channel at time "<<GetTime()<<endl;
	cout<<"Passenger "<<item<<" sent to Special Kiosk at time "<<GetTime()<<endl;
	pthread_mutex_unlock(&mtx_print);
	specialPassenger.push(item);
	pthread_mutex_unlock(&mtxSpecialKiosk);
	sem_post(&specialKioskFull);
	// cout<<"MTX UNLOCK R "<<GetTime()<<endl;
	pthread_mutex_lock(&mtx_wc);
	wc = wc - 1;
	if(wc==0){
		pthread_mutex_unlock(&mtx_extradb);
	}
	pthread_mutex_unlock(&mtx_wc);
}


void * SendRightToLeft(void * arg){
	// sem_wait(&specialKioskEmpty);
	
	pthread_mutex_lock(&mtx_db);
	pthread_mutex_lock(&mtx_wc);
	wc = wc + 1;
	if(wc==1){
		pthread_mutex_lock(&mtx_extradb);
		
	}
	pthread_mutex_unlock(&mtx_wc);
	pthread_mutex_unlock(&mtx_db);
	
	// cout<<"MTX LOCK R "<<GetTime()<<endl;
	pthread_t thread;
	pthread_create(&thread, NULL, SendRightToLeftThread, (void*)arg);
	
}


void * SendLeftToRight(void * arg){
	int item = ((struct args*)arg)->num;
	sleep(z);
	pthread_mutex_lock(&mtxSecurityBoarding);
	cout<<"Passenger "<<item<<" crossed VIP Tunnel at time "<<GetTime()<<endl;
	boardingQueue.push(item);
	pthread_mutex_unlock(&mtxSecurityBoarding);
	sem_post(&boardingFull);
}


void * SecurityProduce(void * arg){
	int item = ((struct args*)arg)->num;
	int belt = ((struct args*)arg)->belt;
	sleep(x);
	sem_post(&securityBeltEmpty[belt]);
	pthread_mutex_lock(&mtxSecurityBoarding);
	boardingQueue.push(item);
	pthread_mutex_lock(&mtx_print);
	cout<<"Passenger "<<item<<" has crossed the security check at time "<<GetTime()<<endl;
	cout<<"Passenger "<<item<<" has started waiting to be boarded at time "<<GetTime()<<endl;
	pthread_mutex_unlock(&mtx_print);
	pthread_mutex_unlock(&mtxSecurityBoarding);
	sem_post(&boardingFull);
}

void * SecurityFunc(void * arg){
	string name = ((struct args*)arg)->name;
	int num = ((struct args*)arg)->num;
	while(true){
        if(boarderCnt==TO_BE_BOARDED)break;
		sem_wait(&securityBeltFull[num]);
		pthread_mutex_lock(&mtxKioskSecurity);
		int item = securityQueue[num].front();
		securityQueue[num].pop();
		pthread_mutex_lock(&mtx_print);
		cout<<"Passenger "<<item<<" has started the security check at time "<<GetTime()<<endl;
		pthread_mutex_unlock(&mtx_print);
		struct args* a = (struct args *)malloc(sizeof(struct args));
		a->name="SecurityProduce";
		a->num=item;
		a->belt=num;
		pthread_t thread;
		pthread_create(&thread, NULL, SecurityProduce,(void*)a);
		pthread_mutex_unlock(&mtxKioskSecurity);
	}
}


void * BoardingFunc(void * arg){
	while(true){
        if(boarderCnt==TO_BE_BOARDED)break;
		sem_wait(&boardingFull);
		pthread_mutex_lock(&mtxSecurityBoarding);
		int item = boardingQueue.front();
		boardingQueue.pop();
		pthread_mutex_unlock(&mtxSecurityBoarding);
		struct args* a = (struct args *)malloc(sizeof(struct args));
		a->num=item;
		
		// if(1==1){
		if(GetTime()%5==0){
			pthread_mutex_lock(&mtx_print);
			cout<<"Passenger "<<item<<" [Lost] at time "<<GetTime()<<endl;
			pthread_mutex_unlock(&mtx_print);
			pthread_t thread;
			pthread_create(&thread, NULL, SendRightToLeft,(void*)a);

		}else{
			pthread_mutex_lock(&mtx_print);
			cout<<"Passenger "<<item<<" has started boarding the plane at time "<<GetTime()<<endl;
			pthread_mutex_unlock(&mtx_print);
			sleep(y);
			pthread_mutex_lock(&mtx_print);
			cout<<"Passenger "<<item<<"  has boarded the plane at time "<<GetTime()<<endl;
			pthread_mutex_unlock(&mtx_print);
            boarderCnt++;
			if(boarderCnt==TO_BE_BOARDED){
				cout<<"ALL PASSENGERS BOARDED"<<endl;
			}
		}
		
	}
}

void initializeSecurityElements(){
	securityBeltEmpty = new sem_t[N];
	securityBeltFull = new sem_t[N];
	securityQueue = new queue<int>[N];

	for(int i=0; i<N; i++){
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
	clock_gettime(CLOCK_MONOTONIC, &start);
	srand(time(0));
	FileIO;
	cin>>M>>N>>P>>w>>x>>y>>z;
	cout<<M<<" "<<N<<" "<<P<<" "<<endl<<w<<" "<<x<<" "<<y<<" "<<z<<endl;
	
	for(int i=1; i<=M; i++){
		kioskNumber.push(i);
	}

	pthread_t thread1;
	pthread_t thread2;
	pthread_t thread3;
	pthread_t thread4;

	char* message = "Nothing";
	
	sem_init(&kioskEmpty, 0, M);
	sem_init(&kioskFull, 0, 0);
	sem_init(&boardingEmpty, 0, 10);
	sem_init(&boardingFull, 0, 0);
	sem_init(&specialKioskEmpty,0,M);
	sem_init(&specialKioskFull,0,0);
	sem_init(&boardingVipFull,0,0);


	pthread_mutex_init(&mtx_print, NULL);
	pthread_mutex_init(&mtx_rc, NULL);
	pthread_mutex_init(&mtx_wc, NULL);
	pthread_mutex_init(&mtx_db, NULL);
	pthread_mutex_init(&mtx_extradb, NULL);
	pthread_mutex_init(&mtxEntryKiosk,NULL);
	pthread_mutex_init(&mtxKioskSecurity,NULL);
	pthread_mutex_init(&mtxSecurityBoarding,NULL);
	pthread_mutex_init(&mtxSpecialKiosk,NULL);

	pthread_create(&thread1, NULL, GeneratePassenger,(void*)message);
	pthread_create(&thread2, NULL, KioskFunc,(void*)message);
	initializeSecurityElements();
	pthread_create(&thread3, NULL, BoardingFunc,(void*)message);
	pthread_create(&thread4, NULL, SpecialKiosk, (void*)message);

	pthread_exit(NULL);
	return 0;
}

