#pragma once
#include <iostream>
#include <cstring>
#include <iostream>
#include <algorithm>
#include <string>
#include <vector>
#include <cstdlib>
#include <stack>
#include <fstream>
#include <msclr/marshal_cppstd.h>
#include <stdlib.h>
#include <queue>
#include <string.h>
#include <sstream>
#include <map>
#include <unordered_map>
#include <cstdlib>
//#include "stdc++.h"



namespace CPUSchedulers {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;
	using namespace std;

	int scheduler_nb;
	double Arrival[100];
	double Burst[100];
	int Priority[100];
	int processes_page = 0;
	unsigned long converted_nb_processes;
	int converted_rr_quantum;

	class process
	{
	public:
		int p_no = 0;
		time_t start_AT = 0, AT = 0, BT_left = 0, BT = 0, temp_BT = 0, CT = 0, TAT = 0, WT = 0, RT = 0;
		int priority = 0;

		// Function for completion time
		void set_CT(time_t time)
		{
			CT = time;
			set_TAT();
			set_WT();
		}

		// Function for Turn Around Time
		void set_TAT()
		{
			TAT = CT - start_AT;
		}

		// Function for Waiting Time
		void set_WT()
		{
			WT = TAT - BT;
		}
		// Function to set starting Arrival Time
		// Because arrival time gets updated
		// when you push process in ready queue again
		// in preemptive algorithms
		void P_set()
		{
			start_AT = AT;
			BT_left = BT;
		}
		// Function to set Response Time
		void set_RT(time_t time)
		{
			RT = time - start_AT;
		}

		// Overload operator '<' w.r.t arrival
		// time because arrival time is the
		// first priority even greater than
		// priority of process and priority_queue
		// pops out the greatest value first
		// so we need to replace '<' with '>' inorder
		// to pop out smallest value
		friend bool operator<(const process& a, const process& b)
		{
			return a.AT > b.AT;
		}
	};
	priority_queue<process> ready_queue, completion_queue;

	void reverse_priority(int P_t[]) {
		int max = 0;
		for (int i = 0; i < converted_nb_processes; i++) if (P_t[i] > max) max = P_t[i];
		for (int i = 0; i < converted_nb_processes; i++) P_t[i] = max - P_t[i] + 1;
	}

	void initialization() {
		process temp;
		//reverse_priority(Priority);
		for (int i = 0; i < converted_nb_processes; i++)
		{
			temp.AT = Arrival[i];
			temp.BT = Burst[i];
			temp.priority = Priority[i];
			temp.p_no = i;
			temp.P_set();
			ready_queue.push(temp);
		}
	}


	// Function to implement FCFS algorithm

	priority_queue<process> FCFS_run(priority_queue<process> ready_queue, queue<process>* gantt)
	{
		priority_queue<process> completion_queue;
		process p;
		time_t clock = 0;

		// Till ready queue is not empty
		while (!ready_queue.empty())
		{
			// While clock is less than
			// Arrival Time
			while (clock < ready_queue.top().AT)
			{
				p.temp_BT++;
				clock++;
			}
			if (p.temp_BT > 0)
			{
				p.p_no = -1;
				p.CT = clock;
				(*gantt).push(p);
			}
			p = ready_queue.top();
			ready_queue.pop();
			p.set_RT(clock);
			while (p.BT_left > 0)
			{
				p.temp_BT++;
				p.BT_left--;
				clock++;
			}
			p.set_CT(clock);

			// Update the Gantt Chart
			(*gantt).push(p);
			p.temp_BT = 0;

			// Update the completion time to
			// the queue
			completion_queue.push(p);
		}
		return completion_queue;
	}

	double get_total_WT(priority_queue<process> processes)
	{
		double total = 0;
		while (!processes.empty())
		{
			total += processes.top().WT;
			processes.pop();
		}
		return total;
	}

	double get_total_TAT(priority_queue<process> processes)
	{
		double total = 0;
		while (!processes.empty()) {
			total += processes.top().TAT;
			processes.pop();
		}
		return total;
	}

	double get_total_RT(priority_queue<process> processes)
	{
		double total = 0;
		while (!processes.empty()) {
			total += processes.top().RT;
			processes.pop();
		}
		return total;
	}

	// Function to display Gantt Chart
	string disp_gantt_chart(queue<process> gantt, priority_queue<process> main_queue)
	{
		string output = "+";
		int temp, prev = 0;
		process p;
		queue<process> tp = gantt;
		while (!tp.empty())
		{
			output += string(to_string(tp.front().p_no).length() + (tp.front().p_no != -1) + 2 * tp.front().temp_BT, '-') + "+";
			tp.pop();
		}
		output += "\n|";
		tp = gantt;

		// For process no. in 2nd row
		while (!tp.empty())
		{
			output += string(tp.front().temp_BT, ' ');
			if (tp.front().p_no == -1)
			{
				output += "IS" + string(tp.front().temp_BT, ' ') + "|";
			}
			else
			{
				output += "P" + to_string(tp.front().p_no) + string(tp.front().temp_BT, ' ') + "|";
			}
			tp.pop();
		}
		tp = gantt;
		output += "\n+";

		while (!tp.empty())
		{
			output += string(to_string(tp.front().p_no).length() + (tp.front().p_no != -1) + 2 * tp.front().temp_BT, '-') + "+";
			tp.pop();
		}
		tp = gantt;
		output += "\n0";

		// For 3rd row of gantt chart
		while (!tp.empty())
		{
			temp = to_string(tp.front().CT).length();
			output += string(to_string(tp.front().p_no).length() + (tp.front().p_no != -1) + 2 * tp.front().temp_BT - temp / 2 - prev, ' ') + string(to_string(tp.front().CT));
			prev = temp / 2 - temp % 2 == 0;
			tp.pop();
		}
		priority_queue<process> tempq = main_queue;
		output += "\n\nAverage waiting time :  " + to_string(get_total_WT(tempq) / main_queue.size());
		output += "\nAverage Turn-around time : " + to_string(get_total_TAT(tempq) / main_queue.size());
		output += "\nAverage Response time : " + to_string(get_total_RT(tempq) / main_queue.size());

		return output;
	}

	priority_queue<process> RR_run(priority_queue<process> ready_queue, time_t Time_Slice, queue<process>* gantt){
		priority_queue<process> completion_queue;
		process p;
		time_t clock = 0;

		while (!ready_queue.empty())
		{
			while (clock < ready_queue.top().AT)
			{
				p.temp_BT++;
				clock++;
			}
			if (p.temp_BT > 0){
				p.p_no = -1;
				p.CT = clock;
				(*gantt).push(p);
			}
			p = ready_queue.top();
			ready_queue.pop();
			if (p.AT == p.start_AT)
				p.set_RT(clock);
			while (p.BT_left > 0 && (p.temp_BT < Time_Slice || ready_queue.empty() || clock < ready_queue.top().AT)){
				p.temp_BT++;
				p.BT_left--;
				clock++;
			}
			if (p.BT_left == 0){
				p.AT = p.start_AT;
				p.set_CT(clock);
				(*gantt).push(p);
				p.temp_BT = 0;
				completion_queue.push(p);
			}
			else{
				p.AT = clock;
				p.CT = clock;
				(*gantt).push(p);
				p.temp_BT = 0;
				ready_queue.push(p);
			}
		}
		return completion_queue;
	}
	
