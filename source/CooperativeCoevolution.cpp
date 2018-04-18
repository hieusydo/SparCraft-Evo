#include "CooperativeCoevolution.h"
#include <utility> 
#include "Common.h"
using namespace SparCraft;

CooperativeCoevolution::CooperativeCoevolution(size_t mu, size_t lambda, size_t epoch, size_t ecosysSize) 
	: _mu(mu),
	_lambda(lambda),
	_popSize(mu + lambda),
	_epoch(epoch),
	_ecosysSize(ecosysSize) {}

void CooperativeCoevolution::initRandomWeights(vector<WeightDNA>& weights) {
	for (size_t i = 0; i < Constants::MGene_Len; ++i) {
		WeightDNA w(Constants::Num_Params);
		for (size_t j = 0; j < Constants::Num_Params; ++j) { w[j] = WDISTR(ENGINE); }
		weights.push_back(w);
	}
}

std::vector<MGene> CooperativeCoevolution::makeSubpop() {
	std::vector<MGene> newSubpop;
	for (size_t i = 0; i < _popSize; ++i) {
		vector<WeightDNA> weights;
		initRandomWeights(weights);
		MGene c(weights, LLONG_MIN);
		newSubpop.push_back(c);
	}
	return newSubpop;
}

void CooperativeCoevolution::initEvosys(const std::vector<GameState>& state, PlayerPtr & p1, PlayerPtr & p2) {
	for (size_t i = 0; i < _ecosysSize; ++i) {
		// Create a mu-lambda ES subpop and add it to the ecosystem
		_ecosys.push_back(makeSubpop());
	}
}

void CooperativeCoevolution::evolveParams(const std::vector<GameState>& state, PlayerPtr & p1, PlayerPtr & p2) {
	
	// Set up the ecosystem
	initEvosys(state, p1, p2);

	// Main cooperative coevolution loop
	for (size_t e = 0; e < _epoch; ++e) {
		/* Do ES of this subpop. Note aout fitness score: 
		*		Choose 1 fixed representative from other subpops that are not under evaluation
		*		Calculate the score when this subpop cooperates/behaves with those reps from other subpops
		*/
		for (size_t currentSubpop = 0; currentSubpop < _ecosysSize; ++currentSubpop) {
			// Get representatives
			vector<MGene> reps;
			for (size_t r = 0; r < _ecosysSize; ++r) {
				if (r == currentSubpop) { continue; }
				// Sort subpop and choose best MGene to be rep
				sort(_ecosys[r].begin(), _ecosys[r].end(), MGeneComparator(false));
				reps.push_back(_ecosys[r][0]);
			}

			// Evaluate performance with reps


			// Remove lambda worst
			

			// Re-mutate mu best


		}
	}
}