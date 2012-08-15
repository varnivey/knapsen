#ifndef BOT_H
#define BOT_H

#include "client.h"

class Bot : public Client
{
	Q_OBJECT
	
	bool deckIsClosed;
	
	//
	QPair< Card::CardSuit, bool > pairOfKingWasInCentralCards[4];
	QPair< Card::CardSuit, bool > pairOfQueenWasInCentralCards[4];
	
	bool getPairOfKingWasInCentralCards( Card::CardSuit );
	bool getPairOfQueenWasInCentralCards( Card::CardSuit );
	//
	
private slots:
	void slotOpponentDisconnected();
	
	void slotPlayerInAction();
	
	void slotSelectCard();
	
	void slotNewRound();
	
	//
	void slotCloseDeck();
	//
	
	void slotCentralCardChanged( int, Card );
	
	void slotEndRound( QString, int );
	
	void slotendGame( QString );
	
public:
    Bot( QObject* parent = 0 );
	
};

#endif //BOT_H
