/* File: prog.c
 * Duarte Cernadas
 * qag985
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "linkedlist.h"
#include "prog.h"


int main(int argc, char *argv[]) {
	FILE *fp;
	int numParams; // == (argc+1) / 2
	Params_st *params = (Params_st*)malloc(sizeof(Params_st));
	PerfData_st *perfData = (PerfData_st*)malloc(sizeof(PerfData_st));
	Linkedlist_st *pcbList;

	int id, prio, burst;

	int cpuReg[8] = {0};
	/*
		int CLOCK = 0;
		int Total_waiting_time = 0;
		int Total_turnaround_time = 0;
		int Total_job = 0;
	 */

	/* argc:
	 * Each "token" counts as one arg. i.e., '-alg FIFO' == 2*arg.
	 * Since there are two "tokens" to one param (except arg0):
	 * 	argc = (params * 2) - 1
	 *		params = (argc + 1) / 2
	 * 
	 * Minimum number of params: 3 => argc = 5
	 * Maximum number of params: 4 => argc = 7
	 * 
	 * Min & Max params are also only valid options.
	 */

	// check if argc is even
	// (valid argc will always be odd)
	if(argc % 2 == 0) {
		printf("\n\tUsage: ./prog -alg [FIFO|SJF|PR|RR] [-quantum integer(ms)] -input [input_file_name.txt]\n\n");
		free(params);
		free(perfData);
		return 0;
	}
	// check number of params
	numParams = (argc + 1) / 2;
	if(numParams != 3 && numParams != 4) {
		printf("\n\tUsage: ./prog -alg [FIFO|SJF|PR|RR] [-quantum integer(ms)] -input [input_file_name.txt]\n\n");
		free(params);
		free(perfData);
		return 0;
	}

	if( ParseCmdlineParams(argc, argv, params) == 1 ) {
		free(params);
		free(perfData);
		return 1;
	}

	// open file
	fp = fopen(params->inputFile, "r");
	if( !fp ) {
		fprintf(stderr, "\n\tERROR: could not open file %s\n\n", params->inputFile);
		free(params);
		free(perfData);
		return 1;
	}

	// initialize list
	pcbList = NewLinkedList();
	if(pcbList == NULL) {
		free(params);
		free(perfData);
		FreeLinkedList(pcbList);
		return 1;
	}

	// read file
	while( fscanf(fp, "%d %d %d", &id, &prio, &burst) != EOF) {
		Enlist(pcbList, NewPCBnode(id, prio, burst));
	}
	// close file
	fclose(fp);

	/* debugging
		int k = 0;
		PCB_st *curpcb = pcbList->head;

		while(curpcb != NULL) {
		printf("pcb %d: id %d, prio %d, burst %d\n", i, curpcb->procId, curpcb->procPrio, curpcb->cpuBurst);
		curpcb = curpcb->next;
		i++;
		}
	 */

	// print name, scheduling algo, inputfile name
	printf("\nStudent name:\t\tDuarte Cernadas\n");
	printf("Input file:\t\t%s\n", params->inputFile);
	printf("CPU Scheduling Alg:\t");
	switch(params->alg) {
		case FIFO:
			printf("FIFO");
			break;
		case SJF:
			printf("SJF");
			break;
		case PR: 
			printf("PR");
			break;
		case RR:
			printf("RR");
			break;
	}
	printf("\n\n");

	// run algorithm
	switch(params->alg) {
		case FIFO:
			if( FIFO_Scheduling(pcbList, cpuReg, perfData) == 1 )
				printf("\n\tERROR: running the FIFO scheduling algorithm failed\n");
			break;
		case SJF:
			if( SJF_Scheduling(pcbList, cpuReg, perfData) == 1)
				printf("\n\tERROR: running the SJF scheduling algorithm failed\n");
			break;
		case PR: 
			if( PR_Scheduling(pcbList, cpuReg, perfData) == 1)
				printf("\n\tERROR: running the PR scheduling algorithm failed\n");
			break;
		case RR:
			if( RR_Scheduling(pcbList, cpuReg, perfData, params->qTime) == 1 )
				printf("\n\tERROR: running the RR scheduling algorithm failed\n");
			break;
	}

	free(params);
	free(perfData);
	FreeLinkedList(pcbList);
	return 0;
}


