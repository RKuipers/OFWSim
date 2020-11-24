#include "Model.h"

Model::Model(InputData* data) : data(data) { }

Solution* Model::solve()
{
	// TODO: fill in properly
	// TODO: set clocks at right moments

	p.setMsgLevel(0);
	clock_t start = clock();
	p.mipOptimise();
	double dur = ((double)clock() - start) / (double)CLOCKS_PER_SEC;

	return genSolution(&p, dur);
}
