#include "transitiongraphic.h"
#include "scstate.h"
#include <QDebug>

TransitionGraphic::TransitionGraphic(StateBoxGraphic *parentGraphic, StateBoxGraphic *targetGraphic, SCTransition * t, KeyController * keys, MouseController* mouse) :
    QGraphicsObject(NULL),
    _transitionDM(t),
    _lineSegments(),
    _elbows(),
    _targetStateGraphic(targetGraphic),
    _keyController(keys),
    _mouseController(mouse)
{
    this->setFlag(QGraphicsItem::ItemIsMovable, false);
    this->setParentItem(parentGraphic);
    //this->acceptHoverEvents();
    //this->setParent(targetGraphic);

    TransitionAttributes::TransitionPathAttribute * p =
            dynamic_cast<TransitionAttributes::TransitionPathAttribute *> (  t->attributes.value("path"));

    QList<QPointF> pointList = p->asQPointFList();
    qDebug() << "Printing Point List of size: " <<pointList.count();
    for(int i = 0; i < pointList.count();i++)
        qDebug() << pointList.at(i);


    if(1)   // load the point list and create the elbows and line segments
    {
        // add every point as an elbow
        for(int i = 0; i < pointList.count(); i++)
        {
            ElbowGrabber* elb;
            elb = new ElbowGrabber(this, pointList.at(i));
            elb->installSceneEventFilter(this);                 // transition graphic will handle elbow events
            elb->setAcceptHoverEvents(true);                    // allow the elbow to be hovered

            _elbows.append(elb);
        }


        // create a line segment in between every elbow
        for(int i = 0; i < _elbows.count()-1; i++)
        {
            ElbowGrabber* elbOne = _elbows.at(i);
            ElbowGrabber* elbTwo = _elbows.at(i+1);

            //SelectableLineSegmentGraphic * segment = new SelectableLineSegmentGraphic(elbOne, elbTwo,t, this,_keyController);
            LineSegmentGraphic* segment = new LineSegmentGraphic(elbOne, elbTwo, this, _keyController);
            //segment->acceptHoverEvents();
            //segment->installSceneEventFilter(this);

           // connect(segment, SIGNAL(startEndMoved(QPointF)), parentGraphic, SLOT(handleTransitionLineStartMoved(QPointF)));
           // connect(segment, SIGNAL(updateModel()), this, SLOT(updateModel()));

            if(i==0){
                elbOne->setSegmentOne(NULL);
                elbOne->setSegmentTwo(segment);
            }
            else if(i==_elbows.count()-2){

                elbTwo->setSegmentOne(segment);
                elbTwo->setSegmentTwo(NULL);
            }
            else
            {
                elbOne->setSegmentTwo(segment);
                elbTwo->setSegmentOne(segment);
            }




            //elbOne->setVisible(false);
           // elbOne->setVisible(false);


            _lineSegments.append(segment);

        }

        // anchors

        _anchors[0] = _elbows.at(0);                    //anchors will be the saved as the first and last points following a path from the start to end states
        _anchors[1] = _elbows.at(_elbows.count()-1);
        _lineSegments.at(_lineSegments.count()-1)->setTerminal(true); // set the last segment as the terminator
        _anchors[1]->setPaintStyle(ElbowGrabber::kArrowHead);   // set the end of the transition to an arrowhead

        _anchors[0]->setAnchor(true);
        _anchors[1]->setAnchor(true);
        connect(_anchors[0],SIGNAL(anchorMoved(QPointF)),parentGraphic,SLOT(handleTransitionLineStartMoved(QPointF)));  // state box will handle snapping the source elbow/anchor to its border instead of standard movement




    }
    else
    {/*
        if(pointList.count() < 2 && targetGraphic != NULL )
        {

            // this path is new, anchor each end to the source and target states
            // find the orientation of the parent and target graphics to determine which sides to anchor to.

            int sourceSide=0;
            int targetSide=0;

            getClosestSides( & sourceSide, & targetSide);
            QPointF sourceAnchor = this->parentItem()->mapFromScene(this->parentItemAsStateBoxGraphic()->getSideCenterPointInSceneCoord(sourceSide));
            QPointF targetAnchor = _targetStateGraphic->mapFromScene( _targetStateGraphic->getSideCenterPointInSceneCoord(targetSide));
            _anchors[0]->setPos(sourceAnchor);
            _anchors[1]->setPos(targetAnchor);

            LineSegmentGraphic * segment  = new LineSegmentGraphic(sourceAnchor,sourceAnchor, targetAnchor, t,this,_keyController);
            segment->setParentItem(this);
            segment->setAcceptHoverEvents(true);
            //segment->acceptHoverEvents();
            //segment->installSceneEventFilter(this);

            _lineSegments.append(segment);
            connect(segment, SIGNAL(startEndMoved(QPointF)), parentGraphic, SLOT(handleTransitionLineStartMoved(QPointF))); // if the start point moved, then update
            connect(segment, SIGNAL(updateModel()), this, SLOT(updateModel()));     // update the transition's datamodel if the segment is changed

            segment->setTerminator(true);

        }
        else if ( pointList.count() == 2 )      // straight line from start to end state
        {

            LineSegmentGraphic * segment   = new LineSegmentGraphic(pointList[0], pointList[0], pointList[1], t, this,_keyController);
            segment->setParentItem(this);
           // segment->setParent(parentGraphic);
            _lineSegments.append(segment);

            // connect the parent state-graphic's slots to the new transition graphic's signals

            connect(segment, SIGNAL(startEndMoved(QPointF)), parentGraphic, SLOT(handleTransitionLineStartMoved(QPointF)));
            connect(segment, SIGNAL(updateModel()), this, SLOT(updateModel()));

            segment->setTerminator(true);
        }
        else
        {

            LineSegmentGraphic * lastSegment=NULL;

            for (int i = 0 ; i < pointList.count() - 1 ; i ++)
            {
               LineSegmentGraphic * segment   = new LineSegmentGraphic( pointList[i+0], pointList[i+0], pointList[i+1], t, this,_keyController);
                segment->setParentItem(this);
                _lineSegments.append(segment);
                connect ( segment, SIGNAL(startEndMoved(QPointF)), parentGraphic, SLOT(handleTransitionLineStartMoved(QPointF)));
                connect(segment, SIGNAL(updateModel()), this, SLOT(updateModel()));

                lastSegment = segment;
            }

            lastSegment->setTerminator(true);

        }
        */
    }

     //printInfo();
}



