#include<iostream>
#include<fstream>
#include<algorithm>
#include<queue>
#include<vector>
#include<string>


// Ahigad Genish
// Id: 316228022
// This project calculate the average turn around of set of processes by many scheduling algorithm such:
// FCFS,LCFS,LCFS preemptive,Round Robin, SRTF
// Process class :
using namespace std;
class Process
{
private:
	int _arriveTime;
	int _processTime;
	int _turnAround;
	int _remainingTime;
	int _index;
public:
	// getters
	int getArriveTime() const {
		return this->_arriveTime;
	}
	int getProcessTime() const {
		return this->_processTime;
	}
	int getTurnAround() const {
		return this->_turnAround;
	}
	int getRemainingTime() const {
		return this->_remainingTime;
	}
	int getIndex()const {
		return this->_index;
	}
	// setters
	void setArriveTime(int arriveTime) {
		this->_arriveTime = arriveTime;
	}
	void setProcessTime(int processTime) {
		this->_processTime = processTime;
	}
	void setTurnAround(int turnAround) {
		this->_turnAround = turnAround;
	}
	void setRemainingTime(int remainingTime) {
		this->_remainingTime = remainingTime;
	}
	void setInd(int ind) {
		this->_index = ind;
	}
	// constructor and copy constructor
	Process(int arriveTime = 0, int processTime = 0, int turnAround = 0, int ind = 0) {
		this->setArriveTime(arriveTime);
		this->setProcessTime(processTime);
		this->setRemainingTime(processTime);
		this->setTurnAround(turnAround);
		this->setInd(ind);
	}
	Process(const Process& anyProcess) {
		this->setArriveTime(anyProcess.getArriveTime());
		this->setRemainingTime(anyProcess.getRemainingTime());
		this->setProcessTime(anyProcess.getProcessTime());
		this->setInd(anyProcess.getIndex());
		this->setTurnAround(anyProcess.getTurnAround());
	}
};
// scheduler infrastructure :

// operator to insert to the heap by maximium arriving time for LCFS
class operateMaxHeap {
public:
	bool operator()(const Process& A, const Process& B){
		if (A.getArriveTime() == B.getArriveTime())
			return A.getIndex() < B.getIndex();
		return A.getArriveTime() < B.getArriveTime();
	}
};
// operator to insert to the heap by minimum remaining time for SRTF:
class operateMinHeap {
public:
	bool operator()(const Process& A, const Process& B) {
		if (A.getRemainingTime() == B.getRemainingTime()) {
			if (A.getArriveTime() == B.getArriveTime())
				return A.getIndex() > B.getIndex();
			return A.getArriveTime() > B.getArriveTime();
		}
		return A.getRemainingTime() > B.getRemainingTime();
	}
};


// sortBy arriving time before calculate turn around
bool sortByArriveTime(const Process &A,const Process &B) {
	if (A.getArriveTime() == B.getArriveTime())
		return A.getIndex() < B.getIndex();
	return A.getArriveTime() < B.getArriveTime();
}

// sort
void sortProcesses(vector<Process>& processes) {
	sort(processes.begin(), processes.end(),sortByArriveTime);
}