//------ CPU SCHEDULING FUNC ------//
int FIFO_Scheduling(Linkedlist_st *pcbList, int cpuReg[], PerfData_st *perfData) {
	int i = 0;
	PCB_st *pcb;

	while(!LinkedListIsEmpty(pcbList)) {
		pcb = Delist(pcbList);
		if(pcb == NULL) return 1;

		for(i = 0; i < 8; i++) {
			cpuReg[i] = pcb->myReg[i];
		}

		// work
		for(i = 0; i < 8; i++) {
			cpuReg[i] += 1;
		}

		for(i = 0; i < 8; i++) {
			pcb->myReg[i] = cpuReg[i];
		}

		// performance metrics
		pcb->waitingTime = pcb->waitingTime + perfData->CLOCK - pcb->queueEnterClock;
		perfData->Total_waiting_time += pcb->waitingTime;
		perfData->CLOCK += pcb->cpuBurst;
		perfData->Total_turnaround_time += perfData->CLOCK;
		perfData->Total_job++;

		printf("Process %d finished at %d ms\n", pcb->procId, perfData->CLOCK);

		free(pcb);
	}

	printf("\n");

	PrintPerfMetrics(perfData);

	return 0;
}

int SJF_Scheduling(Linkedlist_st *pcbList, int cpuReg[], PerfData_st *perfData) {
	int i = 0;
	PCB_st *pcb;

	while(!LinkedListIsEmpty(pcbList)) {
		pcb = PopMinCPUburst(pcbList);
		if(pcb == NULL) return 1;

		for(i = 0; i < 8; i++) {
			cpuReg[i] = pcb->myReg[i];
		}

		// work
		for(i = 0; i < 8; i++) {
			cpuReg[i] += 1;
		}

		for(i = 0; i < 8; i++) {
			pcb->myReg[i] = cpuReg[i];
		}

		// performance metrics
		pcb->waitingTime = pcb->waitingTime + perfData->CLOCK - pcb->queueEnterClock;
		perfData->Total_waiting_time += pcb->waitingTime;
		perfData->CLOCK += pcb->cpuBurst;
		perfData->Total_turnaround_time += perfData->CLOCK;
		perfData->Total_job++;

		printf("Process %d finished at %d ms\n", pcb->procId, perfData->CLOCK);

		free(pcb);
	}

	printf("\n");

	PrintPerfMetrics(perfData);

	return 0;
}

int PR_Scheduling(Linkedlist_st *pcbList, int cpuReg[], PerfData_st *perfData) {
	int i = 0;
	PCB_st *pcb;

	while(!LinkedListIsEmpty(pcbList)) {
		pcb = PopHighestPrio(pcbList);
		if(pcb == NULL) return 1;

		for(i = 0; i < 8; i++) {
			cpuReg[i] = pcb->myReg[i];
		}

		// work
		for(i = 0; i < 8; i++) {
			cpuReg[i] += 1;
		}

		for(i = 0; i < 8; i++) {
			pcb->myReg[i] = cpuReg[i];
		}

		// performance metrics
		pcb->waitingTime = pcb->waitingTime + perfData->CLOCK - pcb->queueEnterClock;
		perfData->Total_waiting_time += pcb->waitingTime;
		perfData->CLOCK += pcb->cpuBurst;
		perfData->Total_turnaround_time += perfData->CLOCK;
		perfData->Total_job++;

		printf("Process %d finished at %d ms\n", pcb->procId, perfData->CLOCK);

		free(pcb);
	}

	printf("\n");

	PrintPerfMetrics(perfData);

	return 0;
}

