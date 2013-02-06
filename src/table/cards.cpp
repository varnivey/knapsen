#include <KDE/KDebug>
//
#include <QtCore/QDebug>
//
#include <QtSvg/QSvgRenderer>
//
#include <QtCore/QSizeF>
#include <QtCore/QPointF>
//
//
#include <QtCore/QTimer>
//
#include "engine/card.h"
#include "table/svgcard.h"
#include "table/cards.h"

//
#include "QtCore/QTimeLine"
#include "QtGui/QGraphicsItemAnimation"
//

const double HIGHLIGHT_Y_PERCENT = 20;

const int INVALID_HIGHLIGHT_CARD_ID = -1;

/*Cards::Cards( QSvgRenderer* renderer, double scale ) :
    mRenderer( renderer ),
    mScale( scale ),
    mHighlightCardId( INVALID_HIGHLIGHT_CARD_ID ),
    mCardIsComing( false )
{
    mShowOpponentCardsId = 0;
}*/

Cards::Cards( QSizeF cardSize ) : 
    QGraphicsObject(),
    mCardSize( cardSize ),
    mHighlightCardId( INVALID_HIGHLIGHT_CARD_ID ),
    mCardIsComing( false )
{
    mShowOpponentCardsId = 0;
}


QRectF Cards::boundingRect() const
{
    int width = 0;
    int height = 0;
    
    if( mCards.size() > 0 ){
        height = mCards.at( 0 )->getSizeF().height();
    
        //
        //width = ( mCards.size() + 2 ) * ( mCards.at( 0 )->getSizeF().width() / 2 );
    }
    
    for( int i = 0; i < mCards.size(); ++i ){
        //width += mCards.at( i )->getSizeF().width() / 2;
    
        if( i != mCards.size()-1 ){
            width += mCards.at( i )->getSizeF().width() / 2;  
        }else{
            width += mCards.at( i )->getSizeF().width();
        }
    }
    
    //
    if( mCards.size() > 0 && mHighlightCardId != INVALID_HIGHLIGHT_CARD_ID && mHighlightCardId != ( mCards.size() - 1 )  ){
        width += mCards.at( 0 )->getSizeF().width() / 2;
    }
    //
    
    if( mCardIsComing ){
        if( mCards.size() == 0 ){
            width += mCardSize.width();
        }else{
            width += mCardSize.width() / 2;
        }
    }
    
    return QRectF( QPointF(0,0), QSizeF( width, height ) );
}

void Cards::paint( QPainter* /*painter*/, const QStyleOptionGraphicsItem* /*option*/, QWidget* /*widget*/ )
{}

void Cards::slotRemoveAllCards()
{
    while( mCards.size() != 0 ){
        delete mCards.takeAt( 0 );
    }
    
    mHighlightCardId = INVALID_HIGHLIGHT_CARD_ID;
    
    emit signalSizeChanged();
}

void Cards::slotMouseEnter( SvgCard* svgCard )
{
    //int id = getCardId( svgCard );
    int id = mCards.indexOf( svgCard );
    
    kDebug() /*<< "slotMouseEnter(): "*/ << id;
    
    /*if( id != mHighlightCardId ){
        //Highlight card
        mCards.at( id )->setPos( mCards.at( id )->pos().x(),
                                 mCards.at( id )->pos().y() - mCards.at( id )->getSizeF().height() * ( HIGHLIGHT_Y_PERCENT / 100 ) );
        
        
        //Next cards move right
        for( int i = id + 1; i < mCards.size(); ++i ){
            mCards.at( i )->setPos( mCards.at( i )->pos().x() + ( mCards.at( i )->getSizeF().width() ) / 2, 
                                    mCards.at( i )->pos().y() );
        }
        
        mHighlightCardId = id;
        
        //
        emit signalSizeChanged();
    }*/
    
    if( mHighlightCardId != INVALID_HIGHLIGHT_CARD_ID ){
        removeHighlight( mHighlightCardId );
        
        mHighlightCardId = INVALID_HIGHLIGHT_CARD_ID;
    }
    
    highlightCard( id );
    mHighlightCardId = id;
    
    emit signalSizeChanged();
}

void Cards::slotMouseLeave( SvgCard* svgCard )
{
    //int id = getCardId( svgCard );
    int id = mCards.indexOf( svgCard );
    
    kDebug() /*<< "slotMouseLeave(): "*/ << id;
    
    /*if( id == mHighlightCardId ){
        //Highlight back
        mCards.at( id )->setPos( mCards.at( id )->pos().x(),
                                 mCards.at( id )->pos().y() + mCards.at( id )->getSizeF().height() * ( HIGHLIGHT_Y_PERCENT / 100 ) );
        
        
        //Next cards move back
        for( int i = id + 1; i < mCards.size(); ++i ){
            mCards.at( i )->setPos( mCards.at( i )->pos().x() - ( mCards.at( i )->getSizeF().width() ) / 2, 
                                    mCards.at( i )->pos().y() );
        }
    
        mHighlightCardId = INVALID_HIGHLIGHT_CARD_ID;
        
        //
        emit signalSizeChanged();
    }*/
    
    if( id == mHighlightCardId ){
        removeHighlight( mHighlightCardId );
        mHighlightCardId = INVALID_HIGHLIGHT_CARD_ID;
        
        emit signalSizeChanged();
    }
}

