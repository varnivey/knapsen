
function createCard( parent, source, startElementId, endElementId, scale, x, y ){
    var component = Qt.createComponent("Card.qml");
    var card = component.createObject( parent, {
        "x": x, 
        "y": y,
        "source": source,
        "elementId": startElementId,
        "newElementId": endElementId,
        "scale": scale
    });
    
    return card;
}

function createSingleShot( parent, interval ){
    var component = Qt.createComponent( "Singleshot.qml" );
    var singleShot = component.createObject( parent, {
        "interval": interval,
        "running": false
    });
    
    return singleShot;
}