	process pop_index(priority_queue<process>* main_queue, int index)
	{
		priority_queue<process> rm_index;
		int i;
		process p;
		switch (index)
		{
		case 0:
			p = (*main_queue).top();
			(*main_queue).pop();
			break;
		default:
			for (i = 0; i < index; i++)
			{
				rm_index.push((*main_queue).top());
				(*main_queue).pop();
			}
			p = (*main_queue).top();
			(*main_queue).pop();
			while (!(*main_queue).empty())
			{
				rm_index.push((*main_queue).top());
				(*main_queue).pop();
			}
			(*main_queue) = rm_index;
			break;
		}
		return p;
	}

	time_t min_BT(priority_queue<process> main_queue, time_t clock)
	{
		time_t min = 0;
		while (!main_queue.empty() && main_queue.top().AT <= clock)
		{
			if (min == 0 || min > main_queue.top().BT_left)
				min = main_queue.top().BT_left;
			main_queue.pop();
		}
		return min;
	}

	int min_BT_index(priority_queue<process> main_queue, time_t limit)
	{
		int index, i = 0;
		time_t min = 0;
		while (!main_queue.empty() && main_queue.top().AT <= limit)
		{
			if (min == 0 || main_queue.top().BT_left < min)
			{
				min = main_queue.top().BT_left;
				index = i;
			}
			main_queue.pop();
			i++;
		}
		return index;
	}

	// False=lowest number as highest priority
	int max_priority(priority_queue<process> main_priority_queue, int limit, bool high)
	{
		int max = -1;
		if (high == 1) {
			while (!main_priority_queue.empty()
				&& main_priority_queue.top().AT <= limit) {
				if (main_priority_queue.top().priority > max)
					max = main_priority_queue.top().priority;
				main_priority_queue.pop();
			}
		}
		else {
			while (!main_priority_queue.empty()
				&& main_priority_queue.top().AT <= limit) {
				if (max == -1 || main_priority_queue.top().priority < max)
					max = main_priority_queue.top().priority;
				main_priority_queue.pop();
			}
		}
		return max;
	}
	
	// Function to implement maximum priority index
	int max_priority_index(priority_queue<process> main_queue, int limit, bool high)
	{
		int max = -1, i = 0, index = 0;
		if (high == 1) {
			while (!main_queue.empty() && main_queue.top().AT <= limit) {
				if (main_queue.top().priority > max) {
					max = main_queue.top().priority;
					index = i;
				}
				main_queue.pop();
				i++;
			}
		}
		else {
			while (!main_queue.empty()
				&& main_queue.top().AT <= limit) {
				if (max == -1 || main_queue.top().priority < max) {
					max = main_queue.top().priority;
					index = i;
				}
				main_queue.pop();
				i++;
			}
		}
		return index;
	}

	// Function to implement SJF preemptive algorithm
	priority_queue<process> SJF_P_run(priority_queue<process> ready_queue, queue<process>* gantt)
	{
		priority_queue<process> completion_queue;
		process p;
		time_t clock = 0;

		while (!ready_queue.empty())
		{
			while (clock < ready_queue.top().AT)
			{
				p.temp_BT++;
				clock++;
			}
			if (p.temp_BT > 0)
			{
				p.p_no = -1;
				p.CT = clock;
				(*gantt).push(p);
			}
			p = pop_index(&ready_queue, min_BT_index(ready_queue, clock));
			if (p.AT == p.start_AT)
				p.set_RT(clock);
			while (p.BT_left > 0 && (ready_queue.empty() || clock < ready_queue.top().AT || p.BT_left <= min_BT(ready_queue, clock)))
			{
				p.BT_left--;
				p.temp_BT++;
				clock++;
			}
			if (p.BT_left == 0)
			{
				p.AT = p.start_AT;
				p.set_CT(clock);
				(*gantt).push(p);
				p.temp_BT = 0;
				completion_queue.push(p);
			}
			else
			{
				p.AT = clock;
				p.CT = clock;
				(*gantt).push(p);
				p.temp_BT = 0;
				ready_queue.push(p);
			}
		}
		return completion_queue;
	}

	// Function to implement SJF(Non-preemptive)
	priority_queue<process> SJF_NP_run(priority_queue<process> ready_queue, queue<process>* gantt)
	{
		priority_queue<process> completion_queue;
		process p;
		time_t clock = 0;
		while (!ready_queue.empty())
		{
			while (clock < ready_queue.top().AT)
			{
				p.temp_BT++;
				clock++;
			}
			if (p.temp_BT > 0)
			{
				p.p_no = -1;
				p.CT = clock;
				(*gantt).push(p);
			}
			p = pop_index(&ready_queue,
				min_BT_index(ready_queue, clock));
			p.set_RT(clock);
			while (p.BT_left > 0)
			{
				p.temp_BT++;
				p.BT_left--;
				clock++;
			}
			p.set_CT(clock);
			(*gantt).push(p);
			p.temp_BT = 0;
			completion_queue.push(p);
		}
		return completion_queue;
	}

	// Function to implement priority based Preemptive scheduling
	priority_queue<process> Priority_P_run(priority_queue<process> ready_queue, queue<process>* gantt, bool high)
	{
		int temp;
		priority_queue<process> completion_queue;
		process p;
		time_t clock = 0;
		if (high == 1) {
			while (!ready_queue.empty()) {
				while (clock < ready_queue.top().AT) {
					p.temp_BT++;
					clock++;
				}
				if (p.temp_BT > 0) {
					p.p_no = -1;
					p.CT = clock;
					(*gantt).push(p);
				}
				p = pop_index(&ready_queue,
					max_priority_index(ready_queue, clock, high));
				if (p.AT == p.start_AT)
					p.set_RT(clock);
				while (p.BT_left > 0
					&& (ready_queue.empty()
						|| clock < ready_queue.top().AT
						|| p.priority >= max_priority(ready_queue, clock, high))) {
					p.temp_BT++;
					p.BT_left--;
					clock++;
				}
				if (p.BT_left == 0) {
					p.AT = p.start_AT;
					p.set_CT(clock);
					(*gantt).push(p);
					p.temp_BT = 0;
					completion_queue.push(p);
				}
				else {
					p.AT = clock;
					p.CT = clock;
					(*gantt).push(p);
					p.temp_BT = 0;
					ready_queue.push(p);
				}
			}
		}
		else {
			while (!ready_queue.empty()) {
				while (clock < ready_queue.top().AT) {
					p.temp_BT++;
					clock++;
				}
				if (p.temp_BT > 0) {
					p.p_no = -1;
					p.CT = clock;
					(*gantt).push(p);
				}
				p = pop_index(&ready_queue,
					max_priority_index(ready_queue,
						clock, high));

				if (p.AT == p.start_AT)
					p.set_RT(clock);
				temp = max_priority(ready_queue, clock, high);

				while (p.BT_left > 0 && (ready_queue.empty()
					|| clock < ready_queue.top().AT
					|| p.priority <= max_priority(ready_queue, clock, high))) {
					p.temp_BT++;
					p.BT_left--;
					clock++;
				}
				if (p.BT_left == 0) {
					p.AT = p.start_AT;
					p.set_CT(clock);
					(*gantt).push(p);
					p.temp_BT = 0;
					completion_queue.push(p);
				}
				else {
					p.AT = clock;
					p.CT = clock;
					(*gantt).push(p);
					p.temp_BT = 0;
					ready_queue.push(p);
				}
			}
		}

		return completion_queue;
	}

