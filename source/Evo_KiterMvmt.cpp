#include "Evo_KiterMvmt.h"
#include "Evo_KiterSD.h"
#include <queue>
#include <random>
#include <utility> 
#include "Common.h"
using namespace SparCraft;

std::random_device RD;
unsigned int SEED = 1294198436;
std::mt19937_64 ENGINE(SEED);
std::normal_distribution<double> WDISTR(0, 1); 
std::normal_distribution<double> MDISTR(0, 0.1);

Evo_KiterMvmt::Evo_KiterMvmt(size_t mu, size_t lambda, size_t epoch)
	: _mu(mu),
	_lambda(lambda),
	_popSize(mu + lambda),
	_epoch(epoch) {}

void Evo_KiterMvmt::initRandomWeights(std::vector<Array<double, Constants::Num_Params>>& weights) {
	//std::random_device rd; // get a random seed from the OS entropy device
	//std::mt19937_64 eng(SEED); // use the 64-bit Mersenne Twister 19937 generator with the rd seed
	//std::normal_distribution<double> distr(0, 1); // define the distribution

	for (size_t i = 0; i < 4; ++i) {
		Array<double, Constants::Num_Params> w;
		w.init(0);
		weights.push_back(w);
	}

	// map the random number generated by the Mersenne engine to the uniform distribution
	// and add to array: pretty sketchy way to add to Array class...
	for (size_t d = 0; d < weights.size(); ++d) {
		for (size_t i = 0; i < weights[d].capacity(); ++i) { 
			//weights[d][i] = distr(eng); 
			weights[d][i] = WDISTR(ENGINE);
		}
	}
}

// initialize method for population of many KiterMvmt
void Evo_KiterMvmt::initialize(const std::vector<GameState>& states, PlayerPtr & p1, PlayerPtr & p2) {
	Player_KiterMvmt* kiter = dynamic_cast<Player_KiterMvmt *>(p1.get());
	kiter->switchOnOffline();
	for (size_t i = 0; i < _popSize; ++i) {
		std::vector<Array<double, Constants::Num_Params>> weights;
		this->initRandomWeights(weights);
		kiter->setWeights(weights);
		int score = this->eval(states, p1, p2);
		ChromosomeEMP c(weights, score);
		_genePool.push_back(c);
	}
}

// mutate method
ChromosomeEMP Evo_KiterMvmt::mutate(const ChromosomeEMP& c, const std::vector<GameState>& states, PlayerPtr & p1, PlayerPtr & p2) const {
	// Set up mutation delta
	//std::random_device rd; 
	//std::mt19937_64 eng(rd()); 
	//std::normal_distribution<double> deltaDistr(0, 0.1);
	
	ChromosomeEMP res = c;
	Player_KiterMvmt* KiterMvmt = dynamic_cast<Player_KiterMvmt *>(p1.get());
	KiterMvmt->switchOnOffline();

	// Apply mutation to weights
	//auto weights = res.first;
	for (size_t d = 0; d < res.first.size(); ++d) {
		for (size_t i = 0; i < res.first[d].capacity(); ++i) {
			//res.first[d][i] += deltaDistr(eng);
			res.first[d][i] += MDISTR(ENGINE);
			KiterMvmt->setWeights(res.first);
			res.second = this->eval(states, p1, p2);
		}
	}

	return res;
}

// Find the average score of a kiter with a given safeDist
int Evo_KiterMvmt::eval(const std::vector<GameState>& states, PlayerPtr & p1, PlayerPtr & p2) const {
	int kiterScore = 0;
	for (size_t i = 0; i < states.size(); ++i) {
		Game gcopy(states[i], p1, p2, 1000);
		gcopy.play();
		kiterScore += gcopy.getState().evalLTD2(Players::Player_One);
	}
	// static_cast from size_t to int might cause overflow 
	// if the states size exceeds INT_MAX, which won't be the case for this project
	return kiterScore / static_cast<int>(states.size());
}

void Evo_KiterMvmt::writeFinalResult(const ChromosomeEMP& c) const {
	std::cout << "Evolution complete. Writing final result...\n";
	// result data for each epoch
	std::ofstream finalRes;
	finalRes.open("kiterMvmt/finalRes.txt");

	for (auto it = c.first.begin(); it != c.first.end(); it++) {
		for (size_t i = 0; i < it->size(); ++i) {
			finalRes << it->get(i) << " ";
		}
		finalRes << "\n";
	}
	finalRes.close();
}

void Evo_KiterMvmt::printChrom(const ChromosomeEMP& c, std::ostream& os) const {
	for (auto w : c.first) {
		os << w << "\n";
	}
	os << "Score: " << c.second << "\n";
}

// EVOLUTION STRATEGY: http://www.cleveralgorithms.com/nature-inspired/evolution/evolution_strategies.html
void Evo_KiterMvmt::evolveParams(const std::vector<GameState>& states, PlayerPtr & p1, PlayerPtr & p2) {
	std::cout << "Generated seed: " << SEED << "\n";

	// initialize and evaluate the baseline population 
	ChromosomeEMP bestGene;
	this->initialize(states, p1, p2);
	bestGene = _genePool[0];

	// summary result for each epoch
	std::ofstream epochDat;
	epochDat.open("kiterMvmt/epochDat.txt");
	epochDat << "epoch, score\n";
	// raw result for each epoch
	std::ofstream epochRaw;
	epochRaw.open("kiterMvmt/epochRaw.txt");
	epochRaw << "Initial bestGene:\n";
	this->printChrom(bestGene, epochRaw);

	// main evolution loop
	for (size_t e = 0; e < _epoch; ++e) { 		
		// remove lambda worst
		std::sort(_genePool.begin(), _genePool.end(), KiterComparator(false));
		for (size_t l = 0; l < _lambda; ++l) {
			_genePool.pop_back();
		}
		// re-mutate mu best
		for (size_t m = 0; m < _lambda; ++m) {
			ChromosomeEMP c = _genePool[m];
			ChromosomeEMP mutated = mutate(c, states, p1, p2);
			_genePool.push_back(mutated);
		}

		std::sort(_genePool.begin(), _genePool.end(), KiterComparator(false));
		bestGene = _genePool[0];

		epochDat << e << ", " << bestGene.second << "\n";
		epochRaw << "\nbestGene:\n";
		this->printChrom(bestGene, epochRaw);
		epochRaw << "============================== End of epoch " << e << " ==============================\n";
	}

	epochDat.close();
	epochRaw.close();

	// Write result to a .txt file
	this->writeFinalResult(bestGene);
}

