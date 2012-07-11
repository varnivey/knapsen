#include <QtGui/QFont>
#include <QtGui/QFontMetrics>
#include <QtGui/QPainter>
#include <KDE/KLocalizedString>
#include <KDE/KDebug>
#include "scoretable.h"

const QString TRICKS_TEXT = i18n( "Tricks:" );
const QString SCORE_TEXT = i18n( "Score:" );

ScoreTable::ScoreTable()
{
	tricksItem = new QGraphicsTextItem( this );
	scoreItem = new QGraphicsTextItem( this );
	
	setTricks( 0 );
	scoreItem->setPlainText( SCORE_TEXT+" "+QString::number( 0 ) );
	
	tricksItem->setPos( 0, 0 );
	scoreItem->setPos( 0, tricksItem->boundingRect().height() );
}

QRectF ScoreTable::boundingRect() const
{
	int width = 0;
	int height = 0;
	
	if( tricksItem->boundingRect().width() > scoreItem->boundingRect().width() ){
		width = tricksItem->boundingRect().width();
	}else{
		width = scoreItem->boundingRect().width();
	}
	
	height = tricksItem->boundingRect().height() + scoreItem->boundingRect().height();
	
	QRectF ret( QPointF( 0, 0 ), QSize( width, height ) );
	
	return ret;
}

void ScoreTable::setTricks( int tricks )
{
	tricksItem->setPlainText( TRICKS_TEXT+" "+QString::number( tricks ) );
	
	update();
}

void ScoreTable::setScores( int scores )
{
	scoreItem->setPlainText( SCORE_TEXT+" "+QString::number( scores ) );

	update();
}

void ScoreTable::paint(QPainter * painter, const QStyleOptionGraphicsItem *, QWidget * )
{
	painter->fillRect( boundingRect(), Qt::white );
}
