#pragma once

#include "Common.h"
#include "GameState.h"
#include "Player.h"
#include "Action.h"
#include <memory>
#include <set>
#include "Player_NOKDPS.h"
#include "random"

using std::vector;

namespace SparCraft
{
	class POEChromosome{

	public:
		vector<vector<int>> _scriptMap; //row r means at step r, column c means for the cth unit
		// entry at r,c is the index of the script to use for unit c at time r.
		ScoreType _score=std::numeric_limits<int>::min(); //the score of this chromosome, after evaluation
		int _num_scripts;
		int _time_length;
		IDType _poePlayerId;//chromosome generate poe moves for poe player, but only generates NOKAV moves for the enemy player
		POEChromosome(const IDType & player, int num_units, int time_length, int num_scripts, const std::vector<POEScriptPlayerPtr> poePlayerPtrPortfolio);
		void initialize_randomly();//give you a random chromosome
		void initialize_with_script_index(const int script_index);
		void setScore(ScoreType newScore);
		ScoreType POEChromosome::getScore();
		void printOutChromosome();
		const vector<vector<int>> getFullScriptMap();
		const vector<vector<int>> getCopyOfScriptMap();//maybe safer to use this to copy

		void setScriptMapAndResetScore(vector<vector<int>> newScriptMap);//set a new full script map, and reset score

		const vector<int> getScriptMapAtTimestep(const int timestep);
		const int getTimeLength();

		std::vector<std::set<IDType>> _allowedUnitSets;
		std::vector<POEScriptPlayerPtr> _poePlayerPtrPortfolio;

		vector<PlayerPtr> _playoutPlayers;

		void initialize_allowed_unit_sets();
		void clear_allowed_unit_sets();

		void generateMoves(const IDType & player, const MoveArray & moves, GameState & state, const int timestep, std::vector<Action> & moveVec);
		bool hasScoreAlready();
		void resetScore();

		bool  operator < (const POEChromosome& chr) const;
		bool  operator > (const POEChromosome& chr) const;

		void debug_print_scripmap(vector<vector<int>> sm);

			

	};
}