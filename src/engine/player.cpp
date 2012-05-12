#include <KDE/KDebug>
#include "player.h"

Player::Player( QObject* parent ): 
	QTcpSocket( parent ),
	mName( "" ),
	mTricks( 0 ),
	mScores( 0 ),
	mTrumpCard(),
	mTwentyButtonVisible( false ),
	mFortyButtonVisible( false ),
	mCloseButtonVisible( false )
{
	mOpponent = 0;
	
	connect( this, SIGNAL( readyRead() ), this, SLOT( slotReadyRead() ) );
	connect( this, SIGNAL( disconnected() ), this, SLOT( slotDisconnected() ) );
}

Player::~Player()
{
	kDebug() << getName() << "deleting.";
}

QString Player::getCommandPartOfCommand( QString text )
{
	if( text.indexOf('=') == -1 ){
		return text;
	}
	
	return text.mid( 0, text.indexOf( '=' )+1 ); //The '=' too.
}

QString Player::getValuePartOfCommand( QString text )
{
	return text.mid( text.indexOf( '=' )+1 );
}

void Player::setNumberOfCardsInHand( int size )
{
	for( int i = 0; i < size; ++i ){
		mCards.append( Card() );
	}
}

int Player::addNewCard( Card card )
{
	for( int i = 0; i < mCards.size(); ++i ){
		if( !mCards.at( i ).isValid() ){
			mCards[ i ] = card;
			//emit signalNewPlayerCard();
			return i;
		}
	}
	
	return -1;
}

int Player::changeTrumpCard()
{
	int ret = -1;
	
	for( int i = 0; i < mCards.size(); ++i ){
		
		if( mCards.at(i).isValid() && mCards.at(i).getCardSuit() == mTrumpCardSuit && mCards.at(i).getCardType() == Card::Jack ){
			kDebug() << i;
			
			Card tmpCard = mTrumpCard;
			mTrumpCard = mCards.at(i);
			removeCard( i );
			ret = addNewCard( tmpCard );
			
			break;
		}
		
	}
	
	mTrumpCard.setSelectable( false );

	return ret;
}

void Player::setSelectableAllCards( bool enabled )
{
	for( int i = 0; i < mCards.size(); ++i ){
		if( mCards.at( i ).isValid() ){
			mCards[ i ].setSelectable( enabled );
			emit signalPlayerCardSelectableChanged( i, enabled );
		}
	}
}

void Player::setSelectableCertainCards()
{
	//Set true, which cards equal type whit central card
	if( setSelectableCardsWhatEqualSuit( mCentralCards.at( 0 ).getCardSuit() ) == false ){
		//If have not cards which equal type whith central card, then set true, which equal type whith trump card 
		if( setSelectableCardsWhatEqualSuit( mTrumpCardSuit ) == false ){
			//If have not that cards, then equal all card
			setSelectableAllCards( true );
			
		}
		
	}
}

void Player::setSelectableRegularMarriagesCards()
{
	for( int i = 0; i < mCards.size(); ++i ){
		
		if( mCards.at( i ).isValid() && 
			mCards.at( i ).getCardSuit() != mTrumpCardSuit && 
			mCards.at( i ).getCardType() == Card::King
		){
			for( int j = 0; j < mCards.size(); ++j ){
				
				if( mCards.at( j ).isValid() &&
					mCards.at( j ).getCardSuit() == mCards.at( i ).getCardSuit() &&
					mCards.at( j ).getCardType() == Card::Queen
					
				){
					mCards[ i ].setSelectable( true );
					mCards[ j ].setSelectable( true );
					emit signalPlayerCardSelectableChanged( i, mCards.at( i ).isSelectable() );
					emit signalPlayerCardSelectableChanged( j, mCards.at( j ).isSelectable() );
				}
				
			}
		}
		
	}
}