//void Hand::slotCardClicked( int id )
void Cards::slotCardClicked( SvgCard* svgCard )
{
    //int id = getCardId( svgCard );
    int id = mCards.indexOf( svgCard );
    
    emit signalSelectedCardId( id );
    
    delete mCards.takeAt( id );
    
    for( int i = id; i < mCards.size(); ++i ){
        mCards.at( i )->setPos( mCards.at( i )->x() - mCards.at( i )->getSizeF().width(), mCards.at( i )->y() );
    }
    
    mHighlightCardId = INVALID_HIGHLIGHT_CARD_ID;
    
    emit signalSizeChanged();
}

/*void Cards::newCard( QString cardText )
{
    kDebug() << cardText;
    
    //c
    //cardSvgItem->setElementId( cardText );
    SvgCard *cardSvgItem = new SvgCard( mRenderer, mScale, cardText, this );
    
    double x = 0;
    for( int i = 0; i < mCards.size(); ++i ){
        x+=mCards.at( i )->getSizeF().width()/2;
    }
    
    cardSvgItem->setPos( x, 0 );
    
    mCards.append( cardSvgItem );
    
    connect( cardSvgItem, SIGNAL( signalMouseEnter( SvgCard* ) ),    this, SLOT( slotMouseEnter( SvgCard* ) ) );
    connect( cardSvgItem, SIGNAL( signalMouseLeave( SvgCard* ) ),    this, SLOT( slotMouseLeave( SvgCard* ) ) );
    connect( cardSvgItem, SIGNAL( signalClick( SvgCard* ) ),         this, SLOT( slotCardClicked( SvgCard* ) ) );
    
    emit signalSizeChanged();*/
    
    /*cardSvgItem->stackBefore( mDeck );
    
    connect( cardSvgItem, SIGNAL( signalMouseEnter( SvgCard* ) ),    this, SLOT( slotMouseEnter( SvgCard* ) ) );
    connect( cardSvgItem, SIGNAL( signalMouseLeave( SvgCard* ) ),    this, SLOT( slotMouseLeave( SvgCard* ) ) );
    connect( cardSvgItem, SIGNAL( signalClick( SvgCard* ) ),         this, SLOT( slotCardClicked( SvgCard* ) ) );
    
    double x = 0;
    for( int i = 0; i < mCards.size(); ++i ){
        x+=mCards.at( i )->getSizeF().width()/2;
    }
    
    mCards.append( cardSvgItem );
    
    emit signalSizeChanged();
    
    //cardSvgItem->setPos( mapFromScene( 0, 0 ) + mCoordFromCard );
    cardSvgItem->setPos( mapFromScene( 0, 0 ) + mDeck->pos() );
    
    QTimeLine* timeLine = new QTimeLine( 1000 );
    timeLine->setFrameRange( 0, 100 );
    
    connect( timeLine, SIGNAL( finished() ), this, SLOT( slotTimeLineFinished() ) );
    
    QGraphicsItemAnimation* animation = new QGraphicsItemAnimation;
    animation->setItem( cardSvgItem );
    animation->setTimeLine( timeLine );
    
    //animation->setPosAt( 0.0, mapFromScene( 0, 0 ) + mCoordFromCard );
    
    animation->setPosAt( 1.0, QPointF( x, 0 ) );
    
    timeLine->start();
}*/

void Cards::highlightCard( int id )
{
    mCards.at( id )->setPos( mCards.at( id )->pos().x(),
                             mCards.at( id )->pos().y() - mCards.at( id )->getSizeF().height() * ( HIGHLIGHT_Y_PERCENT / 100 ) );
            
    //Next cards move right
    for( int i = id + 1; i < mCards.size(); ++i ){
        mCards.at( i )->setPos( mCards.at( i )->pos().x() + ( mCards.at( i )->getSizeF().width() ) / 2, 
                                mCards.at( i )->pos().y() );
    } 
}

