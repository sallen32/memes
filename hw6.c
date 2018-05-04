#include "hw6.h"
#include <stdio.h>
#include <pthread.h>

pthread_mutex_t lock;
pthread_barrier_t barrier;

typedef struct elevator{
	int current_floor;
	int direction;
	int occupancy;
	enum {ELEVATOR_ARRIVED=1, ELEVATOR_OPEN=2, ELEVATOR_CLOSED=3} state;	
}elevator; 

elevator elev[ELEVATORS];

//elev *elp = &elev;
//elev **array;

void scheduler_init() {	
	
//	array = malloc(ELEVATORS * sizeof(elev*));
	
	for(int i=0;i<ELEVATORS;i++)
	{	
//		(*array) = malloc(sizeof(elev));
		elev[i].current_floor=0;		
		elev[i].direction=-1;
		elev[i].occupancy=0;
		elev[i].state=ELEVATOR_ARRIVED;
		
	//	pthread_mutex_t i;
	//	pthread_mutex_init(&i,0);
	}
		pthread_barrier_init(&barrier,NULL,ELEVATORS+1);
		pthread_mutex_init(&lock,0);
}


void passenger_request(int passenger, int from_floor, int to_floor, 
											 void (*enter)(int, int), 
											 void(*exit)(int, int))
{
	int i = 0;
	/*for(; i < ELEVATORS; i++)
	{
		if(elev[i].occupancy == 0)
		{
		//	pthread_mutex_lock(&i);
			break;
		}
	}
	*/

	// wait for the elevator to arrive at our origin floor, then get in
	int waiting = 1;
	while(waiting) {
//		pthread_barrier_wait(&barrier);
		pthread_mutex_lock(&lock);
		
		for(i = 0; i<ELEVATORS; i++){
		if(elev[i].current_floor == from_floor && elev[i].state == ELEVATOR_OPEN && elev[i].occupancy==0) {
			enter(passenger, i);
			elev[i].occupancy++;
			waiting=0;
			break;
		}
		}
		
		pthread_mutex_unlock(&lock);
		pthread_barrier_wait(&barrier);
	}

	// wait for the elevator at our destination floor, then get out
	int riding=1;
	while(riding) {
//		pthread_barrier_wait(&barrier);
		pthread_mutex_lock(&lock);

		if(elev[i].current_floor == to_floor && elev[i].state == ELEVATOR_OPEN) {
			exit(passenger, i);
			elev[i].occupancy--;
			riding=0;
		}
	

		pthread_mutex_unlock(&lock);
		pthread_barrier_wait(&barrier);
	}
}

void elevator_ready(int elevator, int at_floor, 
										void(*move_direction)(int, int), 
										void(*door_open)(int), void(*door_close)(int)) {

//	pthread_barrier_wait(&barrier);
//	if(elevator!=0) return;
	
	pthread_barrier_wait(&barrier);
	
//	pthread_mutex_lock(&lock);

	if(elev[elevator].state == ELEVATOR_ARRIVED) {
		door_open(elevator);
		elev[elevator].state=ELEVATOR_OPEN;
	}
	else if(elev[elevator].state == ELEVATOR_OPEN) {
		door_close(elevator);
		elev[elevator].state=ELEVATOR_CLOSED;
	}
	else {
		if(at_floor==0 || at_floor==FLOORS-1) 
			elev[elevator].direction*=-1;
		move_direction(elevator,elev[elevator].direction);
		elev[elevator].current_floor=at_floor+elev[elevator].direction;
		elev[elevator].state=ELEVATOR_ARRIVED;
	}

//	pthread_mutex_unlock(&lock);
}
