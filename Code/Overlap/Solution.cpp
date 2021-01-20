#include "Solution.h"

//-----------------------------------------------BASE----------------------------------------------

Solution::Solution(string name, int id) : modeName(name), modeId(id) 
{ 
	duration = -1;
}

void Solution::setResult(double value, double duration)
{
	this->value = value;
	this->duration = duration;
}

void Solution::printObj()
{
	cout << "Objective value: " << value << endl;
}

void Solution::printDur()
{
	cout << "Duration: " << duration << endl;
}

//-----------------------------------------------YEAR----------------------------------------------

YearSolution::YearSolution(string name, int id) : Solution(name, id) { }

void YearSolution::setVessels(vector<vector<vector<int>>> N)
{
	int Sig = N[0][0].size();
	int Y = N.size();
	int M = N[0].size();

	vessels = vector<vector<vector<int>>>(Sig, vector<vector<int>>(M, vector<int>(Y, -1)));

	for (int sig = 0; sig < Sig; ++sig)
		for (int m = 0; m < M; ++m)
			for (int y = 0; y < Y; ++y)
				vessels[sig][m][y] = N[y][m][sig];
}

void YearSolution::setPlanned(vector<vector<int>> P)
{
	copy(P.begin(), P.end(), back_inserter(planned));
}

void YearSolution::setRepairs(vector<vector<vector<int>>> R)
{
	int Sig = R[0][0].size();
	int I = R[0].size();
	int M = R.size();

	repairs = vector<vector<vector<int>>>(Sig, vector<vector<int>>(M, vector<int>(I, -1)));

	for (int sig = 0; sig < Sig; ++sig)
		for (int m = 0; m < M; ++m)
			for (int i = 0; i < I; ++i)
				repairs[sig][m][i] = R[m][i][sig];
}

void YearSolution::setReactive(vector<vector<vector<int>>> F)
{
	int Sig = F[0][0].size();
	int I = F[0].size();
	int M = F.size();

	reactive = vector<vector<vector<int>>>(Sig, vector<vector<int>>(M, vector<int>(I, -1)));

	for (int sig = 0; sig < Sig; ++sig)
		for (int m = 0; m < M; ++m)
			for (int i = 0; i < I; ++i)
				reactive[sig][m][i] = F[m][i][sig];
}

vector<vector<vector<int>>> YearSolution::getVessels()
{
	return vessels;
}

vector<vector<int>> YearSolution::getPlanned()
{
	return planned;
}

vector<vector<vector<int>>> YearSolution::getRepairs()
{
	return repairs;
}

vector<vector<vector<int>>> YearSolution::getReactive()
{
	return reactive;
}

void YearSolution::printVessels()
{
	for (int sig = 0; sig < vessels.size(); ++sig)
	{
		cout << "Vessels chartered in scenario " << sig << " (per month V and type >):" << endl;

		for (int m = 0; m < vessels[sig].size(); ++m)
		{
			cout << m << ": " << vessels[sig][m][0];

			for (int y = 1; y < vessels[sig][m].size(); ++y)
				cout << ", " << vessels[sig][m][y];

			cout << endl;
		}
	}
}

void YearSolution::printPlanned()
{
	cout << "Planned tasks (per month V and iteration >):" << endl;

	for (int m = 0; m < planned.size(); ++m)
	{
		cout << m << ": " << planned[m][0];

		for (int i = 1; i < planned[m].size(); ++i)
			cout << ", " << planned[m][i];

		cout << endl;
	}
}

void YearSolution::printRepairs()
{
	for (int sig = 0; sig < repairs.size(); ++sig)
	{
		cout << "Repair tasks in scenario " << sig << " (per month V and type >):" << endl;

		for (int m = 0; m < repairs[sig].size(); ++m)
		{
			cout << m << ": " << repairs[sig][m][0];

			for (int i = 1; i < repairs[sig][m].size(); ++i)
				cout << ", " << repairs[sig][m][i];

			cout << endl;
		}
	}
}

void YearSolution::printReactive()
{
	for (int sig = 0; sig < reactive.size(); ++sig)
	{
		cout << "Reactive tasks in scenario " << sig << " (per month V and type >):" << endl;

		for (int m = 0; m < reactive[sig].size(); ++m)
		{
			cout << m << ": " << reactive[sig][m][0];

			for (int i = 1; i < reactive[sig][m].size(); ++i)
				cout << ", " << reactive[sig][m][i];

			cout << endl;
		}
	}
}

void YearSolution::print()
{
	printObj();
	printDur();
	printVessels();
	printPlanned();
	printRepairs();
	printReactive();
}

//-----------------------------------------------MONTH---------------------------------------------

MonthSolution::MonthSolution(string name, int id) : Solution(name, id) { }

void MonthSolution::setFinishes(vector<double> f)
{
	copy(f.begin(), f.end(), back_inserter(finishes));
}

void MonthSolution::setStarts(vector<vector<double>> s)
{
	copy(s.begin(), s.end(), back_inserter(starts));
}

void MonthSolution::setOrders(vector<vector<int>> a)
{
	copy(a.begin(), a.end(), back_inserter(orders));
}

void MonthSolution::printFinishes()
{
	cout << "Finish times for each task:" << endl;

	for (int i = 0; i < finishes.size(); ++i)
		cout << i << ": " << finishes[i] << endl;

}

void MonthSolution::printStarts()
{
	cout << "Start times for each vessel:" << endl;

	for (int v = 0; v < starts.size(); ++v)
	{
		if (starts[v].empty())
			continue;

		cout << v << ": " << starts[v][0];

		for (int j = 1; j < starts[v].size(); ++j)
			cout << ", " << starts[v][j];

		cout << endl;
	}

}

void MonthSolution::printOrders()
{
	cout << "Task order (per vessel):" << endl;

	for (int v = 0; v < orders.size(); ++v)
	{
		if (orders[v].empty())
			continue;

		cout << v << ": " << orders[v][0];

		for (int j = 1; j < orders[v].size(); ++j)
			cout << ", " << orders[v][j];

		cout << endl;
	}
}

void MonthSolution::print()
{
	printObj();
	printDur();
	printFinishes();
	printStarts();
	printOrders();
}
