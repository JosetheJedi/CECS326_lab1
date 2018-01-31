#include <iostream>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
using namespace std;

static int unID = 0;

//system memory
struct memory
{
  int size;     //size of Memory, number of blocks
  string *data; //array of data in memory, string values in blocks
} mem;

//Memory Block Table of system memory
struct MBT
{
  int size;
  int availableBlocks; //size of the MBT, same as memory size
  bool *states;        //state of that memory block, true/false. true = not free
} mbt;

//Process Control Block, process info
struct pcb
{
  int process_ID; //ID of proces
  pcb *nextPT;

  //Page table for a process, address translation
  int address;  //location of memory block process starts
  int pageSize; //number of blocks taken by process
  int *blocks;  //pointer to next PT
};

//Ready Queue for the processes
struct ready_queue
{
  int size;  //size of queue
  pcb *head; //pointer to head
  pcb *tail; //pointer to tail
} rq;

//return unput string for program name
// string Program_Name(){
//   cout << "\nProgram Name: ";
//   string s = "";
//   getline(cin, s);                                                              //get user input, string
//   return s;
// }

//Initialized system memory, 32 blocks reserved for OS
void Initialize_Mem(int mem_size)
{
  mem.size = mem_size; //Instantiate the number of blocks in system memory
  mbt.size = mem.size;
  mbt.availableBlocks = mem.size - 32; //Instantiate the number of blocks in MBT
  mem.data = new string[mem.size];     //Instantiate the data array for sytem memory
  mbt.states = new bool[mbt.size];     //Instantiate the states array for MBT
  for (int i = 0; i < 32; i++)
  {
    mem.data[i] = "Operating System";
    mbt.states[i] = true;
  }
  cout << "\nMemory Initialized. Memory reserved for OS.\n";
}

void InitProc()
{
  int size = rand() % 241 + 10; // chooses from 10 - 250
  pcb *p = new pcb();
  p->pageSize = size;
  p->process_ID = unID;
  p->blocks = new int[p->pageSize];

  unID++;

  //debugging
  // cout << "mbt size: " << mbt.availableBlocks << " - " << p->pageSize << endl;

  if (mbt.availableBlocks >= size)
  {
    mbt.availableBlocks = mbt.availableBlocks - size;

    int placed = 0;
    int currBlock = 32;

    while (placed != p->pageSize)
    {
      if (mbt.states[currBlock] == false)
      {
        if (placed == 0)
        {
          p->address = currBlock;
        }
        p->blocks[placed] = currBlock;
        mbt.states[currBlock] = true;
        placed++;
      }
      currBlock++;
    }

    if (rq.head == NULL)
    {
      rq.head = p; // if null then head points to the only pcb
    }
    else
    {
      rq.tail->nextPT = p; // if not null then tail.pcb points to new pcb
    }

    rq.tail = p; // update tail to point to the new pcb
  }

  // debugging
  // cout << "mbt size now: " << mbt.availableBlocks << endl;
}

void printProc()
{
  pcb *p = rq.head;

  while (p != NULL)
  {
    cout << "ID: " << p->process_ID << " | Address: " << p->address << " | Size: " << p->pageSize << endl;

    // debugging
    for(int i = 0; i < p->pageSize; i++){
      cout << "Block " << i<< " = " << p->blocks[i] << endl;
    }
    p = p->nextPT;
    
  }
}

void TerminateP(int ID){

  pcb *p = rq.head;
  pcb *last = NULL;
  bool found = false;

  while((p != NULL)){

    int compare = p->process_ID;
    cout << "comparing: " << ID << " with " << compare << endl;

    if(ID == compare){
      found = true;
      break;
    }
    else{
      last = p;
      p = p->nextPT;
    }
  }

  if(found != true){
    cout << "Given output ID not found" << endl;
  }
  else{
    cout << "Terminating process with ID: " << p->process_ID << endl;

    int loops = p->pageSize;
    mbt.availableBlocks = mbt.availableBlocks + loops;

    for(int i = 0; i < loops; i++){
      int index = p->blocks[i];
      mbt.states[index] = false;
      p->blocks[i] = 0;
    }
    p->blocks = NULL;
    p->pageSize = 0;
    p->address = 0;

    // getting rid of the pcb from the readyqueue
    if(last != NULL){
      last->nextPT = p->nextPT;

      if(rq.tail == p){
        rq.tail = last;
      }
    }

    if(rq.head == p){
      rq.head = p->nextPT;
    }

  }


}

int main()
{
  Initialize_Mem(1024);
  // cout << "Starting " << mbt.availableBlocks << endl;
  srand(time(NULL));
  cout << "\nEnter to exit the program: " << endl;
  InitProc();
  InitProc();
  InitProc();
  InitProc();
  printProc();
  TerminateP(2);
  printProc();
  InitProc();
  InitProc();
  printProc();
  cin.ignore().get();
  delete[] mem.data;
  delete[] mbt.states;
  return 0;
}