// This function get the arrive time and process time from the line who read from the input file
// and create the Process object.
Process buildProcess(string line, int index) {
	int n = line.length();
	vector<int> nums;
	for (int i = 0; i < n; i++) {
		string temp = "";
		while (i < n && line[i] != ',') {
			temp += line[i];
			i += 1;
		}
		int cur = stoi(temp);
		nums.push_back(cur);
	}
	// arrive time // process time // turnAround // index // 
	Process P = Process(nums[0],nums[1],0,index);
	return P;
}
// This function read from the file, help by buildProcess function to create array of processes and return the sorted array.
vector<Process> readFromFileAndSort(string &path) {
	vector<Process> processes;
	ifstream file;
	file.open(path);
	if (!file.is_open()) {
		cout << "cannot open file" << endl;
		return {};
	}
	string line = "";
	int i = 0;
	while (getline(file, line)) {
		if (i != 0){
			processes.push_back(buildProcess(line , i - 1));
		}
		i += 1;
	}
	file.close();
	sortProcesses(processes);
	return processes;
}
//schedulers:
// FCFS:
double firstComeFirstServe(vector<Process> processes) {
	int globalClock = processes[0].getArriveTime();
	double aveTurnAround = 0;
	double totalTurnAround = 0;
	// Insert to the cpu by arriving time order:
	for (int taskIndex = 0; taskIndex < processes.size(); taskIndex++) {
		int waitingTime = 0;
		if (processes[taskIndex].getArriveTime() < globalClock) {
			waitingTime = globalClock - processes[taskIndex].getArriveTime();
			globalClock += processes[taskIndex].getProcessTime();
		}
		else {
			globalClock = processes[taskIndex].getArriveTime() + processes[taskIndex].getProcessTime();
		}
		int turnAround = waitingTime + processes[taskIndex].getProcessTime();
		processes[taskIndex].setTurnAround(turnAround);
	}
	// calculate the totaturn around of the processes
	for (int i = 0; i < processes.size(); i++) {
		totalTurnAround += processes[i].getTurnAround();
	}
	// return average turn around of first come first serve 
	aveTurnAround = totalTurnAround / processes.size();
	return aveTurnAround;
}
// LCFS -- non preemptive
double lastComeFirstServe(vector<Process> processes) {
	// max priority queue with operator as declare at maxHeap class
	priority_queue<Process, vector<Process>, operateMaxHeap> pq;
	vector<Process> temp;
	int globalClock = 0;
	double aveTurnAround = 0;
	double totalTurnAround = 0;
	int taskIndex = 0;
	while(taskIndex < processes.size() || !pq.empty()){
		
		// if the pq is empty and clock is less than the next task, update clock
		if (pq.empty() && globalClock < processes[taskIndex].getArriveTime()) {
			globalClock = processes[taskIndex].getArriveTime();
		}
		// insert tasks to the pq.
		while (taskIndex < processes.size() && globalClock >= processes[taskIndex].getArriveTime()) {
			pq.push(processes[taskIndex]);
			taskIndex += 1;
		}

		Process top = pq.top();
		pq.pop();
		// Ta = waitingTime + burstTime
		int turnAround = globalClock - top.getArriveTime() + top.getProcessTime();
		top.setTurnAround(turnAround);
		temp.push_back(top);
		globalClock += top.getProcessTime();
	}
	// calculate average turn around time
	for (int i = 0; i < temp.size(); i++) {
		totalTurnAround += temp[i].getTurnAround();
	}
	// return average turn around of last come first serve
	aveTurnAround = totalTurnAround / processes.size();
	return aveTurnAround;
}
// LCFS: -- preemptive
double lastComeFirstServePreemptive(vector<Process> processes) {
	// max priority queue with operator as declare at maxHeap class
	priority_queue<Process, vector<Process>, operateMaxHeap> pq;
	vector<Process> temp;
	int globalClock = 0;
	double aveTurnAround = 0;
	double totalTurnAround = 0;
	// run on the array:
	for(int taskIndex = 0; taskIndex < processes.size(); taskIndex ++){
			
		// while pq is not empty and the remaining time of the top process + globalclock is less or equal the next task
		// we can finish the process withoud cut off. so update total turn around and update clock:
		while (!pq.empty() && globalClock + pq.top().getRemainingTime() <= processes[taskIndex].getArriveTime()) {
			Process top = pq.top();
			totalTurnAround += globalClock + top.getRemainingTime() - top.getArriveTime();
			globalClock += top.getRemainingTime();
			pq.pop();
		}
		// if the pq not empty and we need to cut of the current process
		// update the ramining time and return it to the pq, insert the next to the pq, update clock.
		if (!pq.empty() && globalClock + pq.top().getRemainingTime() > processes[taskIndex].getArriveTime()) {
			Process top = pq.top();
			pq.pop();
			top.setRemainingTime(top.getRemainingTime() - (processes[taskIndex].getArriveTime() - globalClock));
			pq.push(top);
			pq.push(processes[taskIndex]);
			globalClock = processes[taskIndex].getArriveTime();
		}
		// else : the pq is empty( we didnt get in the while and if)
		// insert to the pq and update clock
		if (pq.empty()) {
			pq.push(processes[taskIndex]);
			globalClock = processes[taskIndex].getArriveTime();
		}
	}
	// if the pq is not empty yet, give cpu time by the last come first serve order.
	// there is no preemptive now because there is no more new processes.
	while (!pq.empty()) {
		Process top = pq.top();
		totalTurnAround += globalClock + top.getRemainingTime() - top.getArriveTime();
		globalClock += top.getRemainingTime();
		pq.pop();
	}
	// return average turn around of last come first serve preemprtive
	aveTurnAround = totalTurnAround / processes.size();
	return aveTurnAround;
}
// RR: Time Quantom of 2 ms:
double roundRobin(vector<Process> processes) {

	int n = processes.size();
	int globalClock = 0;
	double aveTurnAround = 0;
	double totalTurnAround = 0;
	int timeQuantom = 2;
	int taskIndex = 0;
	// define Queue of Processes
	queue<Process> Q;
	// top is the current process on the cpu
	while (taskIndex < n || Q.empty() == false) {
		// if the queue empty and clock is less than next process
		// update clock and insert the next process
		if (Q.empty() && globalClock < processes[taskIndex].getArriveTime()) {
			globalClock = processes[taskIndex].getArriveTime();
			Q.push(processes[taskIndex]);
			taskIndex += 1;
		}
		// now the queue has at least one task. 
		//insert it to the cpu for the minimum time between its remaining time and the time Quantom
		Process top = Q.front();
		Q.pop();
		int remainBefore = top.getRemainingTime();
		int cpuTime = min(timeQuantom, remainBefore);
		int remainAfter = remainBefore - cpuTime;
		globalClock += cpuTime;
		// if tasks arrive while the current process on cpu :
		// insert them to the Q.
		while (taskIndex < n && processes[taskIndex].getArriveTime() <= globalClock) {
			Q.push(processes[taskIndex]);
			taskIndex += 1;
		}
		// if the current process has remaining time, insert it to the end of the Q.
		if (remainAfter > 0) {
			top.setRemainingTime(remainAfter);
			Q.push(top);
		}
		// else the process finish and we can update total turn around.
		else {
			totalTurnAround += globalClock - top.getArriveTime();
		}
	}
	// return average turn around of Round Robin.
	aveTurnAround = totalTurnAround / n;
	return aveTurnAround;
}
// SRTF : (compare the remainig time) :
double shortedJobFirstPreemtive(vector<Process> processes) {
	int n = processes.size();
	int globalClock = 0;
	double aveTurnAround = 0;
	double totalTurnAround = 0;
	int taskIndex = 0;
	int cpuTime = 1;
	int step = 1;
	// min priority queue with operator as declare at minHeap class
	priority_queue<Process, vector<Process>, operateMinHeap> pq;
	while (taskIndex < n || pq.empty() == false) {
		// if the priority queue empty and clock is less than next process
		// update clock and insert the next process
		if (pq.empty() && globalClock < processes[taskIndex].getArriveTime()) {
			globalClock = processes[taskIndex].getArriveTime();
			pq.push(processes[taskIndex]);
			taskIndex += 1;
		}
		// if more tasks arrive on this time :
		// insert them to the Q.
		while (taskIndex < n && processes[taskIndex].getArriveTime() <= globalClock) {
			pq.push(processes[taskIndex]);
			taskIndex += 1;
		}
		// insert the minimum remaining time process to the cpu
		Process cur = pq.top();
		pq.pop();
		cpuTime = min(step, cur.getRemainingTime());
		globalClock += cpuTime;
		cur.setRemainingTime(cur.getRemainingTime() - cpuTime);
		// if the current process has remaining time, insert it to the PQ.
		if (cur.getRemainingTime() > 0) {
			pq.push(cur);
		}
		// else the process finish and we can update total turn around.
		else {
			totalTurnAround += globalClock - cur.getArriveTime();
		}
		
	}
	// return average turn around of SRTF.
	aveTurnAround = totalTurnAround / n;
	return aveTurnAround;
}

int main(int argc, char* argv[]) {

	const char* inputFile = argv[1];
	string path(inputFile);
	vector<Process> processes = readFromFileAndSort(path);
	// print results of the average turn around of each algorithm
	cout << "FCFS: mean turnaround = " << firstComeFirstServe(processes) << endl;
	cout << "LCFS(NP) : mean turnaround = " << lastComeFirstServe(processes) << endl;
	cout << "LCFS(P) : mean turnaround = " << lastComeFirstServePreemptive(processes) << endl;
	cout << "RR : mean turnaround = " << roundRobin(processes) << endl;
	cout << "SJF : mean turnaround = " << shortedJobFirstPreemtive(processes) << endl;


	return 0;
}