#include <QtCore/QTimer>
#include <KDE/KDebug>
#include "gamesequence.h"
#include "deck.h"
#include "player.h"
#include "server.h"

//Only 2 players game, 4 players maybe in the future...
const int MAX_PLAYERS = 2;

const int MAX_CENTRAL_CARDS_SIZE = 2;

Server::Server( QObject* parent ) :
	QTcpServer( parent ),
	mAdminName( "" ),
	mTwentyButtonClickedThisTurn( false ),
	mFortyButtonClickedThisTurn( false ),
	mClickedToCloseButtonThisTurn( false )//,
	//mClickedToCloseButtonThisRound( false ) 
{
	kDebug() << "Initialize.";

	mGameSequence = new GameSequence;
	mGameSequence->setPlayerList( &mPlayerList );
	
	//Defaults
	mSizeOfDeck = 20;
	mNumberOfCardsInHand = 5;
	mTypeOfCards = Knapsen::GermanSuits;
	
	mDeck = 0;
	mPlayerWhoClickedToCloseButtonThisRound = 0;
	mWaitingMarriage = 0;
}

Server::~Server()
{
	kDebug() << "Server is deleting.";
	if( mGameSequence ){
		delete mGameSequence;
	}
	if( mDeck ){
		delete mDeck;
	}
}

void Server::roundOver()
{
	kDebug() << "---- Round Over ----";
	
	//Player *winnerPlayer = mGameSequence->getCurrentPlayer();
	//Player *looserPlayer = mGameSequence->getNextPlayer();
	
	Player *currentPlayer = mGameSequence->getCurrentPlayer();
	Player *nextPlayer = mGameSequence->getNextPlayer();
	
	Player *winnerPlayer = 0; //Initialize with 0, becouse the compiler write warning.
	Player *looserPlayer = 0; // --- || ---
	
	if( currentPlayer->getTricks() >= 66 ){
		kDebug() << "Current player have more then 66 tricks.";
		
		winnerPlayer = currentPlayer;
		looserPlayer = nextPlayer;
	}else if( nextPlayer->getTricks() >= 66 ){
		kDebug() << "Next player have more then 66 tricks.";
		
		winnerPlayer = nextPlayer;
		looserPlayer = currentPlayer;
	}else if( currentPlayer->getNumberOfCardsInHandNow() == 0 ){
		kDebug() << "Players number of cards in hand now is 0";
		
		winnerPlayer = currentPlayer;
		looserPlayer = nextPlayer;
	}
	
	if( mPlayerWhoClickedToCloseButtonThisRound == looserPlayer ){
		Player *tmpPlayer = winnerPlayer;
		winnerPlayer = looserPlayer;
		looserPlayer = tmpPlayer;
	}
	
	kDebug() << "Win the round:" << winnerPlayer->getName();
	
	int scores( 0 );
	
	if( !mPlayerWhoClickedToCloseButtonThisRound || mPlayerWhoClickedToCloseButtonThisRound == winnerPlayer ){
		if( looserPlayer->getTricks() == 0 ){
			scores = 3;
		}else if( looserPlayer->getTricks() < 33 ){
			scores = 2;
		}else{ //  33 <= looserPlayer->getTricks() && looserPlayer->getTricks() < 66
			scores = 1;
		}
	}else{ //The loser player closed the deck
		
		//Under developing.
		
	}
	
	
	
}

void Server::slotNewPlayer( Player* player )
{
	kDebug() << player->getName();
	
	if( mPlayerList.size() < MAX_PLAYERS ){
		
		for( int i = 0; i < mPlayerList.size(); ++i ){
			if( mPlayerList.at(i)->getName() == player->getName() ){
				kDebug() << "The \"" << player->getName() << "\" name is busy.";
				return;
			}
		}
		
		mPlayerList.append( player );
		connect( player, SIGNAL( signalPlayerDisconnected( Player* ) ),	this, SLOT( slotPlayerDisconnected( Player* ) ) );
		connect( player, SIGNAL( signalSelectedCard( Card, int ) ),		this, SLOT( slotPlayerSelectedCard( Card, int ) ) ); 
		connect( player, SIGNAL( signalTwentyButtonClicked() ),			this, SLOT( slotPlayerTwentyButtonClicked() ) );
		connect( player, SIGNAL( signalFortyButtonClicked() ),			this, SLOT( slotPlayerFortyButtonClicked() ) );
		connect( player, SIGNAL( signalCloseButtonClicked() ),			this, SLOT( slotPlayerClickedToCloseButton() ) );
		connect( player, SIGNAL( signalChangedTrumpCard( Card ) ),		this, SLOT( slotPlayerChangedTrumpCard( Card ) ) );
		emit signalPlayerConnected( player->getName() );
		
		if( mPlayerList.size() == MAX_PLAYERS ){
			kDebug() << "The server is full.";
			
			emit signalServerFull();
		}
		
	}else{
		kDebug() << "The server is full, no free slot.";
	}
	
}

