
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>

#define N 5
enum possible_states {THINKING, HUNGRY, EATING};


//Semaphore to indicate a lock on chopsticks, eating state
sem_t shared;
//Critical section--chopsticks
sem_t S[N];

//Holds state for each philosopher on the table
int state[N];


void readyToEat(int phNum);
void pickChopsticks(int phNum);
void releaseChopsticks(int phNum);
void* philosopherThread(void* num);


int main()
{

	int phNum;
	int phArray[N] = { 0, 1, 2, 3, 4 };
	pthread_t thread_id[N];

	// initialize the semaphores
	sem_init(&shared, 0, 1);
	for (phNum = 0; phNum < N; phNum++) {
		sem_init(&S[phNum], 0, 0);
	}


	for (phNum = 0; phNum < N; phNum++) {
		// create phNum processes
		pthread_create(&thread_id[phNum], NULL, philosopherThread, &phArray[phNum]);
		printf("Philosopher %d is thinking\n", phNum + 1);
	}

	for (phNum = 0; phNum < N; phNum++){
		pthread_join(thread_id[phNum], NULL);
	}
}

//Check if ready to eat
void readyToEat(int phNum)
{
	int leftNeighbor = (phNum + N - 1) % N;
	int rightNeighbor = (phNum + 1) % N;

	if (state[phNum] == HUNGRY && state[leftNeighbor] != EATING && state[rightNeighbor] != EATING) {

		// If no neighbors are eating, post your state as eating
		state[phNum] = EATING;

		//sleep(2);

		printf("Philosopher %d grabs chopsticks %d and %d\n", phNum + 1, leftNeighbor + 1, phNum + 1);
		printf("Philosopher %d is Eating\n", phNum + 1);

		//Mark philosopher as ready to eat, proceed to pick up chopsticks
		sem_post(&S[phNum]);
	}
}

// take up chopsticks
void pickChopsticks(int phNum)
{

	// Change philosopher's state to hungry

	state[phNum] = HUNGRY;
	printf("Philosopher %d is Hungry\n", phNum + 1);

	//Wait for shared semaphore to go up if any neighbors are eating
	sem_wait(&shared);

	//Eat if neighbours are not eating
	readyToEat(phNum);

	//Take charge of semaphore to lock eating status
	sem_post(&shared);

	//If chopsticks not available, wait
	sem_wait(&S[phNum]);

	sleep(1);
}

// put down chopsticks
void releaseChopsticks(int phNum)
{
	int leftNeighbor = (phNum + N - 1) % N;
	int rightNeighbor = (phNum + 1) % N;

	//Release lock on shared semaphore, allowing neighbors to eat if they want
	sem_wait(&shared);

	//Move to thinking state
	state[phNum] = THINKING;

	printf("Philosopher %d released chopsticks %d and %d\n", phNum + 1, leftNeighbor + 1, phNum + 1);
	printf("Philosopher %d is thinking\n", phNum + 1);

	//Let neighbour philosophers eat if they are hungry--bounded wait
	readyToEat(leftNeighbor);
	readyToEat(rightNeighbor);

	sem_post(&shared);
}

void* philosopherThread(void* num)
{

	int* phNum = num;
	while (1) {

		//Sleep for a random amount of time, Thinking
		sleep(rand() % 5);

		//Hungry
		pickChopsticks(*phNum);

		//sleep(rand() % 5);

		//Release chopsticks and go back to thinking
		releaseChopsticks(*phNum);
	}
}

