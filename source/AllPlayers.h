#pragma once

#include "Common.h"
#include "Player.h"
#include <memory>

// search-based players
#include "Player_AlphaBeta.h"
#include "Player_PortfolioGreedySearch.h"
#include "Player_UCT.h"

// script-based players
#include "Player_AttackClosest.h"
#include "Player_AttackDPS.h"
#include "Player_AttackWeakest.h"
#include "Player_Kiter.h"
#include "Player_KiterDPS.h"
#include "Player_NOKDPS.h"
#include "Player_Kiter_NOKDPS.h"
#include "Player_Cluster.h"
#include "Player_Random.h"

// evolution-based players
#include "Player_KiterSD.h"
#include "Player_KiterMvmt.h"
#include "Player_KiterEvo1.h"
#include "Player_KiterEvo2.h"
#include "Player_PortfolioGreedySearchEvo.h"

//POE
#include "Player_POE.h"

namespace SparCraft
{
    
typedef std::shared_ptr<SparCraft::Player> PlayerPtr;

namespace AllPlayers
{
    Player * getPlayer(const IDType & playerID, const IDType & type);
    PlayerPtr getPlayerPtr(const IDType & playerID, const IDType & type);

	std::string getPlayerName(const IDType & type);
}
}