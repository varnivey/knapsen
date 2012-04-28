#ifndef CLIENT_H
#define CLIENT_H

#include "player.h"
#include "enums.h"
	
class Client : public Player
{
	Q_OBJECT
	
	QString mOpponentName;
	
	Knapsen::TypeOfCards mTypeOfCards;
	
	int mSizeOfDeck;
	
	virtual void newCommand( QString );
	
private slots:
	void slotConnected();
	
	//void slotNewCentralCard( int, Card );
	void slotCentralCardChanged( int, Card );
	
public:
    Client( QObject* parent = 0 );
	
public slots:
	void slotSelectedCardId( int );
	void slotTwentyButtonClicked();
	void slotFortyButtonClicked();
	
signals:
	void signalInitialize( QString, QString, Knapsen::TypeOfCards, int );
	
	void signalNewOpponentCardId( int );
	
	void signalNewPlayerCard( int, QString );
	
	void signalNewTrumpCard( QString );
	
	//void signalNewCentralCard( int, QString );
	void signalCentralCardChanged( int, QString );
	
	void signalOpponentSelectedCardId( int );
	
	void signalOpponentDisconnected();
	
	void signalOpponentTricksChanged( int );
	
	void signalPlayerTricksChanged( int );
	
	void signalDeckVisible( bool );
	
	void signalTrumpCardHide();
	
	
	void signalStartGame();
	
	//For bot
	void signalInAction();

};	
	
#endif //CLIENT_H