void Player::setSelectableTrumpMarriagesCards()
{
	for( int i = 0; i < mCards.size(); ++i ){
		
		if( mCards.at( i ).isValid() && 
			mCards.at( i ).getCardSuit() == mTrumpCardSuit && 
			mCards.at( i ).getCardType() == Card::King
		){
			for( int j = 0; j < mCards.size(); ++j ){
				
				if( mCards.at( j ).isValid() &&
					mCards.at( j ).getCardSuit() == mCards.at( i ).getCardSuit() &&
					mCards.at( j ).getCardType() == Card::Queen
					
				){
					mCards[ i ].setSelectable( true );
					mCards[ j ].setSelectable( true );
					emit signalPlayerCardSelectableChanged( i, mCards.at( i ).isSelectable() );
					emit signalPlayerCardSelectableChanged( j, mCards.at( j ).isSelectable() );
				}
				
			}
		}
		
	}
}

int Player::getNumberOfCardsInHandNow()
{
	int ret = 0;
	
	for( int i = 0; i < getNumberOfCardsInHand(); ++i ){
		if( getCard( i ).isValid() ){
			++ret;
		}
	}
	
	return ret;
}

int Player::getPositionOfPairOfCard( Card card )
{
	for( int i = 0; i < mCards.size(); ++i ){
		if( mCards.at(i).isValid() && card.getCardSuit() == mCards.at(i).getCardSuit() &&
			( ( card.getCardType() == Card::King && mCards.at(i).getCardType() == Card::Queen ) ||
			  ( card.getCardType() == Card::Queen && mCards.at(i).getCardType() == Card::King ) )
		){
			return i;
		}
	}
	
	//Error
	return -1;
}

bool Player::haveRegularMarriages() const
{
	bool ret = false;
	
	for( int i = 0; i < mCards.size(); ++i ){
		if( mCards.at( i ).isValid() && mCards.at( i ).getCardSuit() != mTrumpCardSuit && mCards.at( i ).getCardType() == Card::King ){
			
			for( int j = 0; j < mCards.size(); ++j ){
				if( mCards.at(j).isValid() && mCards.at(j).getCardType() == Card::Queen && mCards.at(j).getCardSuit() == mCards.at(i).getCardSuit() ){
					ret = true;
				}
			}
			
		}
	}
	
	return ret;
}

bool Player::haveTrumpMarriages() const
{
	bool ret = false;
	
	for( int i = 0; i < mCards.size(); ++i ){
		if( mCards.at( i ).isValid() && mCards.at( i ).getCardSuit() == mTrumpCardSuit && mCards.at( i ).getCardType() == Card::King ){
			
			for( int j = 0; j < mCards.size(); ++j ){
				if( mCards.at(j).isValid() && mCards.at(j).getCardType() == Card::Queen && mCards.at(j).getCardSuit() == mCards.at(i).getCardSuit() ){
					ret = true;
				}
			}
			
		}
	}

	return ret;
}

bool Player::canChangeTrumpCard() const
{
	bool canChange = false;
	
	for( int i = 0; i < mCards.size(); ++i ){
		
		if( mCards.at(i).isValid() && mCards.at(i).getCardSuit() == mTrumpCardSuit && mCards.at(i).getCardType() == Card::Jack ){
			canChange = true;
		}
		
	}
	
	return canChange;
}

void Player::addNewCentralCard( Card card )
{
	mCentralCards.append( card );
	
	emit signalCentralCardChanged( mCentralCards.size()-1, card );
}

void Player::clearCentralCards()
{
	/*for( int i = 0; i < mCentralCards.size(); ++i ){
		mCentralCards[ i ] = Card();
		emit signalCentralCardChanged( i, mCentralCards.at( i ) );
	}*/
	for( int i = 0; i < mCentralCards.size(); ++i ){
		mCentralCards[ i ] = Card();
		emit signalCentralCardChanged( i, mCentralCards.at( i ) );
	}
	mCentralCards.clear();
}

void Player::setTwentyButtonVisible( bool twentyButtonVisible )
{
	mTwentyButtonVisible = twentyButtonVisible;
	emit signalTwentyButtonVisible( mTwentyButtonVisible );
}

void Player::setFortyButtonVisible( bool fortyButtonVisible )
{
	mFortyButtonVisible = fortyButtonVisible;
	emit signalFortyButtonVisible( mFortyButtonVisible );
}

void Player::twentyButtonClicked()
{
	setTwentyButtonVisible( false );
	
	if( mFortyButtonVisible ){
		setFortyButtonVisible( false );
	}
	
	setSelectableAllCards( false );
	setSelectableRegularMarriagesCards();
}

