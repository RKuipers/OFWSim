#include "Deter.h"

Mode Deter::initMode()
{
	Mode mode = Mode();

#ifdef MODECUTS
	string names[MODECUTS + 2] = { "No", "Set", "Ord", "Fin", "Pre", "Res", "Act", "Fai", "Cor", "Dow", "Cuts" };
	mode.AddCombDim(MODECUTS, names);
#endif // MODECUTS

#ifdef MODEFIN
	string names2[MODEFIN] = { "FinAll", "FinMin" };
	mode.AddDim(MODEFIN, names2);
#endif // MODEFIN

#ifdef MODETUNE
	mode.AddDim(MODETUNE, "Tune");
#endif // MODETUNE

#ifdef MODETEST
	mode.AddDim(MODETEST, "TEST");
#endif // MODETEST

#pragma region Locks
#ifdef LOCKMODE
	mode.LockMode(LOCKMODE);
#endif // LOCKMODE

#ifdef LOCKDIM
	mode.LockDim(LOCKDIM);
#endif // LOCKDIM

#ifdef LOCKSET
	mode.LockSetting("SetCuts", LOCKSET);
#endif // LOCKSET

#ifdef LOCKORD
	mode.LockSetting("OrdCuts", LOCKORD);
#endif // LOCKORD

#ifdef LOCKFIN
	mode.LockSetting("FinCuts", LOCKFIN);
#endif // LOCKFIN

#ifdef LOCKPRE
	mode.LockSetting("PreCuts", LOCKPRE);
#endif // LOCKPRE

#ifdef LOCKRES
	mode.LockSetting("ResCuts", LOCKRES);
#endif // LOCKRES

#ifdef LOCKACT
	mode.LockSetting("ActCuts", LOCKACT);
#endif // LOCKACT

#ifdef LOCKFAI
	mode.LockSetting("FaiCuts", LOCKFAI);
#endif // LOCKFAI

#ifdef LOCKCOR
	mode.LockSetting("CorCuts", LOCKCOR);
#endif // LOCKCOR

#ifdef LOCKDOW
	mode.LockSetting("DowCuts", LOCKDOW);
#endif // LOCKDOW
#pragma endregion 

	mode.Resize();
	mode.Reset();

	return mode;
}

void Deter::readData()
{
	// Read data from file
	printer("Reading Data", VERBMODE);

	string line;
	ifstream datafile(string() + INPUTFOLDER + PROBNAME + DATAEXT);
	if (!datafile.is_open())
	{
		cout << "Unable to open file" << endl;
		return;
	}

	// Read the task info
	readTasks(&datafile, 0);
	readTasks(&datafile, 1);
	readTasks(&datafile, 2);
	readTasks(&datafile, 3);

	// Read the resource info
	readResources(&datafile);

	// Read the energy value info
	readValues(&datafile);

	// Read the lambda info
	readLambdas(&datafile);

	// Read the task order info
	readPreqs(&datafile);

	// Generate the weather and StartAt values
	generateWeather();
	generateStartAtValues();

	datafile.close();
}

void Deter::readTasks(ifstream* datafile, int taskType)
{
	string line;
	vector<string>* split = new vector<string>();

	string name;
	int ntasks, start;
	switch (taskType)
	{
	case 0:
		name = "ITASKS";
		ntasks = NITASKS;
		start = 0;
		break;
	case 1:
		name = "MPTASKS";
		ntasks = NMPTASKS;
		start = NITASKS;
		break;
	case 2:
		name = "MCTASKS";
		ntasks = NMCTASKS;
		start = NITASKS + NMPTASKS;
		break;
	case 3:
		name = "DTASKS";
		ntasks = NDTASKS;
		start = NITASKS + NMTASKS;
		break;
	default:
		name = "";
		ntasks = -1;
		start = -1;
		break;
	}

	getline(*datafile, line);
	splitString(line, split);
	if ((*split)[0].compare(name) != 0)
		cout << "Error reading " << name << endl;
	if (stoi((*split)[1]) != ntasks)
		cout << "Error with declared " << name << " amount" << endl;

	int copies = 1;

	for (int i = start; i < ntasks + start; i += copies)
	{
		getline(*datafile, line);
		splitString(line, split, '\t');

		if (count(line.begin(), line.end(), '\t') != 2 + NRES)
			cout << "Error with column count on " << name << " line " << i << endl;

		if ((*split)[0].find(" ") != string::npos)
		{
			vector<string>* dups = new vector<string>();
			splitString((*split)[0], dups, ' ');
			copies = stoi((*dups)[1]) - stoi((*dups)[0]);
		}
		else
			copies = 1;

		for (int x = 0; x < copies; ++x)
		{
			d[i + x] = stoi((*split)[1]);
			limits[i + x] = stoi((*split)[2]);

			for (int r = 0; r < NRES; ++r)
				rho[r][i + x] = stoi((*split)[r + 3]);
		}
	}

	getline(*datafile, line);
}