	// Function to implement priority based Non Preemptive scheduling
	priority_queue<process> Priority_NP_run(priority_queue<process> ready_queue, queue<process>* gantt, bool high)
	{
		priority_queue<process> completion_queue;
		process p;
		time_t clock = 0;
		if (high == 1) {
			while (!ready_queue.empty()) {
				while (clock < ready_queue.top().AT) {
					p.temp_BT++;
					clock++;
				}
				if (p.temp_BT > 0) {
					p.p_no = -1;
					p.CT = clock;
					(*gantt).push(p);
				}
				p = pop_index(&ready_queue, max_priority_index(ready_queue, clock, high));
				p.set_RT(clock);

				while (p.BT_left > 0) {
					p.temp_BT++;
					p.BT_left--;
					clock++;
				}
				p.set_CT(clock);
				(*gantt).push(p);
				p.temp_BT = 0;

				completion_queue.push(p);
			}
		}
		else {
			while (!ready_queue.empty()) {
				while (clock < ready_queue.top().AT) {
					p.temp_BT++;
					clock++;
				}
				if (p.temp_BT > 0) {
					p.p_no = -1;
					p.CT = clock;
					(*gantt).push(p);
				}
				p = pop_index(&ready_queue, max_priority_index(ready_queue, clock, high));
				p.set_RT(clock);

				while (p.BT_left > 0) {
					p.temp_BT++;
					p.BT_left--;
					clock++;
				}
				p.set_CT(clock);
				(*gantt).push(p);
				p.temp_BT = 0;

				completion_queue.push(p);
			}
		}
		return completion_queue;
	}


