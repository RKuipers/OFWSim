#include "Weathergen.h"

WeatherGenerator::WeatherGenerator(int base, int variety, int nTimes, int tpp)
{
	this->base = base;
	this->variety = variety;
	this->bonus = -variety / 2;
	this->nTimes = nTimes;
	this->tpp = tpp;
}

vector<int> WeatherGenerator::generateWeather()
{
	for (int p = 0; p < nTimes / tpp; ++p)
	{
		waveheights.push_back(base);
		for (int t = (p * tpp) + 1; t < (p + 1) * tpp; ++t)
			waveheights.push_back(max(0, waveheights[t - 1] + bonus + (rand() % variety)));
	}

	return waveheights;
}

vector<vector<int>> WeatherGenerator::generateStartValues(vector<int> durs, vector<int> limits)
{
	vector<vector<int>> res = vector<vector<int>>(durs.size());

	for (int i = 0; i < durs.size(); ++i)
		for (int t = 0; t <= nTimes; ++t)
		{
			if (i == 6)
				i = 6;

			int worked = 0;
			int t2;
			for (t2 = t - 1; worked < durs[i] && t2 >= 0; --t2)
				if (waveheights[t2] <= limits[i])
					worked++;

			if (worked == durs[i])
				res[i].push_back(t2 + 1);
			else
				res[i].push_back(-1);
		}

	return res;
}
