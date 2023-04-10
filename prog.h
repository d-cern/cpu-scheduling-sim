#ifndef PROG_H
#define PROG_H

/*
 * Enum to represent each cpu scheduling algorithm
 */
typedef enum Alg_enum { FIFO, SJF, PR, RR } Alg_enum;

/*
 * Holds all data regarding the parameters passed
 * through the command line.
 *
 * paramX = whether this parameter was invoked;
 * 									 1=true, 0=false
 */
typedef struct Params_st {
	unsigned paramAlg;
	Alg_enum alg;

	unsigned paramQuantum;
	int qTime;

	unsigned paramInput;
	char *inputFile;
	
} Params_st;

typedef struct PerfData_st {
	int CLOCK;
	int Total_waiting_time;
	int Total_turnaround_time;
	int Total_job;

} PerfData_st;


int FIFO_Scheduling(Linkedlist_st *pcbList, int cpuReg[], PerfData_st *perfData);

int SJF_Scheduling(Linkedlist_st *pcbList, int cpuReg[], PerfData_st *perfData);

int PR_Scheduling(Linkedlist_st *pcbList, int cpuReg[], PerfData_st *perfData);

int RR_Scheduling(Linkedlist_st *pcbList, int cpuReg[], PerfData_st *perfData, int qTime);

/*
 * Parses commandline parameters into Params_st
 */
int ParseCmdlineParams(int argc, char *argv[], Params_st *st);

void PrintPerfMetrics(PerfData_st *perfData); 

void PrintUsageError();

#endif
