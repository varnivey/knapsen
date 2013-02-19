#ifndef CARDS_GROUP_H
#define CARDS_GROUP_H

#include <QtGui/QGraphicsObject>

class SvgCard;
class Animation;
class Card;

class CardsGroup : public QGraphicsObject
{
    Q_OBJECT
    
    //QSizeF mCardSize;
    
    Animation* mAnimation;
    
    int mHighlightCardId;
    
    QList< SvgCard* > mCards;
    
    //
    QPointF getNewCardPos();
    //
    
    void highlightCard( int );
    void removeHighlight( int );
    
public:
    CardsGroup(/* QSizeF */);
    ~CardsGroup();
    
    QRectF boundingRect() const;
    void paint( QPainter*, const QStyleOptionGraphicsItem*, QWidget* );
    
    Animation* getAnimation(){ return mAnimation; }
    
    //
    void addNewCard( SvgCard*, QGraphicsItem* );
    //
    
private slots:
    void slotCardAnimatedEnd();
    
    void slotMouseEnter( SvgCard* );
    void slotMouseLeave( SvgCard* );
    
    void slotCardClicked( SvgCard* );
    
public slots:
    void slotSelectableChanged( int, bool );
    
signals:
    //
    void signalSizeChanged();
    //
    //
    void signalCardArrived();
    //
    void signalSelectedCard( int, SvgCard* );
    
};

#endif //CARDS_GROUP_H