void Deter::readValues(ifstream* datafile)
{
	string line;
	vector<string>* split = new vector<string>();

	getline(*datafile, line);
	splitString(line, split);
	if ((*split)[0].compare("VALUES") != 0)
		cout << "Error reading VALUES" << endl;
	if (stoi((*split)[1]) != NTIMES)
		cout << "Error with declared VALUES amount" << endl;

	getline(*datafile, line);
	char type = line[0];

	getline(*datafile, line);
	splitString(line, split);
	vector<int> vals = vector<int>(NTIMES);
	parsePeriodical(type, *split, 0, &vals, NTIMES);
	copy(vals.begin(), vals.end(), v);

	getline(*datafile, line);
}

void Deter::readLambdas(ifstream* datafile)
{
	string line;
	vector<string>* split = new vector<string>();

	getline(*datafile, line);
	splitString(line, split);
	if ((*split)[0].compare("LAMBDAS") != 0)
		cout << "Error reading LAMBDAS" << endl;
	if (stoi((*split)[1]) != NMTASKS + 1)
		cout << "Error with declared LAMBDAS amount" << endl;

	int copies = 1;

	for (int i = 0; i < NTASKS; i += copies)
	{
		if (i < NITASKS - 1 || i >= NITASKS + NMTASKS)
		{
			for (int a = 0; a < NASSETS; ++a)
				lambda[a][i] = 0;
			copies = 1;
			continue;
		}

		getline(*datafile, line);
		splitString(line, split, '\t');

		if ((*split)[0].find(" ") != string::npos)
		{
			vector<string>* dups = new vector<string>();
			splitString((*split)[0], dups, ' ');
			copies = stoi((*dups)[1]) - stoi((*dups)[0]);
		}
		else
			copies = 1;

		vector<int> vals = vector<int>(NASSETS);
		parsePeriodical((*split)[1][0], *split, 2, &vals, NASSETS);

		for (int a = 0; a < NASSETS; ++a)
			for (int j = i; j < i + copies; ++j)
				lambda[a][j] = vals[a];
	}

	getline(*datafile, line);
}

void Deter::readPreqs(ifstream* datafile)
{
	string line;
	vector<string>* split = new vector<string>();

	getline(*datafile, line);
	splitString(line, split);
	if ((*split)[0].compare("PREREQUISITES") != 0)
		cout << "Error reading PREREQUISITES" << endl;
	if (stoi((*split)[1]) != NIP)
		cout << "Error with declared PREREQUISITES amount" << endl;

	for (int i = 0; i < NIP; ++i)
	{
		getline(*datafile, line);
		splitString(line, split);
		IP[i] = make_tuple(stoi((*split)[0]), stoi((*split)[1]));
	}

	getline(*datafile, line);
}

void Deter::genDecisionVariables(XPRBprob* prob)
{
	// Create the period-based decision variables
	for (int p = 0; p < NPERIODS; ++p)
		for (int r = 0; r < NRES; ++r)
		{
			N[r][p] = prob->newVar(("N_" + to_string(r) + "_" + to_string(p)).c_str(), XPRB_UI);
			N[r][p].setLB(0);
			N[r][p].setUB(m[r][p]);
		}

	// Create the timestep-based decision variables
	for (int t = 0; t < NTIMES; ++t)
		for (int a = 0; a < NASSETS; ++a)
		{
			o[a][t] = prob->newVar(("o_" + to_string(a) + "_" + to_string(t)).c_str(), XPRB_BV);

			for (int i = 0; i < NTASKS; ++i)
				s[a][i][t] = prob->newVar(("s_" + to_string(a) + "_" + to_string(i) + "_" + to_string(t)).c_str(), XPRB_BV);
		}
}

void Deter::genObjective(XPRBprob* prob)
{
	XPRBctr Obj = prob->newCtr();
	for (int p = 0; p < NPERIODS; ++p)
	{
		double dis = pow(DIS, p);

		for (int t = p * TPP; t < (p + 1) * TPP; ++t)
			for (int a = 0; a < NASSETS; ++a)
				Obj.addTerm(o[a][t], v[t] * dis);

		for (int r = 0; r < NRES; ++r)
			Obj.addTerm(N[r][p], -C[r][p] * dis);
	}
	prob->setObj(Obj); // Set the objective function
}