void Server::slotPlayerDisconnected( Player* player )
{
	kDebug() << player->getName() << "disconnected.";
	
	int i = 0;
	while( mPlayerList.at(i)->getName() != player->getName() ){
		i++;
	}
	
	mPlayerList.removeAt(i);
	
	//
	player->disconnectFromHost();
	player->deleteLater();
	//
	
	emit signalPlayerDisconnected( player->getName() );
	
	if( mPlayerList.isEmpty() ){
		kDebug() << "Server is empty!";
		emit signalServerEmpty();
	}else{
		
		for( i = 0; i < mPlayerList.size(); ++i ){
			mPlayerList.at( i )->sendOpponentDisconnected();
		}
		
	}
}

void Server::slotPlayerSelectedCard( Card selectedCard, int cardPosition )
{
	kDebug() << "Selected card:" << selectedCard.getCardText( mTypeOfCards );
	
	mCentralCards.append( selectedCard );
	
	Player* nextPlayer = mGameSequence->getNextPlayer();
	
	if( mCentralCards.size() != MAX_CENTRAL_CARDS_SIZE ){
		kDebug() << "Next player step.";
		
		Player* currentPlayer = mGameSequence->getCurrentPlayer();
		//Player* nextPlayer = mGameSequence->getNextPlayer();
		
		//If the previous player clicked to twenty/forty button, then show that card to current player
		if( mTwentyButtonClickedThisTurn ){
			kDebug() << "Pair this card position:" << currentPlayer->getPositionOfPairOfCard( selectedCard );
			int posOfPairOfCard = currentPlayer->getPositionOfPairOfCard( selectedCard );
			nextPlayer->sendVisibleOpponentCards( cardPosition, selectedCard, posOfPairOfCard, currentPlayer->getCard( posOfPairOfCard ) );
		
			//
			if( currentPlayer->getTricks() > 0 ){
				
				currentPlayer->addTricks( 20 );
				
				if( mGameSequence->isRoundOver() ){
					roundOver();
				}
				
			}else{ //currentPlayer->getTricks() == 0
				mWaitingMarriage = new QPair< Player*, int >;
				mWaitingMarriage->first = currentPlayer;
				mWaitingMarriage->second = 20;
			}
			
			mTwentyButtonClickedThisTurn = false;
		}
		
		if( mFortyButtonClickedThisTurn ){
			kDebug() << "Pair this card position:" << currentPlayer->getPositionOfPairOfCard( selectedCard );
			int posOfPairOfCard = currentPlayer->getPositionOfPairOfCard( selectedCard );
			nextPlayer->sendVisibleOpponentCards( cardPosition, selectedCard, posOfPairOfCard, currentPlayer->getCard( posOfPairOfCard ) );
			
			//
			if( currentPlayer->getTricks() > 0 ){
				
				currentPlayer->addTricks( 40 );
				
				if( mGameSequence->isRoundOver() ){
					roundOver();
				}
				
			}else{ //currentPlayer->getTricks() == 0
				mWaitingMarriage = new QPair< Player*, int >;
				mWaitingMarriage->first = currentPlayer;
				mWaitingMarriage->second = 40;
			}
			
			mFortyButtonClickedThisTurn = false;
		}
		
		if( mClickedToCloseButtonThisTurn ){
			nextPlayer->sendOpponentClickedToCloseButton();
		}
		
		nextPlayer->sendOpponentSelectedCardId( cardPosition );
		nextPlayer->sendOpponentAddNewCentralCard( selectedCard );
		
		mGameSequence->setCurrentPlayer( nextPlayer );
		
		//if( !mClickedToCloseButtonThisRound && mDeck->getDeckSize() > 0 ){
		if( !mPlayerWhoClickedToCloseButtonThisRound && mDeck->getDeckSize() > 0 ){
			nextPlayer->sendSelectableAllCards();
		}else{ // mDeck->getDeckSize() == 0
			nextPlayer->sendSelectableCertainCards();
		}
	
		//The commands end command send in the player class
		
	}else{
		nextPlayer->sendOpponentSelectedCardId( cardPosition );
		nextPlayer->sendOpponentAddNewCentralCard( selectedCard );
		QTimer::singleShot( 1000, this, SLOT( slotCheckCentralCards() ) );
	}
	
	nextPlayer->sendCommandsEnd();
}