void Player::fortyButtonClicked()
{
	setFortyButtonVisible( false );
	
	if( mTwentyButtonVisible ){
		setTwentyButtonVisible( false );
	}
	
	setSelectableAllCards( false );
	setSelectableTrumpMarriagesCards();
}

void Player::newCommand( QString command )
{
	//kDebug() << getName() << "command part of command:" << getCommandPartOfCommand( command );
	
	if( getCommandPartOfCommand( command ) == NAME_COMMAND ){
		kDebug() << getName() << "name:" << getValuePartOfCommand( command );
		
		setName( getValuePartOfCommand( command ) );
		
		emit signalNewPlayer( this );
	}
	
	if( getCommandPartOfCommand( command ) == SELECTED_CARD_ID_COMMAND ){
		kDebug() << getName() << "Selected card id:" << getValuePartOfCommand( command );
		
		bool ok;
		int ret = getValuePartOfCommand( command ).toInt( &ok );
		
		if( ok ){
		
			if( mCards.at( ret ).isSelectable() ){
				setSelectableAllCards( false );
				
				if( isTwentyButtonVisible() ){
					setTwentyButtonVisible( false );
				}
				
				if( isFortyButtonVisible() ){
					setFortyButtonVisible( false );
				}
				
				if( isCloseButtonVisible() ){
					setCloseButtonVisible( false );
				}
				
				if( getTrumpCard().isSelectable() ){
					getTrumpCard().setSelectable( false );
				}
				
				Card selectedCard = getCard( ret );
				addNewCentralCard( selectedCard );
				removeCard( ret );
				
				emit signalSelectedCard( selectedCard, ret );
			}else{
				kDebug() << "ERROR!" << getName() << "selected" << ret << "card id, but this is not selectable!";
			}
			
		}else{
			kDebug() << "ERROR! Cannot convert selected card id value to int!";
		}
	}
	
	if( getCommandPartOfCommand( command ) == TWENTY_BUTTON_CLICKED_COMMAND ){
		kDebug() << getName() << "Twenty button clicked.";
		
		if( mTwentyButtonVisible ){
			twentyButtonClicked();
			//mOpponent->sendOpponentTwentyButtonClicked();
			emit signalTwentyButtonClicked();
		}else{
			kDebug() << getName() << "ERROR! Twenty button clicked, but twenty button is not visible!";
		}
	}
	
	if( getCommandPartOfCommand( command ) == FORTY_BUTTON_CLICKED_COMMAND ){
		kDebug() << getName() << "Forty button clicked.";
		
		if( mFortyButtonVisible ){
			fortyButtonClicked();
			//mOpponent->sendOpponentFortyButtonClicked();
			emit signalFortyButtonClicked();
		}else{
			kDebug() << getName() << "ERROR! Forty button clicked, but forty button is not visible!";
		}
	}
	
	if( getCommandPartOfCommand( command ) == CLOSE_BUTTON_CLICKED_COMMAND ){
		kDebug() << getName() << "Close button clicked.";
		
		if( isCloseButtonVisible() ){
			setCloseButtonVisible( false );
			
			if( getTrumpCard().isSelectable() ){
				getTrumpCard().setSelectable( false );
			}
			
			emit signalCloseButtonClicked();
		}else{
			kDebug() << getName() << "ERROR! Clicked to close button, but this is not visible!";
		}

	}
	
	if( getCommandPartOfCommand( command ) == CHANGE_TRUMP_CARD_COMMAND ){
		kDebug() << getName() << "Change trump card.";
		
		changeTrumpCard();
		emit signalChangedTrumpCard( mTrumpCard );
	}
	
}

bool Player::setSelectableCardsWhatEqualSuit( Card::CardSuit cardSuit )
{
	bool haveSelectableCard = false;
	
	for( int i = 0; i < getNumberOfCardsInHand(); ++i ){
		
		if( mCards.at( i ).isValid() && mCards.at( i ).getCardSuit() == cardSuit ){
			mCards[ i ].setSelectable( true );
			emit signalPlayerCardSelectableChanged( i, true );
			haveSelectableCard = true;
		}
		
	}
	
	return haveSelectableCard;
}

