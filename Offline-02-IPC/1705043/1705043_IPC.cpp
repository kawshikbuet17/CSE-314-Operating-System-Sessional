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


sem_t kioskEmpty;
sem_t kioskFull;
pthread_mutex_t mtxKioskSecurity;
pthread_mutex_t mtxEntryKiosk;

sem_t* securityBeltEmpty;
sem_t* securityBeltFull;
pthread_mutex_t mtxSecurityBoarding;
queue<int> passenger;
queue<int>* securityQueue;
queue<int> boardingQueue;
sem_t boardingEmpty;
sem_t boardingFull;
pthread_mutex_t boardingMtx;
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
		pthread_mutex_unlock(&mtxEntryKiosk);
		sem_post(&kioskFull);
	}
}

void * KioskFunc(void * arg){
	while(true){
		sem_wait(&kioskFull);
		// sleep(1);
		pthread_mutex_lock(&mtxEntryKiosk);
		if(!passenger.empty()){
			int item = passenger.front();
			passenger.pop();
			cout<<"Passenger "<<item<<" in Kiosk at time "<<GetTime()<<endl;
			int r = rand()%M;
			sem_wait(&securityBeltEmpty[r]);
			// sleep(1);
			pthread_mutex_lock(&mtxKioskSecurity);
			securityQueue[r].push(item);
			sleep(w);
			cout<<"Passenger "<<item<<" is sent to security belt "<<r<<" at time "<<GetTime()<<endl;
			pthread_mutex_unlock(&mtxKioskSecurity);
			sem_post(&securityBeltFull[r]);
		}
		pthread_mutex_unlock(&mtxEntryKiosk);
		sem_post(&kioskEmpty);
	}
}

void * SecurityFunc(void * arg){
	string name = ((struct args*)arg)->name;
	int num = ((struct args*)arg)->num;
	while(true){
		sem_wait(&securityBeltFull[num]);
		// sleep(1);
		pthread_mutex_lock(&mtxKioskSecurity);
		if(!securityQueue[num].empty()){
			int item = securityQueue[num].front();
			securityQueue[num].pop();
			
			cout<<"Security Belt "<<num<<" received passenger "<<item<<" at time "<<GetTime()<<endl;

			// sem_wait(&boardingEmpty);
			// sleep(1);
			pthread_mutex_lock(&mtxSecurityBoarding);
			boardingQueue.push(item);
			
			sleep(x);
			cout<<"Passenger "<<item<<" is sent to boarding at time "<<GetTime()<<endl;
			pthread_mutex_unlock(&mtxSecurityBoarding);
			// sem_post(&boardingFull);
		}
		pthread_mutex_unlock(&mtxKioskSecurity);
		sem_post(&securityBeltEmpty[num]);
	}
}

void * BoardingFunc(void * arg){
	while(true){
		// sem_wait(&boardingFull);
		// sleep(1);
		pthread_mutex_lock(&mtxSecurityBoarding);
		if(!boardingQueue.empty()){
			int item = boardingQueue.front();
			boardingQueue.pop();
			cout<<"Passenger "<<item<<" received at boarding at time "<<GetTime()<<endl;
			sleep(y);
			cout<<"Passenger "<<item<<" ready to go at time "<<GetTime()<<endl;
		}
		pthread_mutex_unlock(&mtxSecurityBoarding);
		// sem_post(&boardingEmpty);
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
	pthread_mutex_init(&mtxEntryKiosk,NULL);
	pthread_mutex_init(&mtxKioskSecurity,NULL);
	pthread_mutex_init(&mtxSecurityBoarding,NULL);
	pthread_create(&thread3, NULL, EnterPassenger,(void*)message);
	pthread_create(&thread1, NULL, KioskFunc,(void*)message);
	initializeSecurityElements();
	pthread_create(&thread2, NULL, BoardingFunc,(void*)message);
	while(1);
	pthread_exit(NULL);
	return 0;
}