int RR_Scheduling(Linkedlist_st *pcbList, int cpuReg[], PerfData_st *perfData, int qTime) {
	int i = 0;
	PCB_st *pcb;

	while(!LinkedListIsEmpty(pcbList)) {
		pcb = Delist(pcbList);
		if(pcb == NULL) return 1;

		for(i = 0; i < 8; i++) {
			cpuReg[i] = pcb->myReg[i];
		}

		// work
		for(i = 0; i < 8; i++) {
			cpuReg[i] += 1;
		}

		for(i = 0; i < 8; i++) {
			pcb->myReg[i] = cpuReg[i];
		}

		// performance metrics
		if(pcb->cpuBurst <= qTime) {
			pcb->waitingTime = pcb->waitingTime + perfData->CLOCK - pcb->queueEnterClock;
			perfData->Total_waiting_time += pcb->waitingTime;
			perfData->CLOCK += pcb->cpuBurst;
			perfData->Total_turnaround_time += perfData->CLOCK;
			perfData->Total_job++;

			printf("Process %d finished at %d ms\n", pcb->procId, perfData->CLOCK);

			free(pcb);
		}
		else if(pcb->cpuBurst > qTime) {
			pcb->waitingTime = pcb->waitingTime + perfData->CLOCK - pcb->queueEnterClock;
			perfData->Total_waiting_time += pcb->waitingTime;
			perfData->CLOCK += qTime;
			pcb->cpuBurst -= qTime;
			pcb->queueEnterClock = perfData->CLOCK;

			Enlist(pcbList, pcb);
		}
	}

	printf("\n");

	PrintPerfMetrics(perfData);

	return 0;
}

//---------- HELPER FUNC ----------// 
/*
 * Parses commandline parameters into Params_st
 *
 * Returns 0 if successful, 1 otherwise. 
 */
int ParseCmdlineParams(int argc, char *argv[], Params_st *st) {
	int i;
	char *param; // ./prog -param parg -param arg ...
	char *parg;

	// iterate through argv[]
	for(i = 1; i < argc; i+=2) {

		param = argv[i];
		parg = argv[i+1];

		// check for parameter
		if( strncmp(param, "-alg", 5) == 0 ) {
			st->paramAlg = 1;

			// check for algorithm
			if( strncmp(parg, "FIFO", 5) == 0 ) {
				st->alg = FIFO;
			}
			else if(strncmp(parg, "SJF", 4) == 0 ) {
				st->alg = SJF;
			}
			else if( strncmp(parg, "PR", 3) == 0 ) {
				st->alg = PR;
			}
			else if( strncmp(parg, "RR", 3) == 0 ) {
				st->alg = RR;
			}
			else {
				PrintUsageError();
				return 1;
			}
		}
		else if( strncmp(param, "-input", 7) == 0 ) {

			// error check argument for -input
			// (if parg[0] == '-') => not a valid linux filepath or no file path given
			if( parg[0] == '-' ) {
				printf("\n\tInvalid/No value given for parameter -input\n");
				PrintUsageError();
				return 1;
			}

			st->paramInput = 1;
			st->inputFile = parg;
		}
		else if( strncmp(param, "-quantum", 9) == 0 ) {
			st->paramQuantum = 1;

			// read and error check argument for -quantum
			if( sscanf(parg, "%d", &st->qTime) != 1 ) {
				printf("\n\t Quantum time (-quantum) must be > 0\n");
				printf("\n\tArgument for parameter -quantum must be a positive integer\n");
				PrintUsageError();

				return 1;
			} 
			else if(st->qTime <= 0) {
				printf("\n\tQuantum time (-quantum) must be > 0\n");
				PrintUsageError();

				return 1;
			}
		}
	}

	// check that RR alg has quantum time
	if( st->alg == RR && st->paramQuantum != 1 ) {
		printf("\n\tRR algorithm requires -quantum\n");
		PrintUsageError();
		return 1;
	}

	// check for mandatory parameters (-alg, -input)
	if( st->paramAlg == 0 || st->paramInput == 0 ) {
		printf("\n\t-alg and -input are required parameters\n");
		PrintUsageError();
		return 1;
	}

	return 0;
}
void PrintPerfMetrics(PerfData_st *perfData) {

	printf("Average wait time: %0.2f ms\t\t(%d/%d)\n", (double) perfData->Total_waiting_time / (double) perfData->Total_job, perfData->Total_waiting_time, perfData->Total_job);

	printf("Average turnaround time: %0.2f ms\t(%d/%d)\n", (double) perfData->Total_turnaround_time / (double) perfData->Total_job, perfData->Total_turnaround_time, perfData->Total_job),

		printf("Throughput: %0.2f jobs per ms\t\t(%d/%d)\n\n", (double) perfData->Total_job / (double) perfData->CLOCK, perfData->Total_job, perfData->CLOCK);

}

void PrintUsageError() {
	printf("\n\tUsage: ./prog -alg [FIFO|SJF|PR|RR] [-quantum integer(ms)] -input [input_file_name.txt]\n\n");
}