void Deter::genPartialProblem(XPRBprob* prob, Mode* m)
{
	outputPrinter.printer("Initialising Original constraints", VERBINIT);

	genSetConstraints(prob, m->GetCurrentBySettingName("SetCuts") == 1);
	genOrderConstraints(prob, m->GetCurrentBySettingName("OrdCuts") == 1);
	genFinishConstraints(prob, m->GetCurrentBySettingName("FinCuts") == 1, m->GetCurrentBySettingName("FinAll") == 1);
	genPrecedenceConstraints(prob, m->GetCurrentBySettingName("PreCuts") == 1);
	genResourceConstraints(prob, m->GetCurrentBySettingName("ResCuts") == 1);
	genActiveConstraints(prob, m->GetCurrentBySettingName("ActCuts") == 1);
	genFailureConstraints(prob, m->GetCurrentBySettingName("FaiCuts") == 1);
	genCorrectiveConstraints(prob, m->GetCurrentBySettingName("CorCuts") == 1);
	genDowntimeConstraints(prob, m->GetCurrentBySettingName("DowCuts") == 1);
}

void Deter::genFullProblem(XPRBprob* prob, Mode* m)
{
	clock_t start = clock();

	outputPrinter.printer("Initialising Full constraints", VERBINIT);

	if (m->GetCurrentBySettingName("SetCuts") == 1)
		genSetConstraints(prob, false);
	if (m->GetCurrentBySettingName("OrdCuts") == 1)
		genOrderConstraints(prob, false);
	if (m->GetCurrentBySettingName("FinCuts") == 1)
		genFinishConstraints(prob, false, m->GetCurrentBySettingName("FinAll") == 1);
	if (m->GetCurrentBySettingName("PreCuts") == 1)
		genPrecedenceConstraints(prob, false);
	if (m->GetCurrentBySettingName("ResCuts") == 1)
		genResourceConstraints(prob, false);
	if (m->GetCurrentBySettingName("ActCuts") == 1)
		genActiveConstraints(prob, false);
	if (m->GetCurrentBySettingName("FaiCuts") == 1)
		genFailureConstraints(prob, false);
	if (m->GetCurrentBySettingName("CorCuts") == 1)
		genCorrectiveConstraints(prob, false);
	if (m->GetCurrentBySettingName("DowCuts") == 1)
		genDowntimeConstraints(prob, false);

	double duration = ((double)clock() - start) / (double)CLOCKS_PER_SEC;
	outputPrinter.printer("Duration of initialisation: " + to_string(duration) + " seconds", VERBINIT);
}

void Deter::printProbOutput(XPRBprob* prob, Mode* m, int id)
{
	if (prob->getProbStat() == 1)
		return;

	ofstream file;
	file.open(string() + OUTPUTFOLDER + PROBNAME + to_string(id) + PROBOUTPUTEXT);

	printObj(&file, prob);
	printTurbines(&file);
	printResources(&file);
	printTasks(&file);

	file.close();
}

void Deter::printModeOutput(Mode* m, bool opt)
{
	ofstream file;
	file.open(string() + OUTPUTFOLDER + PROBNAME + "Modes" + MODEOUTPUTEXT);

	printer("----------------------------------------------------------------------------------------", VERBMODE);

#ifdef OPTIMAL
	if (opt)
		printer("All solutions are optimal", VERBMODE);
	else
		printer("Not all solutions are optimal", VERBMODE);
#endif // OPTIMAL

	vector<string> modeNames = m->GetModeNames();
	m->Reset();

	for (int i = 0; i < m->GetNModes(); ++i)
	{
		double dur = m->GetDur(i);
		string setStr = boolVec2Str(m->GetSettingStatus());
		printer("MODE: " + to_string(i) + " (" + modeNames[i] + ") DUR: " + to_string(dur), VERBMODE);
		file << i << ";" << modeNames[i] << ";" << dur << ";" << setStr << endl;
		m->Next();
	}

	file.close();

#if NMODETYPES > 1
	file.open(string() + OUTPUTFOLDER + PROBNAME + "Settings" + MODEOUTPUTEXT);

	vector<string> settingNames = m->GetSettingNames();
	vector<double> setAvgs = m->GetSettingDurs();

	for (int i = 0; i < m->GetNSettings(); ++i)
	{
		printer("SETTING: " + settingNames[i] + " DUR: " + to_string(setAvgs[i]), VERBMODE);
		file << settingNames[i] << ";" << setAvgs[i] << endl;
	}

	file.close();
	file.open(string() + OUTPUTFOLDER + PROBNAME + "Submodes" + MODEOUTPUTEXT);

	vector<string> subModeNames = m->GetCombModeNames();
	vector<double> subModeAvgs = m->GetModeDurs(subModeNames);

	for (int i = 0; i < subModeNames.size(); ++i)
	{
		if (!isnan(subModeAvgs[i]))
			printer("SUBMODE: " + subModeNames[i] + " DUR: " + to_string(subModeAvgs[i]), VERBMODE);
		file << subModeNames[i] << ";" << subModeAvgs[i] << endl;
	}

	file.close();
#endif // NMODETYPES > 1
}
