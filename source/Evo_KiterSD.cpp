#include "Evo_KiterSD.h"
#include <queue>
#include <random>
#include <utility> 
#include "Common.h"
using namespace SparCraft;

double MU_RATIO = 0.6;

Evo_KiterSD::Evo_KiterSD(size_t mu, size_t lambda, size_t epoch, size_t evalIter)
	: _mu(mu),
	_lambda(lambda),
	_popSize(mu + lambda),
	_epoch(epoch),
	_evalIter(evalIter) {}

// initialize method for population of many KiterSD
void Evo_KiterSD::initialize(const GameState& state, PlayerPtr & p1, PlayerPtr & p2) {
	std::random_device rd; // get a random seed from the OS entropy device
	std::mt19937_64 eng(rd()); // use the 64-bit Mersenne Twister 19937 generator with the rd seed
	std::uniform_int_distribution<size_t> distr(0, 150); // define the distribution

	Player_KiterSD* kiter = dynamic_cast<Player_KiterSD *>(p1.get());
	kiter->switchOnOffline();
	for (size_t i = 0; i < _popSize; ++i) {
		size_t d = distr(eng);
		kiter->setSafeDist(d);
		int score = eval(state, p1, p2);
		Chromosome c(d, score);
		_genePool.push_back(c);
	}
}

// mutate method
Chromosome Evo_KiterSD::mutate(size_t mutationDelta, const Chromosome& c, const GameState & state, PlayerPtr & p1, PlayerPtr & p2) {
    Chromosome res;
    Player_KiterSD* kiter = dynamic_cast<Player_KiterSD *>(p1.get());
    kiter->switchOnOffline();

    // Consider mutation by wiggling the parent dist +/- mutationDelta
	if (c.first < mutationDelta) {
		kiter->setSafeDist(0);
	}
	else {
		kiter->setSafeDist(c.first - mutationDelta);
	}
    int leftScore = eval(state, p1, p2);

	kiter->setSafeDist(c.first + mutationDelta);
    int rightScore = eval(state, p1, p2);

    // Choose the offspring with better fitness eval score
	res.first = (leftScore > rightScore) ? (c.first < mutationDelta ? 0 : (c.first - mutationDelta)) : (c.first + mutationDelta);
    res.second = (leftScore > rightScore) ? leftScore : rightScore;
    return res;
}

// Find the average score of a kiter with a given safeDist
int Evo_KiterSD::eval(const GameState & state, PlayerPtr & p1, PlayerPtr & p2) {
	//std::cout << "\n \n Evaluating d = " << dynamic_cast<Player_KiterSD *>(p1.get())->getSafeDist() << "\n";
	int kiterScore = 0;
	//for (int i = 0; i < _evalIter; ++i) {
		Game g(state, p1, p2, 1000);
		g.play();
		//kiterScore += g.getState().evalLTD2(Players::Player_One);
		kiterScore = g.getState().evalLTD2(Players::Player_One);
	//}
	//kiterScore /= _evalIter;
	return kiterScore;
}

void Evo_KiterSD::printDist() {
    for (auto it = _genePool.begin(); it != _genePool.end(); it++) {
        std::cout << "(" << it->first << ", S: " << it->second << ") "; 
    }
	std::cout << "\n";
}

// EVOLUTION STRATEGY: http://www.cleveralgorithms.com/nature-inspired/evolution/evolution_strategies.html
size_t Evo_KiterSD::evolveSafeDist(const GameState & state, PlayerPtr & p1, PlayerPtr & p2) {
	Chromosome bestGene;

	// result data for each epoch
	std::ofstream epochDat;
	epochDat.open("epochDat-KiterEvo.txt");
	epochDat << "epoch, score, d\n";

	// Used in calculating mutation delta
	std::random_device rd; // get a random seed from the OS entropy device
	std::mt19937_64 eng(rd()); // use the 64-bit Mersenne Twister 19937 generator with the rd seed
	std::uniform_int_distribution<size_t> deltaDistr(1, 50); // define the distribution

	// initialize population of Kiters to having random safeDist values
	// and evaluate the baseline population 
	initialize(state, p1, p2);
	//printDist();

	bestGene = _genePool[0];
	// main evolution loop
	for (size_t e = 0; e < _epoch; ++e) {
		// remove lambda worst
        std::sort(_genePool.begin(), _genePool.end(), KiterComparator(false));
		for (size_t l = 0; l < _lambda; ++l) {
			_genePool.pop_back();
		}

		// re-mutate mu best
		for (size_t m = 0; m < _mu; ++m) {
			Chromosome c = _genePool[m];
			size_t mutationDelta = deltaDistr(eng);
			Chromosome mutated = mutate(mutationDelta, c, state, p1, p2);
            _genePool.push_back(mutated);
		}

		std::sort(_genePool.begin(), _genePool.end(), KiterComparator(false));
		bestGene = _genePool[0];
		//std::cout << "\n End of gen " << e << " - Best d = " << bestGene.first << " with score = " << bestGene.second << "\n";
		epochDat << e << "," << bestGene.second << "," << bestGene.first << "\n";
		//printDist();
	}
	
	epochDat.close();
    return bestGene.first;
}

