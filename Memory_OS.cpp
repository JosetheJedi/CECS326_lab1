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
  pcb *nextPT;    //pointer to next PT

  //Page table for a process, address translation
  int address;  //location of memory block process starts
  int pageSize; //number of blocks taken by process
  int *blocks;
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


void printMBT()
{
  for(int i = 0; i < mbt.size; i++){
    cout << "MBT[" << i << "] = " << mbt.states[i] << endl;
  }
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

    // output page table and MBT
    cout << "\nID: " << p->process_ID << " | Size: " << p->pageSize << endl;
    for (int i = 0; i < p->pageSize; i++)
    {
      cout << "Block " << i << " = " << p->blocks[i] << endl;
    }

    // outputting the MBT
    printMBT();
  }
  else
  {
    cout << "\n\t**Insufficient Memory:" << mbt.availableBlocks << endl;
  }

  // debugging
  // cout << "mbt size now: " << mbt.availableBlocks << endl;
}

void printProc()
{
  // gets the first pcb on the readyqueue.
  pcb *p = rq.head;

  // if it is empty, notify the user that there is no process running.
  if (p == NULL)
  {
    cout << "\n\t**No processes running!**\n";
  }
  else
  {
    /*
      if the pointer is not null then we print out the ID of the process, the starting address that it takes
      and the size that the process takes up.
    */
    while (p != NULL)
    {
      cout << "ID: " << p->process_ID << " | Address: " << p->address << " | Size: " << p->pageSize << endl;

      // debugging
      // for (int i = 0; i < p->pageSize; i++)
      // {
      //   cout << "Block " << i << " = " << p->blocks[i] << endl;
      // }

      // we get the next pcb in the readyqueue.
      p = p->nextPT;
    }
  }
}

void TerminateProcess(int ID)
{

  pcb *p = rq.head;   // points to the current pcb
  pcb *last = NULL;   // points to the last pcb
  bool found = false; // will change to true if process we are trying
                      // to terminate is found.

  // while there are still pcb's in the linked list then we will continue
  // the search for the pcb with the matching ID.
  while ((p != NULL))
  {

    int compare = p->process_ID; // getting the ID to compare from the
                                 // current PCB

    // for debugging
    // cout << "comparing: " << ID << " with " << compare << endl;

    // comparing the pcb given by the user with the current pcb_id
    // if found we will set the found value to true then break the while loop.
    if (ID == compare)
    {
      found = true;
      break;
    }
    else
    {
      /*
        if the current pcb_id does not match the id given by the user
        then we set the last pcb pointer to the current one, then
        we change the current pcb to the next pcb on the readyqueue.
      */
      last = p;
      p = p->nextPT;
    }
  }

  // if the ID provided was not found in the readyqueue then we notify the user.
  if (found != true)
  {
    cout << "\n\t**Given output ID not found!**" << endl;
  }
  else
  {
    /*
      when we find the PCB that has the given ID we notify the user that the process will
      be terminated.
    */
    cout << "\n\t**Terminating process with ID: " << p->process_ID << endl;

    // we need to get the size of how many blocks we will free up in the MBT
    int loops = p->pageSize;

    // change the available block to account for the blocks freed after terminating the
    // process..
    mbt.availableBlocks = mbt.availableBlocks + loops;

    // we will search through the PCB page table to determine which
    // MBT blocks to set to true.
    for (int i = 0; i < loops; i++)
    {
      int index = p->blocks[i];  // get the address that an index in the
                                 // page table takes up.
      mbt.states[index] = false; // sets that index in the MBT to false, meaning free.
      p->blocks[i] = 0;          // setting the value in the page table to 0.
    }

    delete (p->blocks); // getting rid of the blocks array.
    p->pageSize = 0;    // resetting the value of the pagetable.
    p->address = 0;     // nullifying the value of the address.

    // getting rid of the pcb from the readyqueue
    if (last != NULL)
    {
      /* if the last pointer is not null then this process is not at the head
        of the ready queue so we need to point the last pointer to the next pointer
      
      */
      last->nextPT = p->nextPT;

      /*
        if the pointer we are removing is at the tail, then we change the
        tail to the previous pointer.
      */
      if (rq.tail == p)
      {
        rq.tail = last;
      }
    }

    /*
      if the pointer that we are removing is at the head of the readyqueue
      then we must change the head to the next pointer.
    */
    if (rq.head == p)
    {
      rq.head = p->nextPT;
    }

    delete (p);
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
  cin.ignore().get();
  delete[] mem.data;
  delete[] mbt.states;
  return 0;
}
