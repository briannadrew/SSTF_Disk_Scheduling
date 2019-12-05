/*********************************************************************/
/* Name: Disk Scheduling Simulation SSTF (Lab 4)                     */
/* Author: Brianna Drew                                              */
/* Description                                                       */
/*    This program simulates disk scheduling for a disk with 800     */
/*    tracks with an SSTF scheduling algorithm.                      */
/*********************************************************************/
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <values.h>
#include <time.h>
#include <stdbool.h>

// linked list to represent the queue
struct Queue_Node{
  int trackNumber; // track number of the track request in the queue
  struct Queue_Node *next; // pointer to the next node in queue
  struct Queue_Node *prev; // pointer to the previous node in queue
};

struct Queue_Struct{
  struct Queue_Node *q_head; // pointer to first node in queue
  struct Queue_Node *q_tail; // pointer to last node in queue
};

// create the queue
struct Queue_Struct sstf;

// function declarations
int trackReqs(void);
int numTrack(void);
void AddToList(struct Queue_Struct *req_queue, int trackNum);
int RemoveFromList(struct Queue_Struct *req_queue);
void ProcessReqs(struct Queue_Struct *req_queue);

// global variable declarations/initializations
unsigned seed;
int fileReqs;
bool first = true;
float diff;
float totalHeadMove = 0;
int currTrack = 0;
int totalReqs = 0;

void main(){
  sstf.q_head = NULL; // initialize head of queue to null
  sstf.q_tail = NULL; // initialize tail of queue to null
  printf("*********************************************************************\n");
  printf("*********************************************************************\n");
  printf("**                                                                 **\n");
  printf("**                DISK SCHEDULING SYSTEM SIMULATION                **\n");
  printf("**                          SSTF EDITION                           **\n");
  printf("**                         By Brianna Drew                         **\n");
  printf("**                                                                 **\n");
  printf("*********************************************************************\n");
  printf("*********************************************************************\n\n");
  printf("Seed for the random number generator: ");
  scanf("%d", &seed); // get user input as integer for seed for random number generation
  srand(seed);
  printf("Number of file requests: ");
  scanf("%d", &fileReqs); // get user input as integer for number of file requests

  // local variable declarations/initializations
  int numFileReqs = 0;
  float average;

  do { // do this...
    int numTrackReqs = trackReqs(); // call function to get a random number between 1 and 5 to represent the number of track requests for the current file request
    int i;
    for (i = 0; i < numTrackReqs; i++) { // for each track request for the current file request...
      int trackNum = numTrack(); // call function to get a random number between 0 and 799 to represent the number of the track requested
      AddToList(&sstf, trackNum); // call function to add the track request to the queue
      first = false;
    }
    int nextTrack = RemoveFromList(&sstf); // call function to remove the next (first) track request from the queue (signifying that the disk head will be moved to that track) and have that track returned
    diff = abs((float)nextTrack - (float)currTrack); // calculate the head movement for the current track request
    totalHeadMove += diff; // add the head movement for the current track request to the total head movement
    totalReqs++; // increase number of total track requests by 1
    currTrack = nextTrack; // make the current track now the next track
    numFileReqs++; // increase number of file requests by 1
  } while(numFileReqs <= fileReqs); // ...for each file request
  ProcessReqs(&sstf); // call function to remove the remaining track requests from the queue
  average = totalHeadMove / (float) totalReqs; // calculate the average total head movement for each file request and print the result
  printf("Average head movement: %5.2f\n", average);
}

int trackReqs(void){
  int rand_num = (rand() % (5 - 1 + 1)) + 1; // generate random number from 1 to 5 representing number of track requests for the current file request
  return rand_num;
}

int numTrack(void){
  int rand_num = rand() % 800; // generate random number from 0 to 799 representing
  return rand_num;
}

void AddToList(struct Queue_Struct *req_queue, int trackNum){

  struct Queue_Node *newnode; // create new node to be inserted into the queue linked list
  newnode = (struct Queue_Node *) malloc(sizeof(struct Queue_Node)); // create the required space for the node in the queue linked list
  newnode->next = NULL; // initialize the next pointer of the newly created node to null (as it is being added to the end of the queue)
  newnode->prev = req_queue->q_tail; // initialize the prev pointer of the newly created node to the node at the tail of the queue (as it is being inserted after the current last node)
  newnode->trackNumber = trackNum; // insert given track number data into the new node

  // if the queue is initially empty...
  if(req_queue->q_tail == NULL){
    req_queue->q_head = newnode; // set queue head pointer to the new node
    req_queue->q_tail = newnode; // set queue tail pointer to the new node
    return;
  }
  // if not, then...
  req_queue->q_tail->next = newnode; // link new node to the end of the queue
  req_queue->q_tail = newnode; // set queue tail pointer to the new node
  return;
}