void Player::slotReadyRead()
{
	QString command = readLine();
	
	//kDebug() << getName() << "input command:" << command;
	
	//Sometimes more commands in one command, then do many pieces
	for( int lastI = 0, i = 1; i < command.size(); ++i ){
		
		if( command.at( i ) == ':' ){
			newCommand( command.mid( lastI, i-lastI ) );
			lastI = i;
		}
		
		if( i == command.size()-1 ){
			newCommand( command.mid( lastI ) );
		}
		
	}
}

void Player::sendTypeOfCard( Knapsen::TypeOfCards typeOfCards )
{
	if( typeOfCards == Knapsen::GermanSuits ){
		sendCommand( TYPE_OF_CARDS_COMMAND+TYPE_OF_CARDS_GERMAN_SUITS_VALUE );
	}else{ // typeOfCards == Knapsen::FrenchSuits
		sendCommand( TYPE_OF_CARDS_COMMAND+TYPE_OF_CARDS_FRENCH_SUITS_VALUE );
	}
}

void Player::sendNumberOfCardsInHand( int size )
{
	setNumberOfCardsInHand( size );
	sendCommand( NUMBER_OF_CARDS_IN_HAND_COMMAND+QString::number( size ) );
}

void Player::sendNewCard( const Card card )
{
	int id = addNewCard( card );
	
	sendCommand( NEW_PLAYER_CARD_COMMAND+QString::number( card.getValue() ) );
	sendNewOpponentCard( id );
}

void Player::sendNewTrumpCard( const Card trumpCard )
{
	setTrumpCard( trumpCard );
	sendCommand( NEW_TRUMP_CARD_COMMAND+QString::number( mTrumpCard.getValue() ) );
}

void Player::sendClearTrumpCard()
{
	clearTrumpCard();
	sendCommand( CLEAR_TRUMP_CARD_COMMAND );
}

void Player::sendSelectableAllCards()
{
	setSelectableAllCards( true );
	sendCommand( SELECTABLE_ALL_CARDS_COMMAND );
}

void Player::sendSelectableCertainCards()
{
	setSelectableCertainCards();
	sendCommand( SELECTABLE_CERTAIN_CARDS_COMMAND );
}

void Player::sendOpponentAddNewCentralCard( Card card )
{
	kDebug() << getName() << "Opponent add new central card.";
	addNewCentralCard( card );
	sendCommand( OPPONENT_ADD_NEW_CENTRAL_CARD_COMMAND+QString::number( card.getValue() ) );
}

void Player::addTricks( int tricks )
{
	setTricks( getTricks() + tricks );
	sendCommand( PLAYER_TRICKS_CHANGED_COMMAND+QString::number( getTricks() ) );
	mOpponent->sendCommand( OPPONENT_TRICKS_CHANGED_COMMAND+QString::number( getTricks() ) );
}

void Player::addScores( int scores )
{
	setScores( getScores() + scores );
	sendCommand( PLAYER_SCORES_CHANGED_COMMAND+QString::number( getScores() ) );
	mOpponent->sendCommand( OPPONENT_SCORES_CHANGED_COMMAND+QString::number( getScores() ) );
}

void Player::sendVisibleOpponentCards( int card1Pos, Card card1, int card2Pos, Card card2 )
{
	sendCommand( VISIBLE_OPPONENT_CARDS_COMMAND+QString::number( card1Pos )+","+QString::number( card1.getValue() )+","+
												QString::number( card2Pos )+","+QString::number( card2.getValue() )
	);
}

void Player::sendClearCentralCards()
{
	clearCentralCards();
	sendCommand( CLEAR_CENTRAL_CARDS_COMMAND );
}

void Player::sendTwentyButtonVisible()
{
	setTwentyButtonVisible( true );
	sendCommand( TWENTY_BUTTON_VISIBLE_COMMAND );
}

void Player::sendFortyButtonVisible()
{
	setFortyButtonVisible( true );
	sendCommand( FORTY_BUTTON_VISIBLE_COMMAND );
}

void Player::sendCloseButtonVisible()
{
	setCloseButtonVisible( true );
	sendCommand( CLOSE_BUTTON_VISIBLE_COMMAND );
}
