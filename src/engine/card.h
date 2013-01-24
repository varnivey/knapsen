#ifndef CARD_H
#define CARD_H

#include <QtCore/QString>
//#include "enums.h"

class Card
{
public:
    enum Suit{ Heart, Diamond, Spade, Club };
    enum Type{ Ace, Ten, King, Queen, Jack, Nine };
    
private:
    int mValue;
    
    bool mSelectable;
    
public:
    static const int CARD_NUMBERS_WITHOUT_9 = 20;
    static const int CARD_NUMBERS_WITH_9 = 24;

    Card( int );
    
    int getValue() const{ return mValue; }

    void setSelectable( bool selectable ){ mSelectable = selectable; }
    
    bool isSelectable() const{ return mSelectable; }
    
    QString getCardText(/* Knapsen::TypeOfCards */) const;
    
    int getCardPoint() const;
    
    Suit getSuit() const;
    Type getType() const;

    //bool operator==( const Card * );
};

#endif //CARD_H