/**
 * @brief TransitionGraphic::setGrabbersVisible
 * @param visible
 * calls setVisible for all elbows in the transition graphic to the bool value
 */
void TransitionGraphic::setGrabbersVisible(bool visible)
{
    //TODO move corner grabbers from line segment to higher level transition graphic
    for(int i = 0; i < _elbows.count(); i++)
        _elbows.at(i)->setVisible(visible);
}

/**
 * @brief TransitionGraphic::parentItemAsStateBoxGraphic
 * @return returns the parent QGraphicsItem of the transition graphic as a StateBoxGraphic
 */
StateBoxGraphic* TransitionGraphic::parentItemAsStateBoxGraphic()
{
    return static_cast<StateBoxGraphic*>(this->parentItem());
}

void TransitionGraphic::updateElbow(QPointF newPos, ElbowGrabber *elbow)
{
    //elbow->setPos(newPos);

    // check if this elbow is an anchor
    if(elbow->isAnchor())
    {
       // qDebug() << "selected elbow is an anchor";
        // this elbow is an anchor, so we need to lock it to its parent state box graphic
        if(elbow==_elbows[0])
        {
           // qDebug() << "selected elbow is the source elbow\n";
            //QPointF mouse(x,y);
            //emit elbow->anchorMoved(mouse);

            emit elbow->anchorMoved(newPos);
        }
        else
        {
            elbow->setPos(newPos);
        }
    }
    // change the elbow's coordinates to the specified x and y
    else
        elbow->setPos(newPos);
}

/**
 * @brief TransitionGraphic::sceneEventFilter
 * @param watched   object that caused the event
 * @param event     action (mouse)
 * @return
 *
 * The scene event filter of transition graphic will update(change coordinates/redraw) the child elbow that was manipulated in the graphics view
 * and subsequently update the datamodel by calling TransitionGraphic::updateModel() when the mouse is released
 *
 */
bool TransitionGraphic::sceneEventFilter ( QGraphicsItem * watched, QEvent * event )
{
    // qDebug() << "transition graphic scene event filter";
    // qDebug() << " QEvent == " + QString::number(event->type()) << " " << count++;
    // qDebug() << "QGraphicsItem Scene Event " << watched->
    ElbowGrabber * elbow = dynamic_cast<ElbowGrabber *>(watched);
    if ( elbow == NULL) return false; // not expected to get here

    QGraphicsSceneMouseEvent * mevent = dynamic_cast<QGraphicsSceneMouseEvent*>(event);
    if ( mevent == NULL)
    {
        // this is not one of the mouse events we are interrested in
        return false;
    }


    switch (event->type() )
    {
        // if the mouse went down, record the x,y coords of the press, record it inside the corner object
    case QEvent::GraphicsSceneMousePress:
        {
        //qDebug() << "mouse press";
            elbow->setMouseState(ElbowGrabber::kMouseDown);

            QPointF scenePosition =  elbow->mapToScene(mevent->pos());
            elbow->mouseDownY = scenePosition.y();
            elbow->mouseDownX = scenePosition.x();

            //_cornerGrabbed = true;
            qDebug() << "Corner Position: " << elbow->mouseDownX<<" ," << elbow->mouseDownY;
        }
        break;


    case QEvent::GraphicsSceneMouseRelease:
        {
            //qDebug() << "mouse release";
            //_cornerGrabbed = false;

            elbow->setMouseState(ElbowGrabber::kMouseReleased);
            updateModel();                  // update the datamodel for this path
        }
        break;


    case QEvent::GraphicsSceneMouseMove:
        {
       //     qDebug() << "mouse moving";
            elbow->setMouseState(ElbowGrabber::kMouseMoving );
        }
        break;


    default:
        // we dont care about the rest of the events
        return false;
        break;
    }


    if ( elbow->getMouseState() == ElbowGrabber::kMouseMoving )
    {
        this->updateElbow(this->mapFromItem(elbow, mevent->pos()), elbow);
        //this->update();
    }

    return true;// true => do not send event to watched - we are finished with this event

}