int RemoveFromList(struct Queue_Struct *req_queue){

  struct Queue_Node *loc; // create a new temporary node to hold the first request from the queue to be removed
  struct Queue_Node *q_curr; // create a new temporary node to represent the current node when traversing the queue
  int first_req; // integer representing the track number of the first request in the queue
  int req_index_count = 0; // integer representing the position in the queue of the current track request
  int req_index = 0; // integer representing the position in the queue of the track with least difference from the current track
  float track_diff = abs((float)currTrack - (float)req_queue->q_head->trackNumber); // float representing the difference between the track number of the current track request and the current lowest difference track number
  float curr_diff; // float representing the difference between the current track in the trasversal of the queue and the current lowest difference track number

  // if the queue is empty...
  if(req_queue->q_head == NULL){
    printf("***Error - Queue is Empty***\n"); // print error message
    return(NULL); // return null as the track number for first request in the queue
  }

  // if there is only one request in the queue...
  if(req_queue->q_head == req_queue->q_tail){
    loc = req_queue->q_head; // store the first request in the queue in the temporary node
    first_req = loc->trackNumber; // get the track number of the first request in the queue
    req_queue->q_tail = NULL; // set queue tail pointer to null (empty queue)
    req_queue->q_head = NULL; // set queue head pointer to null (empty queue)
    return first_req; // return the track number of the first request in the queue
  }

  q_curr = req_queue->q_head->next; // initialize the current track request of the traversal to the second request in the queue (because req_index and track_diff are initialized to that of the head of the queue)
  // if there are 2 or more requests in the queue, traverse the queue and...
  while(q_curr != NULL){
    req_index_count++; // increase index of queue by 1
    curr_diff = abs((float)currTrack - (float)q_curr->trackNumber); // calculate the difference between the current current track and the current track of the traversal's track numbers
    // if the current track of the traversal is closer to the current track than the previous closest...
    if(curr_diff < track_diff){
      req_index = req_index_count; // store the current track of the traversal's index in the queue
      track_diff = curr_diff; // store the difference between the current track of the traversal and the current track as the new shortest seek time
    }
    q_curr = q_curr->next; // the current track request is now the next track request in the queue
  }

  // if the track request needed to be moved to the head of the queue is at the tail of the queue...
  if(req_index == req_index_count){
    q_curr = req_queue->q_tail;
    req_queue->q_tail = q_curr->prev; // remove this track request from current position in queue by setting the previous node as the queue's tail
    q_curr->prev->next = NULL; // remove this track request from current position in queue by linking the previous node to NULL
    req_queue->q_head->prev = q_curr; // link the head of the queue backward to the current track request
    q_curr->next = req_queue->q_head; // link the current track request forward to the head of the queue
    req_queue->q_head = q_curr; // set the current tracj request as the new head of the queue
  }

  // if the track request needed to be moved to the head of the queue is not already at the head of the queue and it is also not at the tail of the queue...
  if(req_index != 0 && req_index != req_index_count){
    req_index_count = 0; // reset queue index counter
    q_curr = req_queue->q_head->next; // reset current temporary node to the head of the queue
    //travserse the queue and...
    while(q_curr != NULL){
      req_index_count++; // increase index of queue by 1
      // if we have arrived at the track request we need to move to the front of the queue...
      if(req_index == req_index_count){
        q_curr->prev->next = q_curr->next; // remove this track request from current position in queue by linking the two nodes on either side forward
        q_curr->next->prev = q_curr->prev; // remove this track request from current position in queue by linking the two nodes on either side backward
        req_queue->q_head->prev = q_curr; // link the head of the queue backward to the current track request
        q_curr->next = req_queue->q_head; // link the current track request forward to the head of the queue
        req_queue->q_head = q_curr; // set the current track request as the new head of the queue
        break;
      }
      q_curr = q_curr->next; // the current track request is now the next track request in the queue
    }
  }

  loc = req_queue->q_head; // store the first request in the queue in the temporary node
  first_req = loc->trackNumber; // get the track number of the first request in the queue
  req_queue->q_head = loc->next; // set the next request in the queue now as the first request
  loc->next->prev = NULL; // set the previous node of the next track request to NULL as it will be the head of the queue
  free(loc); // free the temporary node holding head of queue
  return first_req; // return the track number of the first request in the queue
}

void ProcessReqs(struct Queue_Struct *req_queue){

  while(req_queue->q_head != NULL){ // while the queue is not empty...
    int nextTrack = RemoveFromList(&sstf); // call function to remove the next (first) track request from the queue (signifying that the disk head will be moved to that track) and have that track returned
    diff = abs((float)nextTrack - (float)currTrack); // calculate the head movement for the current track request
    totalHeadMove += diff; // add the head movement for the current track request to the total head movement
    totalReqs++; // increase number of total track requests by 1
    currTrack = nextTrack; // make the current track now the next track
  }
  return;
}