void Server::slotPlayerTwentyButtonClicked()
{
	kDebug() << mGameSequence->getCurrentPlayer()->getName() << "Clicked to twenty button.";
	mTwentyButtonClickedThisTurn = true;
}

void Server::slotPlayerFortyButtonClicked()
{
	kDebug() << mGameSequence->getCurrentPlayer()->getName() << "Clicked to forty button.";
	mFortyButtonClickedThisTurn = true;
}

void Server::slotPlayerClickedToCloseButton()
{
	kDebug() << mGameSequence->getCurrentPlayer()->getName() << "Clicked to close button.";
	mClickedToCloseButtonThisTurn = true;
	//mClickedToCloseButtonThisRound = true;
	mPlayerWhoClickedToCloseButtonThisRound = mGameSequence->getCurrentPlayer();
}

void Server::slotPlayerChangedTrumpCard( Card newTrumpCard )
{
	mTrumpCard = newTrumpCard;
	
	for( int i = 0; i < mPlayerList.size(); ++i ){
		
		if( mPlayerList.at(i) != mGameSequence->getCurrentPlayer() ){
			mPlayerList.at(i)->sendNewTrumpCard( mTrumpCard );
		}
		
	}
}

void Server::slotCheckCentralCards()
{
	
	kDebug() << "Check the central cards.";
		
	int centralCard1Point = mCentralCards.at(0).getCardPoint();
	int centralCard2Point = mCentralCards.at(1).getCardPoint();
		
	kDebug() << "centralCard1Point" << centralCard1Point;
	kDebug() << "centralCard2Point" << centralCard2Point;
		
	bool currentPlayerStartNextTurn;
		
	if( mTrumpCard.getCardSuit() == mCentralCards.at(0).getCardSuit() && mTrumpCard.getCardSuit() != mCentralCards.at(1).getCardSuit() ){
		currentPlayerStartNextTurn = false;
	}else if( mTrumpCard.getCardSuit() != mCentralCards.at(0).getCardSuit() && mTrumpCard.getCardSuit() == mCentralCards.at(1).getCardSuit() ){
		currentPlayerStartNextTurn = true;
	}else if( mCentralCards.at(0).getCardSuit() == mCentralCards.at(1).getCardSuit() ){
		
		if( centralCard1Point < centralCard2Point ){
			currentPlayerStartNextTurn = true;
		}else{
			currentPlayerStartNextTurn = false;
		}
		
	}else{ // mCentralCards.at(0).getCardType != mCentralCards.at(1).getCardType
		currentPlayerStartNextTurn = false;
	}
		
	if( !currentPlayerStartNextTurn ){
		mGameSequence->setCurrentPlayer( mGameSequence->getNextPlayer() );
	}
	
	Player* currentPlayer = mGameSequence->getCurrentPlayer();
		
	kDebug() << currentPlayer->getName() << "get" << centralCard1Point+centralCard2Point << "points.";
	currentPlayer->addTricks( centralCard1Point + centralCard2Point );
	
	if( mWaitingMarriage && mWaitingMarriage->first == currentPlayer ){
		currentPlayer->addTricks( mWaitingMarriage->second );
		delete mWaitingMarriage;
		mWaitingMarriage = 0;
	}
		
	if( mClickedToCloseButtonThisTurn ){
		mClickedToCloseButtonThisTurn = false;
	}
		
	if( mGameSequence->isRoundOver() ){
		roundOver();
	}else{
		kDebug() << "Start next turn";
		
		//Clear central cards
		mCentralCards.clear();
		for( int i = 0; i < mPlayerList.size(); ++i ){
			mPlayerList.at( i )->sendClearCentralCards();
		}
		
		//if( !mClickedToCloseButtonThisRound ){
		if( !mPlayerWhoClickedToCloseButtonThisRound ){
			//If the dech have card yet, then add new cards to players, frist who won the last turn
			if( mDeck->getDeckSize() > 0 ){
				currentPlayer->sendNewCard( mDeck->getCard() );
			
				if( mDeck->getDeckSize() > 0 ){
					mGameSequence->getNextPlayer()->sendNewCard( mDeck->getCard() );
				}else{
					mGameSequence->getNextPlayer()->sendNewCard( mTrumpCard );
					mTrumpCard = Card();
				
					for( int i = 0; i < mPlayerList.size(); ++i ){
						mPlayerList.at( i )->sendClearTrumpCard();
					}
				
				}
			
			}
		}
		
		currentPlayer->sendSelectableAllCards();
		
		if( currentPlayer->haveRegularMarriages() ){
			kDebug() << currentPlayer->getName() << "have regular marriages.";
			currentPlayer->sendTwentyButtonVisible();
		}
	
		if( currentPlayer->haveTrumpMarriages() ){
			kDebug() << currentPlayer->getName() << "have trump marriages.";
			currentPlayer->sendFortyButtonVisible();
		}
		
		if( currentPlayer->canChangeTrumpCard() ){
			kDebug() << currentPlayer->getName() << "can change trump card.";
			currentPlayer->sendSelectableTrumpCard();
		}
		
		//if( !mClickedToCloseButtonThisRound && mDeck->getDeckSize() > 3 ){
		if( !mPlayerWhoClickedToCloseButtonThisRound && mDeck->getDeckSize() > 3 ){
			currentPlayer->sendCloseButtonVisible();
		}
		
	}
	
	for( int i = 0; i < mPlayerList.size(); ++i ){
		mPlayerList.at( i )->sendCommandsEnd();
	}
	
}