void TransitionGraphic::createCustomPath(QPointF mouseLocation, ElbowGrabber* elbow)
{//TODO

    // update all line segments connected to this elbow

    qDebug() << "Elbow position: " << elbow->x() << ", "<< elbow->y();

}

TransitionGraphic::~TransitionGraphic()
{
    for(int i=0;i < _lineSegments.count(); i++)
    {
        LineSegmentGraphic * ls =_lineSegments.at(i);
        delete ls;
    }
    _lineSegments.clear();
    for(int i = 0; i < _elbows.count() ; i++)
    {
        ElbowGrabber* elb = _elbows.at(i);
        delete elb;
    }
    _elbows.clear();
}

/**
 * @brief setCurrentlyHoveredSegment
 */
void TransitionGraphic::setCurrentlyHoveredSegment(SelectableLineSegmentGraphic* seg)
{
    _hovered = seg;
}

void TransitionGraphic::clearCurrentlyHoveredSegment()
{
    _hovered = NULL;
}

bool TransitionGraphic::isCurrentlyHovered()
{
    return (_hovered!=NULL);
}

SelectableLineSegmentGraphic* TransitionGraphic::getCurrentlyHoveredSegment()
{
    return _hovered;
}

/**
 * @brief TransitionGraphic::printInfo
 * prints the transition graphic's parent and target states
 */
void TransitionGraphic::printInfo(void)
{
    qDebug() << "Transition graphic from: " << this->parentItemAsStateBoxGraphic()->objectName() << " to: " << _targetStateGraphic->objectName();
}

/**
 * @brief TransitionGraphic::createNewElbow
 * create a new elbow on the currently hovered line segment, based on where the mouse is currently
 * a new line segment and elbow grabber object is created and added to the transition graphic
 */
void TransitionGraphic::createNewElbow()
{
    //_hovered->printInfo();
    ElbowGrabber* elb = new ElbowGrabber(this);
    elb->setPaintStyle(ElbowGrabber::kBox);
    elb->setPos(this->mapFromScene(_mouseController->getX(),_mouseController->getY()));    // maps the scene based mouse coordinates to create a new elbow at the cursor location
    //elb->setPos(this->_mouseController->getX(),_mouseController->getY());

    _elbows.append(elb);
}

/**
 * @brief TransitionGraphic::handleKeyPressEvent
 * @param key
 * connection made when a line segment of this transition graphic is hovered
 */
void TransitionGraphic::handleKeyPressEvent(int key)
{
   // qDebug() << "Transition Graphic Key Press: " << key;
    if(key==Qt::Key_N)
    {
        //qDebug() << "Creating New Elbow at pos: ";
        this->createNewElbow();
    }
}

/**
 * @brief TransitionGraphic::updateModel
 * Updates the transition graphic's data model if it exists
 * converts
 */
void TransitionGraphic::updateModel()
{

    if ( _transitionDM )                                 // check if the data model for this transition exists
    {
        // use elbows to create the path attribute
        QList<QPointF> path;
        for( int  i = 0; i < _elbows.count(); i++)
            path.append(_elbows.at(i)->pos());

        TransitionAttributes::TransitionPathAttribute * pathAttr = dynamic_cast<TransitionAttributes::TransitionPathAttribute *> (_transitionDM->attributes.value("path"));

        pathAttr->setValue(path);   // update the path values of _transitionDM, the data model object for this transition
    }
}



void TransitionGraphic::getClosestSides(int* sourceSide, int* targetSide)
{

    QPointF sourcePos = this->parentItemAsStateBoxGraphic()->getVisibleCenter();
    QPointF targetPos = _targetStateGraphic->getVisibleCenter();

    double deltaX = abs (sourcePos.x() - targetPos.x());
    double deltaY = abs (sourcePos.y() - targetPos.y());

    if ( deltaX > deltaY)
    {
        if ( sourcePos.x() > targetPos.x() )
        {
            *sourceSide = 3;
            *targetSide = 1;
        }
        else
        {
            *sourceSide = 1;
            *targetSide = 3;
        }
    }
    else
    {
        if ( sourcePos.y() > targetPos.y() )
        {
            *sourceSide = 0;
            *targetSide = 2;
        }
        else
        {
            *sourceSide = 2;
            *targetSide = 0;
        }
    }


}

void TransitionGraphic::paint (QPainter *, const QStyleOptionGraphicsItem *, QWidget *)
{

}

QRectF TransitionGraphic::boundingRect() const
{
    return QRectF();
}
