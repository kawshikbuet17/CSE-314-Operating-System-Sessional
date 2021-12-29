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
sem_t empty;
sem_t full;
sem_t kioskEmpty;
sem_t kioskFull;
pthread_mutex_t mtx;
pthread_mutex_t mtxKiosk;

sem_t* securityBelt;
pthread_mutex_t* securityMtx;
queue<int>* securityQueue;

queue<int> q;
queue<int> q1;


void * ProducerFunc(void * arg)
{	
	// printf("%s\n",(char*)arg);
	cout<<(char*)arg<<endl;
	int i;
	for(i=1;i<=10;i++)
	{
		sem_wait(&empty);
		sleep(1);
		
		pthread_mutex_lock(&mtx);
		q.push(i);
		// printf("producer produced item %d\n",i);
		cout<<"producer produced item "<<i<<endl;
		pthread_mutex_unlock(&mtx);
		sem_post(&full);
	}
}

void * ConsumerFunc(void * arg)
{
	// printf("%s\n",(char*)arg);
	cout<<(char*)arg<<endl;
	int i;
	for(i=1;i<=10;i++)
	{	
 		sem_wait(&full);
		sleep(1);
		
		pthread_mutex_lock(&mtx);
		int item = q.front();
		
		q.pop();
		// printf("consumer consumed item %d\n",item);	
		cout<<"consumer consumed item "<<item<<endl;
		pthread_mutex_unlock(&mtx);
		sem_post(&empty);	
	}
}

void * KioskFunc(void * arg){
	cout<<(char*)arg<<endl;
	for(int i=1; i<=10; i++){
		sem_wait(&kioskEmpty);
		sleep(1);
		pthread_mutex_lock(&mtxKiosk);
		q1.push(i);
		cout<<"Entered Kiosk Passenger "<<i<<endl;
		pthread_mutex_unlock(&mtxKiosk);
		sem_post(&kioskFull);
	}
}

void * SecurityFunc(void * arg){
	cout<<(char*)arg<<endl;
}

int main(void)
{
	FileIO;
	cin>>M>>N>>P>>w>>x>>y>>z;
	cout<<M<<N<<P<<"\n"<<w<<x<<y<<z<<endl;

	pthread_t thread1;
	pthread_t thread2;
	pthread_t thread3;
	
	//init_semaphore();
	sem_init(&empty,0,5);
	sem_init(&full, 0,0);
	sem_init(&kioskEmpty, 0, M);
	sem_init(&kioskFull, 0, 0);
	pthread_mutex_init(&mtx,NULL);
	pthread_mutex_init(&mtxKiosk,NULL);

	char * message1 = "i am producer";
	char * message2 = "i am consumer";	
	char * message3 = "i am kiosk";
	char * message4 = "i am security belt";
	
	pthread_create(&thread1,NULL,ProducerFunc,(void*)message1 );
	pthread_create(&thread2,NULL,ConsumerFunc,(void*)message2 );
	pthread_create(&thread3, NULL, KioskFunc, (void*)message3);
	securityBelt = new sem_t[M];
	securityMtx = new pthread_mutex_t[M];
	securityQueue = new queue<int>[M];
	for(int i=0; i<M; i++){
		cout<<"Creating SecurityFunc "<<i<<endl;
		pthread_create(&thread3, NULL, KioskFunc, (void*)message4);
	}
	while(1);
	pthread_exit(NULL);
	return 0;
}