void Server::incomingConnection( int socketDescriptor )
{
	kDebug() << "socketDescriptor:" << socketDescriptor;
	
	Player* player = new Player( this );
	player->setSocketDescriptor( socketDescriptor );
	
	//If the player have name, then will be a player
	connect( player, SIGNAL( signalNewPlayer( Player* ) ), this, SLOT( slotNewPlayer( Player* ) ) );
}

void Server::setWhoStartGame( Knapsen::WhoStartGame whoStartGame )
{
	mGameSequence->setWhoStartGame( whoStartGame );
}

void Server::startGame()
{
	kDebug() << "Start game.";
	
	//Initialize deck
	mDeck = new Deck( mSizeOfDeck, mTypeOfCards );
	mDeck->buildDeck();
	
	//To all players
	for( int i = 0; i < mPlayerList.size(); ++i ){
		
		//Set opponent
		for( int j = 0; j < mPlayerList.size(); ++j ){
			if( j != i ){
				mPlayerList.at( i )->setOpponent( mPlayerList.at( j ) );
			}
		}
		
		//Send the opponent player's name
		mPlayerList.at( i )->sendOpponentName();
		
		//Send the type of cards
		mPlayerList.at( i )->sendTypeOfCard( mTypeOfCards );
		//Send the size of deck
		mPlayerList.at( i )->sendSizeOfDeck( mSizeOfDeck );
		//Send number of cards in hand
		mPlayerList.at( i )->sendNumberOfCardsInHand( mNumberOfCardsInHand );
		
		//Initialize table
		mPlayerList.at( i )->sendInitializeTable();
	}
	
	//Send cards to players
	for( int i = 0; i < mNumberOfCardsInHand; ++i ){
		
		for( int j = 0; j < mPlayerList.size(); ++j ){
			mPlayerList.at( j )->sendNewCard( mDeck->getCard() );
		}
		
		//Get trump card
		if( i == 2 ){
			mTrumpCard = mDeck->getCard();
			
			for( int j = 0; j < mPlayerList.size(); ++j ){
				mPlayerList.at( j )->sendNewTrumpCard( mTrumpCard );
			}
			
		}
		
	}
	
	Player* currentPlayer = mGameSequence->getCurrentPlayer();
	
	if( currentPlayer->haveRegularMarriages() ){
		kDebug() << currentPlayer->getName() << "have regular marriages.";
		currentPlayer->sendTwentyButtonVisible();
	}
	
	if( currentPlayer->haveTrumpMarriages() ){
		kDebug() << currentPlayer->getName() << "have trump marriages.";
		currentPlayer->sendFortyButtonVisible();
	}
	
	if( currentPlayer->canChangeTrumpCard() ){
		kDebug() << currentPlayer->getName() << "can change trump card.";
		currentPlayer->sendSelectableTrumpCard();
	}
	
	currentPlayer->sendCloseButtonVisible();
	
	currentPlayer->sendSelectableAllCards();
	
	//And finally send start the game
	for( int i = 0; i < mPlayerList.size(); ++i ){
		mPlayerList.at( i )->sendStartGame();
		mPlayerList.at( i )->sendCommandsEnd();
	}
	
	
}
