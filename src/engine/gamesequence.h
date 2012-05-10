#ifndef GAMESEQUENCE_H
#define GAMESEQUENCE_H

#include <QtCore/QList>
#include "player.h"
#include "enums.h"

class GameSequence
{	
	QList<Player*> *mPlayers;
	
	int mCurrentPlayer;
	
public:
	GameSequence();

	void setPlayerList( QList<Player*> *players ){ mPlayers = players; }
	
	void setWhoStartGame( Knapsen::WhoStartGame );
	
	void setCurrentPlayer( Player* );
	Player* getCurrentPlayer(){ return mPlayers->at( mCurrentPlayer ); }
	
	Player* getNextPlayer();
	
	bool isRoundOver();
};

#endif //GAMESEQUENCE_H