void Cards::removeHighlight( int id )
{
    //Remove highlightCard
    mCards.at( id )->setPos( mCards.at( id )->pos().x(),
                             mCards.at( id )->pos().y() + mCards.at( id )->getSizeF().height() * ( HIGHLIGHT_Y_PERCENT / 100 ) );
        
    //Next cards move back
    for( int i = id + 1; i < mCards.size(); ++i ){
        mCards.at( i )->setPos( mCards.at( i )->pos().x() - ( mCards.at( i )->getSizeF().width() ) / 2, 
                                mCards.at( i )->pos().y() );
    }
}

/*void Cards::slotNewCard( const Card* card )
{
    newCard( card->getCardText() );
}*/

/*void Cards::slotNewCard( SvgCard* svgCard )
{
    kDebug() << svgCard->elementId() << "arrived.";
    
    svgCard->setParentItem( this );
    
    double x = 0;
    for( int i = 0; i < mCards.size(); ++i ){
        x+=mCards.at( i )->getSizeF().width()/2;
    }
    
    svgCard->setPos( x, 0 );
    
    mCards.append( svgCard );
    
    emit signalSizeChanged();
    
    //emit signalCardArriwed();
}*/

QPointF Cards::getNewCardPos()
{
    QPointF pos( 0.0, 0.0 );
    
    if( mCards.size() != 0 ){
        pos.setX( mCards.size() * ( mCards.at( 0 )->getSizeF().width() / 2 ) );
    }
    
    return pos;
}

void Cards::cardWillArrive()
{
    mCardIsComing = true;
}

void Cards::addNewCard( SvgCard* svgCard )
{
    svgCard->setParentItem( this );
    
    svgCard->setPos( getNewCardPos() );
    
    mCards.append( svgCard );
    
    mCardIsComing = false;
}

void Cards::slotNewCardArrived()
{
    kDebug() << "Arrived.";
}

void Cards::slotChangeCard( int id, const Card* card )
{
    mCards.at( id )->setElementId( card->getCardText() );
}

void Cards::slotSelectableChanged( int id, bool enabled )
{
    kDebug() << id << enabled;
    
    mCards.at( id )->setSelectable( enabled );
}

void Cards::slotRemoveCard( int )
{
    delete mCards.takeLast();
    
    emit signalSizeChanged();
}

void Cards::slotShowCards( int id1, Card card1, int id2, Card card2 )
{
    SvgCard* card;
    
    for( int i = id1 + 1; i < mCards.size(); ++i ){
        card = mCards.at( i );
        card->setPos( card->x() + card->getSizeF().width() / 2, card->y() );
    }
    
    mCards.at( id1 )->setPos( mCards.at( id1 )->x(),  
                              mCards.at( id1 )->y() + ( mCards.at( id1 )->getSizeF().height() * ( HIGHLIGHT_Y_PERCENT / 100 ) ) );
    
    mCards.at( id1 )->setElementId( card1.getCardText() );
    
    for( int i = id2 + 1; i < mCards.size(); ++i ){
        card = mCards.at( i );
        card->setPos( card->x() + card->getSizeF().width() / 2, card->y() );
    }
    
    mCards.at( id2 )->setPos( mCards.at( id2 )->x(),  
                              mCards.at( id2 )->y() + ( mCards.at( id2 )->getSizeF().height() * ( HIGHLIGHT_Y_PERCENT / 100 ) ) );
    
    mCards.at( id2 )->setElementId( card2.getCardText() );
    
    emit signalSizeChanged();
    
    mShowOpponentCardsId = new QPair< int, int>( id1, id2 );
    
    QTimer::singleShot( 1000, this, SLOT( slotHideCards() ) );
}

void Cards::slotHideCards()
{
    kDebug() << "Hide";
    
    SvgCard* card;
    int id1 = mShowOpponentCardsId->first;
    int id2 = mShowOpponentCardsId->second;
    
    delete mShowOpponentCardsId;
    mShowOpponentCardsId = 0;
    
    mCards.at( id1 )->setPos( mCards.at( id1 )->x(),  
                              mCards.at( id1 )->y() - ( mCards.at( id1 )->getSizeF().height() * ( HIGHLIGHT_Y_PERCENT / 100 ) ) );
    
    mCards.at( id1 )->setElementId( "back" );
    
    for( int i = id1 + 1; i < mCards.size(); ++i ){
        card = mCards.at( i );
        card->setPos( card->x() - card->getSizeF().width() / 2, card->y() );
    }
    
    
    mCards.at( id2 )->setPos( mCards.at( id2 )->x(),  
                              mCards.at( id2 )->y() - ( mCards.at( id2 )->getSizeF().height() * ( HIGHLIGHT_Y_PERCENT / 100 ) ) );
    
    mCards.at( id2 )->setElementId( "back" );
    
    for( int i = id2 + 1; i < mCards.size(); ++i ){
        card = mCards.at( i );
        card->setPos( card->x() - card->getSizeF().width() / 2, card->y() );
    }
    
    emit signalHiddenShowedCard();
}