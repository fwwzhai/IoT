// Ex05.cpp/.ino
// loop slicing
//
// This exercise demonstrates how to divide ("slice") the main loop
// into different logical time segments. Instead of using delay(),
// we count loop iterations and trigger tasks at specific points.
// This approach is common in embedded and IoT systems where multiple
// tasks must run without blocking the CPU.
// loop slicing

#include "Thing.h"

// ------------------------------------------------------------------
// Timing variables
// ------------------------------------------------------------------

// Stores the time (in milliseconds) when the program started.
// Used to measure how long a full cycle of loop iterations takes.
// what time did we start?
unsigned long firstSliceMillis;

// Stores the time when the previous slice event occurred.
// Used to measure how long each slice lasts.
// what time did we last run this action?
unsigned long lastSliceMillis;

// ------------------------------------------------------------------
// Loop slicing control
// ------------------------------------------------------------------

// Counter that increases every time loop05() runs.
// This is used to schedule tasks at specific iterations.
// a control iterator for slicing up the main loop ///////////////////////////
int loopIteration = 0;

// Maximum number of loop iterations before resetting the counter.
// Prevents the counter from growing indefinitely.
const int LOOP_ROLLOVER = 25000000; 

// ------------------------------------------------------------------
// setup05()
// Runs once when the device starts.
// ------------------------------------------------------------------
void setup05() {

   // Run all previous setup stages
  setup04();      
  
   // Debug message to indicate this setup stage has started
  dln(startupDBG, "\nsetup05..."); 

  // Record the start time of the program
  firstSliceMillis = millis();        

  // Initialize the previous slice time
  lastSliceMillis = firstSliceMillis; 
}

// ------------------------------------------------------------------
// loop05()
// Runs repeatedly. Tasks are triggered based on loopIteration.
// ------------------------------------------------------------------
void loop05() { 
  // Size of a "slice" of iterations.
  // Every sliceSize loops we report timing information.
  int sliceSize = 1000000;

  // ---------------------------------------------------------------
  // Task triggers on periodic iterations
  // ---------------------------------------------------------------

  // Every 1000 loop iterations do task X
  if(loopIteration % 1000 == 0) { 
    dbg(loopDBG, "task X every 1000 loops, loopIteration = ");
    dln(loopDBG, loopIteration);
  } 
  // Every 50000 loop iterations do task Y
  if(loopIteration % 50000 == 0) { 
    dbg(loopDBG, "task Y every 50000 loops, loopIteration = ");
    dln(loopDBG, loopIteration);
  }

  // ---------------------------------------------------------------
  // Slice reporting
  // Every sliceSize iterations we print timing statistics.
  // ---------------------------------------------------------------
  if(loopIteration % sliceSize == 0) { 
    dbg(loopDBG, "loopIteration number = ");
    dbg(loopDBG, loopIteration);
    dbg(loopDBG, ", slice lasted ");
    dbg(loopDBG, millis() - lastSliceMillis);
    dbg(loopDBG, " milliseconds");
    dbg(loopDBG, "; slice size is ");
    dln(loopDBG, sliceSize);

    // Update slice timestamp
    lastSliceMillis = millis();
  }

  // ---------------------------------------------------------------
  // Loop rollover
  // Reset the counter after reaching LOOP_ROLLOVER.
  // ---------------------------------------------------------------
  if(loopIteration++ == LOOP_ROLLOVER) {
    loopIteration = 0;

    dbg(loopDBG, "loopIteration rolling over; ");
    dbg(loopDBG, LOOP_ROLLOVER);
    dbg(loopDBG, " loops lasted ");
    dbg(loopDBG, millis() - firstSliceMillis);
    dln(loopDBG, " milliseconds...; rolling over");
  }
}