	/// <summary>
	/// Summary for MyForm
	/// </summary>
	public ref class MyForm : public System::Windows::Forms::Form
	{
	public:
		MyForm(void)
		{
			InitializeComponent();
			


			//
			//TODO: Add the constructor code here
			//
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~MyForm()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::Button^ Run_Scheduler;

	private: System::Windows::Forms::Label^ label1;
	private: System::Windows::Forms::Label^ label2;
	private: System::Windows::Forms::Label^ label3;
	private: System::Windows::Forms::ComboBox^ comboBox1;
	private: System::Windows::Forms::TextBox^ Nb_of_Processes;
	private: System::Windows::Forms::Label^ label4;
	private: System::Windows::Forms::Label^ label5;
	private: System::Windows::Forms::Label^ label6;
	private: System::Windows::Forms::Label^ P0;
	private: System::Windows::Forms::Label^ P1;
	private: System::Windows::Forms::Label^ P2;
	private: System::Windows::Forms::Label^ P3;
	private: System::Windows::Forms::Label^ P4;
	private: System::Windows::Forms::Button^ Enter_processes;
	private: System::Windows::Forms::TextBox^ P0_ar;
	private: System::Windows::Forms::TextBox^ P0_bu;
	private: System::Windows::Forms::TextBox^ P0_pr;
	private: System::Windows::Forms::TextBox^ P1_ar;
	private: System::Windows::Forms::TextBox^ P3_ar;
	private: System::Windows::Forms::TextBox^ P1_bu;
	private: System::Windows::Forms::TextBox^ P1_pr;
	private: System::Windows::Forms::TextBox^ P4_bu;
	private: System::Windows::Forms::TextBox^ P2_ar;
	private: System::Windows::Forms::TextBox^ P4_ar;
	private: System::Windows::Forms::TextBox^ P2_bu;
	private: System::Windows::Forms::TextBox^ P3_bu;
	private: System::Windows::Forms::TextBox^ P2_pr;
	private: System::Windows::Forms::TextBox^ P3_pr;
	private: System::Windows::Forms::TextBox^ P4_pr;
	private: System::Windows::Forms::Button^ next;
	private: System::Windows::Forms::Label^ Output_label;
	private: System::Windows::Forms::Label^ Gant_chart;
	private: System::Windows::Forms::TextBox^ rr_quantum_nb;
	private: System::Windows::Forms::Label^ rr_quantum;



	protected:

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->Run_Scheduler = (gcnew System::Windows::Forms::Button());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->comboBox1 = (gcnew System::Windows::Forms::ComboBox());
			this->Nb_of_Processes = (gcnew System::Windows::Forms::TextBox());
			this->label4 = (gcnew System::Windows::Forms::Label());
			this->label5 = (gcnew System::Windows::Forms::Label());
			this->label6 = (gcnew System::Windows::Forms::Label());
			this->P0 = (gcnew System::Windows::Forms::Label());
			this->P1 = (gcnew System::Windows::Forms::Label());
			this->P2 = (gcnew System::Windows::Forms::Label());
			this->P3 = (gcnew System::Windows::Forms::Label());
			this->P4 = (gcnew System::Windows::Forms::Label());
			this->Enter_processes = (gcnew System::Windows::Forms::Button());
			this->P0_ar = (gcnew System::Windows::Forms::TextBox());
			this->P0_bu = (gcnew System::Windows::Forms::TextBox());
			this->P0_pr = (gcnew System::Windows::Forms::TextBox());
			this->P1_ar = (gcnew System::Windows::Forms::TextBox());
			this->P3_ar = (gcnew System::Windows::Forms::TextBox());
			this->P1_bu = (gcnew System::Windows::Forms::TextBox());
			this->P1_pr = (gcnew System::Windows::Forms::TextBox());
			this->P4_bu = (gcnew System::Windows::Forms::TextBox());
			this->P2_ar = (gcnew System::Windows::Forms::TextBox());
			this->P4_ar = (gcnew System::Windows::Forms::TextBox());
			this->P2_bu = (gcnew System::Windows::Forms::TextBox());
			this->P3_bu = (gcnew System::Windows::Forms::TextBox());
			this->P2_pr = (gcnew System::Windows::Forms::TextBox());
			this->P3_pr = (gcnew System::Windows::Forms::TextBox());
			this->P4_pr = (gcnew System::Windows::Forms::TextBox());
			this->next = (gcnew System::Windows::Forms::Button());
			this->Output_label = (gcnew System::Windows::Forms::Label());
			this->Gant_chart = (gcnew System::Windows::Forms::Label());
			this->rr_quantum_nb = (gcnew System::Windows::Forms::TextBox());
			this->rr_quantum = (gcnew System::Windows::Forms::Label());
			this->SuspendLayout();
			// 
			// Run_Scheduler
			// 
			this->Run_Scheduler->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->Run_Scheduler->Location = System::Drawing::Point(760, 412);
			this->Run_Scheduler->Name = L"Run_Scheduler";
			this->Run_Scheduler->Size = System::Drawing::Size(185, 54);
			this->Run_Scheduler->TabIndex = 0;
			this->Run_Scheduler->Text = L"Run Scheduler";
			this->Run_Scheduler->UseVisualStyleBackColor = true;
			this->Run_Scheduler->Visible = false;
			this->Run_Scheduler->Click += gcnew System::EventHandler(this, &MyForm::Run_Scheduler_Click);
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Font = (gcnew System::Drawing::Font(L"Calibri", 16.2F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label1->ForeColor = System::Drawing::Color::Blue;
			this->label1->Location = System::Drawing::Point(12, 9);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(477, 35);
			this->label1->TabIndex = 2;
			this->label1->Text = L"Please choose the type of the Scheduler";
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Font = (gcnew System::Drawing::Font(L"Calibri", 16.2F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label2->ForeColor = System::Drawing::Color::Blue;
			this->label2->Location = System::Drawing::Point(12, 74);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(347, 35);
			this->label2->TabIndex = 3;
			this->label2->Text = L"Choose number of Processes";
			// 
			// label3
			// 
			this->label3->AutoSize = true;
			this->label3->Font = (gcnew System::Drawing::Font(L"Calibri", 13.8F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label3->Location = System::Drawing::Point(13, 155);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(167, 28);
			this->label3->TabIndex = 4;
			this->label3->Text = L"Process Number";
			this->label3->Visible = false;
			// 
			// comboBox1
			// 
			this->comboBox1->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			this->comboBox1->Font = (gcnew System::Drawing::Font(L"Calibri", 13.8F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->comboBox1->FormattingEnabled = true;
			this->comboBox1->Items->AddRange(gcnew cli::array< System::Object^  >(6) {
				L"1. FCFS (First Come First Serve)", L"2. SJF (Preemptive)",
					L"3. SJF (Non Preemptive)", L"4. Priority (Preemptive)", L"5. Priority (Non Preemptive)", L"6. Round Robin"
			});
			this->comboBox1->Location = System::Drawing::Point(558, 10);
			this->comboBox1->Name = L"comboBox1";
			this->comboBox1->Size = System::Drawing::Size(383, 36);
			this->comboBox1->TabIndex = 5;
			this->comboBox1->SelectedIndexChanged += gcnew System::EventHandler(this, &MyForm::comboBox1_SelectedIndexChanged);
			// 
			// Nb_of_Processes
			// 
			this->Nb_of_Processes->Font = (gcnew System::Drawing::Font(L"Calibri", 13.8F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->Nb_of_Processes->Location = System::Drawing::Point(558, 73);
			this->Nb_of_Processes->MaxLength = 5;
			this->Nb_of_Processes->Name = L"Nb_of_Processes";
			this->Nb_of_Processes->Size = System::Drawing::Size(187, 36);
			this->Nb_of_Processes->TabIndex = 6;
			this->Nb_of_Processes->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &MyForm::Nb_of_Processes_KeyPress);
			// 
			// label4
			// 
			this->label4->AutoSize = true;
			this->label4->Font = (gcnew System::Drawing::Font(L"Calibri", 13.8F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label4->Location = System::Drawing::Point(230, 155);
			this->label4->Name = L"label4";
			this->label4->Size = System::Drawing::Size(129, 28);
			this->label4->TabIndex = 7;
			this->label4->Text = L"Arrival Time";
			this->label4->Visible = false;
			// 
			// label5
			// 
			this->label5->AutoSize = true;
			this->label5->Font = (gcnew System::Drawing::Font(L"Calibri", 13.8F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label5->Location = System::Drawing::Point(412, 155);
			this->label5->Name = L"label5";
			this->label5->Size = System::Drawing::Size(115, 28);
			this->label5->TabIndex = 8;
			this->label5->Text = L"Burst Time";
			this->label5->Visible = false;
			// 
			// label6
			// 
			this->label6->AutoSize = true;
			this->label6->Font = (gcnew System::Drawing::Font(L"Calibri", 13.8F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label6->Location = System::Drawing::Point(574, 155);
			this->label6->Name = L"label6";
			this->label6->Size = System::Drawing::Size(83, 28);
			this->label6->TabIndex = 9;
			this->label6->Text = L"Priority";
			this->label6->Visible = false;
			// 
			// P0
			// 
			this->P0->AutoSize = true;
			this->P0->Font = (gcnew System::Drawing::Font(L"Calibri", 13.8F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->P0->Location = System::Drawing::Point(32, 205);
			this->P0->Name = L"P0";
			this->P0->Size = System::Drawing::Size(101, 28);
			this->P0->TabIndex = 10;
			this->P0->Text = L"Process 0";
			this->P0->Visible = false;
			// 
			// P1
			// 
			this->P1->AutoSize = true;
			this->P1->Font = (gcnew System::Drawing::Font(L"Calibri", 13.8F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->P1->Location = System::Drawing::Point(32, 242);
			this->P1->Name = L"P1";
			this->P1->Size = System::Drawing::Size(101, 28);
			this->P1->TabIndex = 11;
			this->P1->Text = L"Process 1";
			this->P1->Visible = false;
			// 
			// P2
			// 
			this->P2->AutoSize = true;
			this->P2->Font = (gcnew System::Drawing::Font(L"Calibri", 13.8F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->P2->Location = System::Drawing::Point(32, 280);
			this->P2->Name = L"P2";
			this->P2->Size = System::Drawing::Size(101, 28);
			this->P2->TabIndex = 12;
			this->P2->Text = L"Process 2";
			this->P2->Visible = false;
			// 
			// P3
			// 
			this->P3->AutoSize = true;
			this->P3->Font = (gcnew System::Drawing::Font(L"Calibri", 13.8F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->P3->Location = System::Drawing::Point(32, 319);
			this->P3->Name = L"P3";
			this->P3->Size = System::Drawing::Size(101, 28);
			this->P3->TabIndex = 13;
			this->P3->Text = L"Process 3";
			this->P3->Visible = false;
			// 
			// P4
			// 
			this->P4->AutoSize = true;
			this->P4->Font = (gcnew System::Drawing::Font(L"Calibri", 13.8F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->P4->Location = System::Drawing::Point(32, 358);
			this->P4->Name = L"P4";
			this->P4->Size = System::Drawing::Size(101, 28);
			this->P4->TabIndex = 14;
			this->P4->Text = L"Process 4";
			this->P4->Visible = false;
			// 
			// Enter_processes
			// 
			this->Enter_processes->Font = (gcnew System::Drawing::Font(L"Calibri", 10.8F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->Enter_processes->Location = System::Drawing::Point(774, 73);
			this->Enter_processes->Name = L"Enter_processes";
			this->Enter_processes->Size = System::Drawing::Size(167, 36);
			this->Enter_processes->TabIndex = 15;
			this->Enter_processes->Text = L"Enter Processes";
			this->Enter_processes->UseVisualStyleBackColor = true;
			this->Enter_processes->Click += gcnew System::EventHandler(this, &MyForm::Enter_processes_Click);
			// 
			// P0_ar
			// 
			this->P0_ar->Font = (gcnew System::Drawing::Font(L"Calibri", 13.8F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->P0_ar->Location = System::Drawing::Point(235, 197);
			this->P0_ar->Name = L"P0_ar";
			this->P0_ar->Size = System::Drawing::Size(124, 36);
			this->P0_ar->TabIndex = 16;
			this->P0_ar->Text = L"0";
			this->P0_ar->Visible = false;
			// 
			// P0_bu
			// 
			this->P0_bu->Font = (gcnew System::Drawing::Font(L"Calibri", 13.8F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->P0_bu->Location = System::Drawing::Point(417, 197);
			this->P0_bu->Name = L"P0_bu";
			this->P0_bu->Size = System::Drawing::Size(124, 36);
			this->P0_bu->TabIndex = 17;
			this->P0_bu->Text = L"0";
			this->P0_bu->Visible = false;
			// 
			// P0_pr
			// 
			this->P0_pr->Font = (gcnew System::Drawing::Font(L"Calibri", 13.8F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->P0_pr->Location = System::Drawing::Point(579, 197);
			this->P0_pr->Name = L"P0_pr";
			this->P0_pr->Size = System::Drawing::Size(124, 36);
			this->P0_pr->TabIndex = 18;
			this->P0_pr->Text = L"1";
			this->P0_pr->Visible = false;
			// 
			// P1_ar
			// 
			this->P1_ar->Font = (gcnew System::Drawing::Font(L"Calibri", 13.8F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->P1_ar->Location = System::Drawing::Point(235, 235);
			this->P1_ar->Name = L"P1_ar";
			this->P1_ar->Size = System::Drawing::Size(124, 36);
			this->P1_ar->TabIndex = 19;
			this->P1_ar->Text = L"0";
			this->P1_ar->Visible = false;
			// 
			// P3_ar
			// 
			this->P3_ar->Font = (gcnew System::Drawing::Font(L"Calibri", 13.8F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->P3_ar->Location = System::Drawing::Point(235, 312);
			this->P3_ar->Name = L"P3_ar";
			this->P3_ar->Size = System::Drawing::Size(124, 36);
			this->P3_ar->TabIndex = 20;
			this->P3_ar->Text = L"0";
			this->P3_ar->Visible = false;
			// 
			// P1_bu
			// 
			this->P1_bu->Font = (gcnew System::Drawing::Font(L"Calibri", 13.8F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->P1_bu->Location = System::Drawing::Point(417, 235);
			this->P1_bu->Name = L"P1_bu";
			this->P1_bu->Size = System::Drawing::Size(124, 36);
			this->P1_bu->TabIndex = 21;
			this->P1_bu->Text = L"0";
			this->P1_bu->Visible = false;
			// 
			// P1_pr
			// 
			this->P1_pr->Font = (gcnew System::Drawing::Font(L"Calibri", 13.8F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->P1_pr->Location = System::Drawing::Point(579, 235);
			this->P1_pr->Name = L"P1_pr";
			this->P1_pr->Size = System::Drawing::Size(124, 36);
			this->P1_pr->TabIndex = 22;
			this->P1_pr->Text = L"1";
			this->P1_pr->Visible = false;
			// 
			// P4_bu
			// 
			this->P4_bu->Font = (gcnew System::Drawing::Font(L"Calibri", 13.8F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->P4_bu->Location = System::Drawing::Point(417, 351);
			this->P4_bu->Name = L"P4_bu";
			this->P4_bu->Size = System::Drawing::Size(124, 36);
			this->P4_bu->TabIndex = 23;
			this->P4_bu->Text = L"0";
			this->P4_bu->Visible = false;
			// 
			// P2_ar
			// 
			this->P2_ar->Font = (gcnew System::Drawing::Font(L"Calibri", 13.8F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->P2_ar->Location = System::Drawing::Point(235, 274);
			this->P2_ar->Name = L"P2_ar";
			this->P2_ar->Size = System::Drawing::Size(124, 36);
			this->P2_ar->TabIndex = 24;
			this->P2_ar->Text = L"0";
			this->P2_ar->Visible = false;
			// 
			// P4_ar
			// 
			this->P4_ar->Font = (gcnew System::Drawing::Font(L"Calibri", 13.8F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->P4_ar->Location = System::Drawing::Point(235, 351);
			this->P4_ar->Name = L"P4_ar";
			this->P4_ar->Size = System::Drawing::Size(124, 36);
			this->P4_ar->TabIndex = 25;
			this->P4_ar->Text = L"0";
			this->P4_ar->Visible = false;
			// 
			// P2_bu
			// 
			this->P2_bu->Font = (gcnew System::Drawing::Font(L"Calibri", 13.8F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->P2_bu->Location = System::Drawing::Point(417, 274);
			this->P2_bu->Name = L"P2_bu";
			this->P2_bu->Size = System::Drawing::Size(124, 36);
			this->P2_bu->TabIndex = 26;
			this->P2_bu->Text = L"0";
			this->P2_bu->Visible = false;
			// 
			// P3_bu
			// 
			this->P3_bu->Font = (gcnew System::Drawing::Font(L"Calibri", 13.8F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->P3_bu->Location = System::Drawing::Point(417, 312);
			this->P3_bu->Name = L"P3_bu";
			this->P3_bu->Size = System::Drawing::Size(124, 36);
			this->P3_bu->TabIndex = 27;
			this->P3_bu->Text = L"0";
			this->P3_bu->Visible = false;
			// 
			// P2_pr
			// 
			this->P2_pr->Font = (gcnew System::Drawing::Font(L"Calibri", 13.8F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->P2_pr->Location = System::Drawing::Point(579, 274);
			this->P2_pr->Name = L"P2_pr";
			this->P2_pr->Size = System::Drawing::Size(124, 36);
			this->P2_pr->TabIndex = 29;
			this->P2_pr->Text = L"1";
			this->P2_pr->Visible = false;
			// 
			// P3_pr
			// 
			this->P3_pr->Font = (gcnew System::Drawing::Font(L"Calibri", 13.8F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->P3_pr->Location = System::Drawing::Point(579, 311);
			this->P3_pr->Name = L"P3_pr";
			this->P3_pr->Size = System::Drawing::Size(124, 36);
			this->P3_pr->TabIndex = 30;
			this->P3_pr->Text = L"1";
			this->P3_pr->Visible = false;
			// 
			// P4_pr
			// 
			this->P4_pr->Font = (gcnew System::Drawing::Font(L"Calibri", 13.8F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->P4_pr->Location = System::Drawing::Point(579, 350);
			this->P4_pr->Name = L"P4_pr";
			this->P4_pr->Size = System::Drawing::Size(124, 36);
			this->P4_pr->TabIndex = 31;
			this->P4_pr->Text = L"1";
			this->P4_pr->Visible = false;
			// 
			// next
			// 
			this->next->Font = (gcnew System::Drawing::Font(L"Calibri", 10.8F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->next->Location = System::Drawing::Point(18, 423);
			this->next->Name = L"next";
			this->next->Size = System::Drawing::Size(185, 36);
			this->next->TabIndex = 32;
			this->next->Text = L"Submit Process(es)";
			this->next->UseVisualStyleBackColor = true;
			this->next->Visible = false;
			this->next->Click += gcnew System::EventHandler(this, &MyForm::next_Click);
			// 
			// Output_label
			// 
			this->Output_label->AutoSize = true;
			this->Output_label->Font = (gcnew System::Drawing::Font(L"Calibri", 18, static_cast<System::Drawing::FontStyle>((System::Drawing::FontStyle::Bold | System::Drawing::FontStyle::Underline)),
				System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
			this->Output_label->Location = System::Drawing::Point(11, 473);
			this->Output_label->Name = L"Output_label";
			this->Output_label->Size = System::Drawing::Size(105, 37);
			this->Output_label->TabIndex = 33;
			this->Output_label->Text = L"Output";
			this->Output_label->Visible = false;
			// 
			// Gant_chart
			// 
			this->Gant_chart->AutoSize = true;
			this->Gant_chart->Font = (gcnew System::Drawing::Font(L"Consolas", 13.8F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->Gant_chart->Location = System::Drawing::Point(32, 524);
			this->Gant_chart->Name = L"Gant_chart";
			this->Gant_chart->Size = System::Drawing::Size(142, 27);
			this->Gant_chart->TabIndex = 34;
			this->Gant_chart->Text = L"Gant Chart";
			this->Gant_chart->Visible = false;
			this->Gant_chart->Click += gcnew System::EventHandler(this, &MyForm::Gant_chart_Click);
			// 
			// rr_quantum_nb
			// 
			this->rr_quantum_nb->Font = (gcnew System::Drawing::Font(L"Calibri", 13.8F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->rr_quantum_nb->Location = System::Drawing::Point(817, 197);
			this->rr_quantum_nb->Name = L"rr_quantum_nb";
			this->rr_quantum_nb->Size = System::Drawing::Size(124, 36);
			this->rr_quantum_nb->TabIndex = 35;
			this->rr_quantum_nb->Text = L"1";
			this->rr_quantum_nb->Visible = false;
			// 
			// rr_quantum
			// 
			this->rr_quantum->AutoSize = true;
			this->rr_quantum->Font = (gcnew System::Drawing::Font(L"Calibri", 13.8F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->rr_quantum->Location = System::Drawing::Point(812, 155);
			this->rr_quantum->Name = L"rr_quantum";
			this->rr_quantum->Size = System::Drawing::Size(133, 28);
			this->rr_quantum->TabIndex = 36;
			this->rr_quantum->Text = L"RR Quantum";
			this->rr_quantum->Visible = false;
			// 
			// MyForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(8, 16);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->BackColor = System::Drawing::Color::Khaki;
			this->ClientSize = System::Drawing::Size(956, 754);
			this->Controls->Add(this->rr_quantum);
			this->Controls->Add(this->rr_quantum_nb);
			this->Controls->Add(this->Gant_chart);
			this->Controls->Add(this->Output_label);
			this->Controls->Add(this->next);
			this->Controls->Add(this->P4_pr);
			this->Controls->Add(this->P3_pr);
			this->Controls->Add(this->P2_pr);
			this->Controls->Add(this->P3_bu);
			this->Controls->Add(this->P2_bu);
			this->Controls->Add(this->P4_ar);
			this->Controls->Add(this->P2_ar);
			this->Controls->Add(this->P4_bu);
			this->Controls->Add(this->P1_pr);
			this->Controls->Add(this->P1_bu);
			this->Controls->Add(this->P3_ar);
			this->Controls->Add(this->P1_ar);
			this->Controls->Add(this->P0_pr);
			this->Controls->Add(this->P0_bu);
			this->Controls->Add(this->P0_ar);
			this->Controls->Add(this->Enter_processes);
			this->Controls->Add(this->P4);
			this->Controls->Add(this->P3);
			this->Controls->Add(this->P2);
			this->Controls->Add(this->P1);
			this->Controls->Add(this->P0);
			this->Controls->Add(this->label6);
			this->Controls->Add(this->label5);
			this->Controls->Add(this->label4);
			this->Controls->Add(this->Nb_of_Processes);
			this->Controls->Add(this->comboBox1);
			this->Controls->Add(this->label3);
			this->Controls->Add(this->label2);
			this->Controls->Add(this->label1);
			this->Controls->Add(this->Run_Scheduler);
			this->Name = L"MyForm";
			this->Text = L"CPU Scheduler Simulator";
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
	private: System::Void Run_Scheduler_Click(System::Object^ sender, System::EventArgs^ e) {
		//MessageBox::Show(Arrival[0].ToString()+"/n"+ Arrival[1].ToString() + "/n"+ Arrival[2].ToString() + "/n"+ Arrival[3].ToString() + "/n"+ Arrival[4].ToString() + "/n"+ Arrival[5].ToString() + "/n");
		if (Burst[0]) {
			Output_label->Visible = true;
			//Gant_chart->Visible = true;
			initialization();
			queue<process> gantt;
			String^ converted_gantt;
			switch (scheduler_nb){
			case(1):
				completion_queue = FCFS_run(ready_queue, &gantt);
				break;
			case(2):
				completion_queue = SJF_P_run(ready_queue, &gantt);
				break;
			case(3):
				completion_queue = SJF_NP_run(ready_queue, &gantt);
				break;
			case(4):
				completion_queue = Priority_P_run(ready_queue, &gantt, 0);
				break;
			case(5):
				completion_queue = Priority_NP_run(ready_queue, &gantt, 0);
				break;
			case(6):
				completion_queue = RR_run(ready_queue,converted_rr_quantum, &gantt);
				break;
			}
			converted_gantt = gcnew String(disp_gantt_chart(gantt, completion_queue).c_str());
			Gant_chart->Visible = true;
			Gant_chart->Text = converted_gantt;

			while (completion_queue.size() > 0) completion_queue.pop();

			while (ready_queue.size() > 0) ready_queue.pop();

			while (gantt.size() > 0) gantt.pop();

		}
	}
	private: System::Void comboBox1_SelectedIndexChanged(System::Object^ sender, System::EventArgs^ e) {
		//clear the textboxes
		/*P0_pr->Text = "1";
		P1_pr->Text = "1";
		P2_pr->Text = "1";
		P3_pr->Text = "1";
		P4_pr->Text = "1";
		P0_ar->Text = "0";
		P0_bu->Text = "0";
		P1_ar->Text = "0";
		P1_bu->Text = "0";
		P2_ar->Text = "0";
		P2_bu->Text = "0";
		P3_ar->Text = "0";
		P3_bu->Text = "0";
		P4_ar->Text = "0";
		P4_bu->Text = "0";*/
		rr_quantum->Visible = false;
		rr_quantum_nb->Visible = false;

		if (comboBox1->Text == "1. FCFS (First Come First Serve)") {
			scheduler_nb = 1;
		}
		if (comboBox1->Text == "2. SJF (Preemptive)") {
			scheduler_nb = 2;
		}
		if (comboBox1->Text == "3. SJF (Non Preemptive)") {
			scheduler_nb = 3;
		}
		if (comboBox1->Text == "4. Priority (Preemptive)") {
			scheduler_nb = 4;
		}
		if (comboBox1->Text == "5. Priority (Non Preemptive)") {
			scheduler_nb = 5;
		}
		if (comboBox1->Text == "6. Round Robin") {
			scheduler_nb = 6;
			rr_quantum->Visible = true;
			rr_quantum_nb->Text = "1";
			rr_quantum_nb->Visible = true;
		}
	}
private: System::Void Enter_processes_Click(System::Object^ sender, System::EventArgs^ e) {
	processes_page = 0;
	try
	{
		converted_nb_processes = System::Convert::ToUInt32((Nb_of_Processes->Text));
	}
	catch (...)
	{
		MessageBox::Show("Please enter an integer number from 1 to 100");

	}
	label3->Visible = true;
	label4->Visible = true;
	label5->Visible = true;
	label6->Visible = false;

	P0->Visible = false;
	P0_ar->Visible = false;
	P0_bu->Visible = false;
	P0_pr->Visible = false;

	P1->Visible = false;
	P1_ar->Visible = false;
	P1_bu->Visible = false;
	P1_pr->Visible = false;

	P2->Visible = false;
	P2_ar->Visible = false;
	P2_bu->Visible = false;
	P2_pr->Visible = false;

	P3->Visible = false;
	P3_ar->Visible = false;
	P3_bu->Visible = false;
	P3_pr->Visible = false;

	P4->Visible = false;
	P4_ar->Visible = false;
	P4_bu->Visible = false;
	P4_pr->Visible = false;

	if (!(converted_nb_processes > 0 && converted_nb_processes < 101) || !scheduler_nb) {
		label3->Visible = false;
		label4->Visible = false;
		label5->Visible = false;
		label6->Visible = false;

		P0->Visible = false;
		P0_ar->Visible = false;
		P0_bu->Visible = false;
		P0_pr->Visible = false;

		P1->Visible = false;
		P1_ar->Visible = false;
		P1_bu->Visible = false;
		P1_pr->Visible = false;

		P2->Visible = false;
		P2_ar->Visible = false;
		P2_bu->Visible = false;
		P2_pr->Visible = false;

		P3->Visible = false;
		P3_ar->Visible = false;
		P3_bu->Visible = false;
		P3_pr->Visible = false;

		P4->Visible = false;
		P4_ar->Visible = false;
		P4_bu->Visible = false;
		P4_pr->Visible = false;

		next->Visible = false;
		MessageBox::Show("Please enter number from 1 to 100 and Choose any type of scheduler");
	}

	if (converted_nb_processes == 1 && scheduler_nb) {
		if (scheduler_nb == 4 || scheduler_nb == 5){
			label6->Visible = true;
			P0_pr->Visible = true;
		}
		P0->Visible = true;
		P0->Text = "Process 0";
		P0_ar->Visible = true;
		P0_bu->Visible = true;

		next->Visible = true;
		next->Text = "Submit Process(es)";

		P1->Visible = false;
		P1_ar->Visible = false;
		P1_bu->Visible = false;
		P1_pr->Visible = false;

		P2->Visible = false;
		P2_ar->Visible = false;
		P2_bu->Visible = false;
		P2_pr->Visible = false;

		P3->Visible = false;
		P3_ar->Visible = false;
		P3_bu->Visible = false;
		P3_pr->Visible = false;

		P4->Visible = false;
		P4_ar->Visible = false;
		P4_bu->Visible = false;
		P4_pr->Visible = false;
	}
	if (converted_nb_processes == 2 && scheduler_nb) {
		if (scheduler_nb == 4 || scheduler_nb == 5) {
			label6->Visible = true;
			P0_pr->Visible = true;
			P1_pr->Visible = true;
		}
		P0->Visible = true;
		P0->Text = "Process 0";
		P0_ar->Visible = true;
		P0_bu->Visible = true;

		P1->Visible = true;
		P1->Text = "Process 1";
		P1_ar->Visible = true;
		P1_bu->Visible = true;

		next->Visible = true;
		next->Text = "Submit Process(es)";

		P2->Visible = false;
		P2_ar->Visible = false;
		P2_bu->Visible = false;
		P2_pr->Visible = false;

		P3->Visible = false;
		P3_ar->Visible = false;
		P3_bu->Visible = false;
		P3_pr->Visible = false;

		P4->Visible = false;
		P4_ar->Visible = false;
		P4_bu->Visible = false;
		P4_pr->Visible = false;
	}
	if (converted_nb_processes == 3 && scheduler_nb) {
		if (scheduler_nb == 4 || scheduler_nb == 5) {
			label6->Visible = true;
			P0_pr->Visible = true;
			P1_pr->Visible = true;
			P2_pr->Visible = true;
		}
		P0->Visible = true;
		P0->Text = "Process 0";
		P0_ar->Visible = true;
		P0_bu->Visible = true;

		P1->Visible = true;
		P1->Text = "Process 1";
		P1_ar->Visible = true;
		P1_bu->Visible = true;

		P2->Visible = true;
		P2->Text = "Process 2";
		P2_ar->Visible = true;
		P2_bu->Visible = true;

		next->Visible = true;
		next->Text = "Submit Process(es)";

		P3->Visible = false;
		P3_ar->Visible = false;
		P3_bu->Visible = false;
		P3_pr->Visible = false;

		P4->Visible = false;
		P4_ar->Visible = false;
		P4_bu->Visible = false;
		P4_pr->Visible = false;
	}
	if (converted_nb_processes == 4 && scheduler_nb) {
		if (scheduler_nb == 4 || scheduler_nb == 5) {
			label6->Visible = true;
			P0_pr->Visible = true;
			P1_pr->Visible = true;
			P2_pr->Visible = true;
			P3_pr->Visible = true;
		}
		P0->Visible = true;
		P0->Text = "Process 0";
		P0_ar->Visible = true;
		P0_bu->Visible = true;
		P1->Visible = true;
		P1->Text = "Process 1";
		P1_ar->Visible = true;
		P1_bu->Visible = true;
		P2->Visible = true;
		P2->Text = "Process 2";
		P2_ar->Visible = true;
		P2_bu->Visible = true;
		P3->Visible = true;
		P3->Text = "Process 3";
		P3_ar->Visible = true;
		P3_bu->Visible = true;

		next->Visible = true;
		next->Text = "Submit Process(es)";

		P4->Visible = false;
		P4_ar->Visible = false;
		P4_bu->Visible = false;
		P4_pr->Visible = false;
	}
	if (converted_nb_processes == 5 && scheduler_nb) {
		if (scheduler_nb == 4 || scheduler_nb == 5) {
			label6->Visible = true;
			P0_pr->Visible = true;
			P1_pr->Visible = true;
			P2_pr->Visible = true;
			P3_pr->Visible = true;
			P4_pr->Visible = true;
		}
		P0->Visible = true;
		P0->Text = "Process 0";
		P0_ar->Visible = true;
		P0_bu->Visible = true;
		P1->Visible = true;
		P1->Text = "Process 1";
		P1_ar->Visible = true;
		P1_bu->Visible = true;
		P2->Visible = true;
		P2->Text = "Process 2";
		P2_ar->Visible = true;
		P2_bu->Visible = true;
		P3->Visible = true;
		P3->Text = "Process 3";
		P3_ar->Visible = true;
		P3_bu->Visible = true;
		P4->Visible = true;
		P4->Text = "Process 4";
		P4_ar->Visible = true;
		P4_bu->Visible = true;

		next->Visible = true;
		next->Text = "Submit Process(es)";
	}
	if (converted_nb_processes > 5 && converted_nb_processes<100 && scheduler_nb) {
		if (scheduler_nb == 4 || scheduler_nb == 5) {
			label6->Visible = true;
			P0_pr->Visible = true;
			P1_pr->Visible = true;
			P2_pr->Visible = true;
			P3_pr->Visible = true;
			P4_pr->Visible = true;
		}
		P0->Visible = true;
		P0->Text = "Process 0";
		P0_ar->Visible = true;
		P0_bu->Visible = true;
		P1->Visible = true;
		P1->Text = "Process 1";
		P1_ar->Visible = true;
		P1_bu->Visible = true;
		P2->Visible = true;
		P2->Text = "Process 2";
		P2_ar->Visible = true;
		P2_bu->Visible = true;
		P3->Visible = true;
		P3->Text = "Process 3";
		P3_ar->Visible = true;
		P3_bu->Visible = true;
		P4->Visible = true;
		P4->Text = "Process 4";
		P4_ar->Visible = true;
		P4_bu->Visible = true;

		next->Visible = true;
		next->Text = "Next Process(es)";
	}
	
	
}
private: System::Void Nb_of_Processes_KeyPress(System::Object^ sender, System::Windows::Forms::KeyPressEventArgs^ e) {
	//if (!char.IsControl(e.KeyChar) && !char.IsDigit(e.KeyChar) && (e.KeyChar != '.'))
	//{
	//	e.Handled = true;
	//}


}
private: System::Void next_Click(System::Object^ sender, System::EventArgs^ e) {
	//, System::Globalization::CultureInfo::InvariantCulture
	try
	{
		Priority[processes_page] = System::Convert::ToDouble(P0_pr->Text);
		Priority[processes_page + 1] = System::Convert::ToDouble(P1_pr->Text);
		Priority[processes_page + 2] = System::Convert::ToDouble(P2_pr->Text);
		Priority[processes_page + 3] = System::Convert::ToDouble(P3_pr->Text);
		Priority[processes_page + 4] = System::Convert::ToDouble(P4_pr->Text);

		Arrival[processes_page] = System::Convert::ToDouble(P0_ar->Text);
		Arrival[processes_page + 1] = System::Convert::ToDouble(P1_ar->Text);
		Arrival[processes_page + 2] = System::Convert::ToDouble(P2_ar->Text);
		Arrival[processes_page + 3] = System::Convert::ToDouble(P3_ar->Text);
		Arrival[processes_page + 4] = System::Convert::ToDouble(P4_ar->Text);

		Burst[processes_page] = System::Convert::ToDouble(P0_bu->Text);
		Burst[processes_page + 1] = System::Convert::ToDouble(P1_bu->Text);
		Burst[processes_page + 2] = System::Convert::ToDouble(P2_bu->Text);
		Burst[processes_page + 3] = System::Convert::ToDouble(P3_bu->Text);
		Burst[processes_page + 4] = System::Convert::ToDouble(P4_bu->Text);

	}
	catch (...)
	{
		MessageBox::Show("Please enter Valid data (numeric values)");
	}

	if (next->Text == "Submit Process(es)") {
		Run_Scheduler->Visible = true;
		converted_rr_quantum = System::Convert::ToUInt32(rr_quantum_nb->Text);
	}

	if (next->Text == "Next Process(es)") {
		//clear the textboxes
		P0_pr->Text = "1";
		P1_pr->Text = "1";
		P2_pr->Text = "1";
		P3_pr->Text = "1";
		P4_pr->Text = "1";
		P0_ar->Text = "0";
		P0_bu->Text = "0";
		P1_ar->Text = "0";
		P1_bu->Text = "0";
		P2_ar->Text = "0";
		P2_bu->Text = "0";
		P3_ar->Text = "0";
		P3_bu->Text = "0";
		P4_ar->Text = "0";
		P4_bu->Text = "0";
		processes_page = processes_page + 5;



		if (System::Convert::ToUInt32(Nb_of_Processes->Text) > 5 && System::Convert::ToUInt32(Nb_of_Processes->Text) % 5 == 1) {
			if (processes_page > System::Convert::ToUInt32(Nb_of_Processes->Text) - 6) {
				next->Text = "Submit Process(es)";
			}
			P0->Text = "Process " + (processes_page);
			P1->Visible = false;
			P1_ar->Visible = false;
			P1_bu->Visible = false;
			P1_pr->Visible = false;

			P2->Visible = false;
			P2_ar->Visible = false;
			P2_bu->Visible = false;
			P2_pr->Visible = false;

			P3->Visible = false;
			P3_ar->Visible = false;
			P3_bu->Visible = false;
			P3_pr->Visible = false;

			P4->Visible = false;
			P4_ar->Visible = false;
			P4_bu->Visible = false;
			P4_pr->Visible = false;

		}
		if (System::Convert::ToUInt32(Nb_of_Processes->Text) > 5 && System::Convert::ToUInt32((Nb_of_Processes->Text)) % 5 == 2) {
			if (processes_page > System::Convert::ToUInt32(Nb_of_Processes->Text) - 6) {
				next->Text = "Submit Process(es)";
			}
			P0->Text = "Process " + (processes_page);
			P1->Text = "Process " + (processes_page + 1);

			P2->Visible = false;
			P2_ar->Visible = false;
			P2_bu->Visible = false;
			P2_pr->Visible = false;

			P3->Visible = false;
			P3_ar->Visible = false;
			P3_bu->Visible = false;
			P3_pr->Visible = false;

			P4->Visible = false;
			P4_ar->Visible = false;
			P4_bu->Visible = false;
			P4_pr->Visible = false;

		}
		if (System::Convert::ToUInt32(Nb_of_Processes->Text) > 5 && System::Convert::ToUInt32((Nb_of_Processes->Text)) % 5 == 3) {
			if (processes_page > System::Convert::ToUInt32(Nb_of_Processes->Text) - 6) {
				next->Text = "Submit Process(es)";
			}
			P0->Text = "Process " + (processes_page);
			P1->Text = "Process " + (processes_page + 1);
			P2->Text = "Process " + (processes_page + 2);

			P3->Visible = false;
			P3_ar->Visible = false;
			P3_bu->Visible = false;
			P3_pr->Visible = false;

			P4->Visible = false;
			P4_ar->Visible = false;
			P4_bu->Visible = false;
			P4_pr->Visible = false;

		}
		if (System::Convert::ToUInt32(Nb_of_Processes->Text) > 5 && System::Convert::ToUInt32((Nb_of_Processes->Text)) % 5 == 4) {
			if (processes_page > System::Convert::ToUInt32(Nb_of_Processes->Text) - 6) {
				next->Text = "Submit Process(es)";
			}
			P0->Text = "Process " + (processes_page);
			P1->Text = "Process " + (processes_page + 1);
			P2->Text = "Process " + (processes_page + 2);
			P3->Text = "Process " + (processes_page + 3);

			P4->Visible = false;
			P4_ar->Visible = false;
			P4_bu->Visible = false;
			P4_pr->Visible = false;

		}
		if ((System::Convert::ToUInt32(Nb_of_Processes->Text) > 5 && System::Convert::ToUInt32((Nb_of_Processes->Text)) % 5 == 0)){
			if (processes_page > System::Convert::ToUInt32(Nb_of_Processes->Text) - 6) {
				next->Text = "Submit Process(es)";
			}
			P0->Text = "Process " + (processes_page);
			P1->Text = "Process " + (processes_page + 1);
			P2->Text = "Process " + (processes_page + 2);
			P3->Text = "Process " + (processes_page + 3);
			P4->Text = "Process " + (processes_page + 4);

		}
		if (processes_page <= System::Convert::ToUInt32(Nb_of_Processes->Text) - 6) {
			P0->Visible = true;
			P0_ar->Visible = true;
			P0_bu->Visible = true;
			P1->Visible = true;
			P1_ar->Visible = true;
			P1_bu->Visible = true;
			P2->Visible = true;
			P2_ar->Visible = true;
			P2_bu->Visible = true;
			P3->Visible = true;
			P3_ar->Visible = true;
			P3_bu->Visible = true;
			P4->Visible = true;
			P4_ar->Visible = true;
			P4_bu->Visible = true;
			P0->Text = "Process " + (processes_page);
			P1->Text = "Process " + (processes_page + 1);
			P2->Text = "Process " + (processes_page + 2);
			P3->Text = "Process " + (processes_page + 3);
			P4->Text = "Process " + (processes_page + 4);

			if (scheduler_nb == 4 || scheduler_nb == 5) {
				label6->Visible = true;
				P0_pr->Visible = true;
				P1_pr->Visible = true;
				P2_pr->Visible = true;
				P3_pr->Visible = true;
				P4_pr->Visible = true;
			}
		}
	}
	

}
private: System::Void Gant_chart_Click(System::Object^ sender, System::EventArgs^ e) {
}
};
}
