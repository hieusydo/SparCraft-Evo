#include "CooperativeCoevolution.h"
#include <utility> 
#include "Common.h"
using namespace SparCraft;

std::string SEP = "\n========================================================================\n";
bool DEBUG = false;
std::random_device CC_RD;
unsigned int CC_SEED = CC_RD();
std::mt19937_64 CC_ENGINE(CC_SEED);
std::normal_distribution<double> CC_WDISTR(0, 1);
std::normal_distribution<double> CC_MDISTR(0, 0.1);

CooperativeCoevolution::CooperativeCoevolution(size_t mu, size_t lambda, size_t epoch, size_t ecosysSize) 
	: _mu(mu),
	_lambda(lambda),
	_popSize(mu + lambda),
	_epoch(epoch),
	_ecosysSize(ecosysSize) {}

void CooperativeCoevolution::initRandomWeights(vector<WeightDNA>& weights) {
	for (size_t i = 0; i < Constants::MGene_Len; ++i) {
		WeightDNA w(Constants::Num_Params);
		for (size_t j = 0; j < Constants::Num_Params; ++j) { w[j] = CC_WDISTR(CC_ENGINE); }
		weights.push_back(w);
	}
}

std::vector<MGene> CooperativeCoevolution::makeSubpop() {
	std::vector<MGene> newSubpop;
	for (size_t i = 0; i < _popSize; ++i) {
		vector<WeightDNA> weights;
		initRandomWeights(weights);
		MGene c(weights, INT_MIN);
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

int CooperativeCoevolution::eval(const std::vector<GameState>& states, PlayerPtr & p1, PlayerPtr & p2) const {
	int score = 0;
	for (size_t i = 0; i < states.size(); ++i) {
		Game gcopy(states[i], p1, p2, 1000);
		gcopy.play();
		score += gcopy.getState().evalLTD2(Players::Player_One);
	}
	return score / static_cast<int>(states.size());
}

MGene CooperativeCoevolution::mutate(const MGene& c, const std::vector<GameState>& state, PlayerPtr & p1, PlayerPtr & p2) const {
	MGene res = c;

	// Apply mutation to weights
	for (size_t d = 0; d < res.first.size(); ++d) {
		for (size_t i = 0; i < res.first[d].size(); ++i) {
			res.first[d][i] += CC_MDISTR(CC_ENGINE);
			res.second = INT_MIN; 
			// No evaluation here since it will be done at the beginning of the next epoch
		}
	}

	return res;
}

void printMGene(MGene mg, std::ostream& os = std::cout) {
	for (vector<double>& dna : mg.first) {
		for (double w : dna) {
			os << w << " ";
		}
		os << "\n";
	}
	//os << "Gene score: " << mg.second << "\n\n";
	os << "\n";
}

void writeFinalResult(const vector<MGene>& mgs) {
	std::cout << "POE offlince evo complete. Writing final result...\n";
	// result data for each epoch
	std::ofstream finalRes;
	finalRes.open("CC_result/offlineResult.txt");
	for (MGene mg : mgs) {
		for (auto it = mg.first.begin(); it != mg.first.end(); it++) {
			for (size_t i = 0; i < it->size(); ++i) {
				finalRes << it->at(i) << " ";
			}
			finalRes << "\n";
		}
		finalRes << "\n";
	}
	finalRes.close();
}

void CooperativeCoevolution::evolveParams(const std::vector<GameState>& state, PlayerPtr & p1, PlayerPtr & p2) {	
	// Set up the ecosystem
	initEvosys(state, p1, p2);

	vector<MGene> bestGenes(_ecosysSize);

	// summary result for each epoch
	std::ofstream epochDat;
	epochDat.open("CC_result/epochData.txt");
	epochDat << "epoch, bestGenes, score\n";

	// Main cooperative coevolution loop
	for (size_t e = 0; e < _epoch; ++e) {

		std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();

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

			// Evaluate performance of each individual in currentSubpop with reps
			for (size_t ind = 0; ind < _ecosys[currentSubpop].size(); ++ind) {
				// Add the current individual to the portfolio 
				Player_POE* poePlayer = dynamic_cast<Player_POE*>(p1.get());
				poePlayer->clearPortfolio();
				POEPlayer_KiterEvo* curr = new POEPlayer_KiterEvo(poePlayer->ID());
				curr->setWeights(_ecosys[currentSubpop][ind].first);
				poePlayer->addPOEScriptPlayer(POEScriptPlayerPtr(curr));
				// Add other reps to portfolio
				for (size_t r = 0; r < reps.size(); ++r) {
					POEPlayer_KiterEvo* rep = new POEPlayer_KiterEvo(poePlayer->ID());
					rep->setWeights(reps[r].first);
					poePlayer->addPOEScriptPlayer(POEScriptPlayerPtr(rep));
				}

				// Evaluate and record score
				int score = eval(state, p1, p2);
				_ecosys[currentSubpop][ind].second = score;
				
				if (DEBUG) {
					std::cout << SEP << "Epoch " << e << ", subPop " << currentSubpop \
						<< ", ind " << ind << "\n";
					printMGene(_ecosys[currentSubpop][ind]);
					std::cout << "...against other reps:\n";
					for (MGene& mg : reps) { printMGene(mg); }
				}
			}

			sort(_ecosys[currentSubpop].begin(), _ecosys[currentSubpop].end(), MGeneComparator(false));

			bestGenes[currentSubpop] = _ecosys[currentSubpop][0];

			// Remove lambda worst
			for (size_t l = 0; l < _lambda; ++l) {
				_ecosys[currentSubpop].pop_back();
			}
			// Replace lambda worst with mutated mu best
			for (size_t m = 0; m < _lambda; ++m) {
				MGene mg = _ecosys[currentSubpop][m];
				MGene mgMutated = mutate(mg, state, p1, p2);
				_ecosys[currentSubpop].push_back(mgMutated);
			}
		}

		// DEBUG PRINT
		std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
		std::cout << "Epoch " \
			<< e << " - Time elapse " << std::chrono::duration_cast<std::chrono::seconds>(end - start).count()  
			<< " seconds - Best genes scores: ";
		for (MGene& mg : bestGenes) { std::cout << mg.second << " "; }
		std::cout << "\n";

		// Write epoch result to file
		epochDat << SEP << "Epoch " \
			<< e << " - Best genes score: ";
		for (MGene& mg : bestGenes) { epochDat << mg.second << " "; }
		epochDat << "\n";
		for (MGene& mg : bestGenes) { printMGene(mg, epochDat); }

	}

	// Write result to text file
	writeFinalResult(bestGenes);
	epochDat.close();

}