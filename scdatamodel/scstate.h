/*
    visualsc - Visual State Chart Editing for SCXML State Engines
    Copyright (C) 2011  David W. Drell (david@davidwdrell.net) and
    Contributing Authors:


    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef SCSTATE_H
#define SCSTATE_H
#include <QMetaType>
#include <QObject>
#include <QList>
#include "stateattributes.h"
#include "sctransition.h"
#include "sctransitionbranch.h"
class QString;
class QXmlStreamWriter;
class QPoint;
#include "textblock.h"
#include "scdatamodel_global.h"
#include "scitem.h"
//#include "stateboxgraphic.h"
#include <QUuid>

/**

  \class   SCState
  \ingroup DataModel
  \brief   The class represents a state in a state-chart Data-Model

  The state contains children elements which are sub-states (also instances of SCState) and
  state-transitions (SCTransition class). The state also contains a public attribute container as
  well as convenience methods to accessing the attributes. Since te attribute container is public,
  attributes can be accessed directly.

  The children states and transitions are contained as QObject children in the base QObject class
  (see addState or addTransition methods).

  */

#define DEFAULT_STATE_WIDTH  200
#define DEFAULT_STATE_HEIGHT 124

class SCDATAMODELSHARED_EXPORT  SCState : public SCItem
{
    Q_OBJECT
public:
    SCState(QObject *parent);
    SCState(const SCState& st);
    SCState(bool topState=false);
    ~SCState();


    void deleteSafely();
    void deleteAllSafely();

    QString getName();
   // void setGraphic(StateBoxGraphic*);



    //void prepareForDelete();
    virtual IAttributeContainer * getAttributes(); // reimplemented from SCItem base
    QString getUid();
    QString getUidFirstName();
    QString getFamilyId();
    bool doNotPrint(QString);
    int doNotPrintSize();
    enum TransitionTransitDirection { kTransitIn, kTransitOut, kDestination };
    void addTransitionReference(SCTransition*, TransitionTransitDirection );
    void addTransitionReference(SCTransitionBranch*, TransitionTransitDirection);

    void addTransistion(SCTransition*);
    void addState(SCState *);

    bool isParallel();

    bool isInitial();
    bool isFinal();

    SCState* getInitialState();
    SCState* getInitialStateMember();
    SCState* getFinalState();
    bool isStateMachine();

    void setInitial(QString);
    void setInitialState(SCState*);


    bool hasAnInitialState();

    SCState* parentAsSCState();

    void setAttributeValue(QString key, QString value);
    void addAttribute(QString key, QString value);
    bool removeAttribute(QString key);
    QString getAttributeValue(QString key);

    void setText(QString text);


    SizeAttribute* getSizeAttr();
    PositionAttribute* getPosAttr();
    StateName* getStateNameAttr();
    StateString* getStringAttr(QString attrName);

    int  getStateCount();

    //void setSize (QPointF& size);
    void setSize (QPointF& size);

    void setPosition (QPointF& size);

    SCState* parentAt(int levelUp);

    void removeTargetsTransitionIn();
    void removeSourcesTransitionOut();

    void reselectParent(SCState* target);

    QList<SCTransition*> getTransitionsIn();
    QList<SCTransition*> getTransitionsTerminating();

    QList<SCTransitionBranch*> getForkedTransitionsIn();
    QList<SCTransitionBranch*> getForkedTransitionsTerminating();

    void removeTransitionReferenceIn(SCTransition* trans);
    void removeTransitionReferenceOut(SCTransition* trans);
    void removeAllTransitionsIn();


    void setFont(QFont * font);
//    void setFont(QString fontName);
//    void setFontSize(int fontSize);

    /**
      * \fn getAllTransitions
      * \brief Get all transitions including decendant state's transitions
      *
      */
     void getAllTransitions( QList<SCTransition*> & list);

     /**
       * \fn getTransitions
       * \brief Get transitions of this state only
       *
       */

     void getTransitions(QList<SCTransition*> & transitionList);



    /**
      * \fn getStates
      * \brief Get direct children states, no decendants
      *
      */
    void getStates(QList<SCState *> & stateList);
    QList<SCState*> getStates();

    /**
      * \fn getAllStates
      * \brief Get all states, including decendants
      *
      */
    void getAllStates(QList<SCState *> & stateList);

    /**
      * \fn getParentState
      * \brief Returns the QObject parent, cast as a state, or NULL if parent is not a state or no parent
      *
      */
    SCState * getParentState();



    /**
      * \fn getLevel
      * \brief Top state machine is level 0. Each sub-state increments level.
      *
      */
    int getLevel();

    /**
      * \fn setLevel
      * \brief Top state machine is level 0. Each sub-state increments level.
      *  The level should only be set by the state chart constructor.
      */

    void setLevel(int level);


    SCState* getStateByUid(QString);


    /**
      * \fn getStateByName
      * \brief find descendant state by name
      *
      */
    SCState* getStateByName(QString name);

    /**
      * \fn writeSCVXML
      * \brief Write out the SCXML for the entire tree from this point downward
      * \param xmlWriter is the writer that writes the xml
      *
      */
    void writeSCVXML(QXmlStreamWriter & xmlWriter);

    /**
      * \fn getIDTextBlock
      * \brief Returns a pointer to the ID TextBlock
      * \param xmlWriter is the writer that writes the xml
      *
      */

    SCTextBlock* getIDTextBlock();

    StateAttributes         attributes;

signals:


     void aboutToBeDeleted(QObject*);
     void clicked(SCState*);
     void minimized(SCState*);
     void expanded(SCState*);
     void changed();
     void attributeChangedSignal(IAttribute*);

     void nameChangedInDataModel(SCState*,QString);
     void positionChangedInDataModel(SCState*, QPointF);
     void sizeChangedInDataModel(SCState*, QPointF);

     void nameChangedInFormView(SCState*,QString);
     void positionChangedInFormView(SCState*, QPointF);
     void sizeChangedInFormView(SCState*, QPointF);

     void changedParent(SCState*,SCState*);

     void bringToFront(SCState*);
     void sendToBack(SCState*);


public slots:
     void setStateName(QString n);
     void handleEntryActionChanged(QString);
     void handleExitActionChanged(QString);

private slots:   
     void handleTextBlockChanged();


private:


    void initCommon();

    //private data

    void resetLevels(SCState* parent);
    //StateBoxGraphic* _stateBoxGraphic;
    SCTextBlock * _IdTextBlock;
    QList<SCTransition*> _transitingTransitionsOut;
    QList<SCTransition*> _transitingTransitionsIn;
    QList<SCTransition*> _transitionsTerminatingHere;

    QList<SCTransitionBranch*> _transitingForkedTransitionsOut;
    QList<SCTransitionBranch*> _transitingForkedTransitionsIn;
    QList<SCTransitionBranch*> _transitionsForkedTerminatingHere;

    QList<SCTextBlock*> _textBlocks;

    int _level ; ///< distance from top state in terms of parent-child relationships
    QList<QString> DEFAULT_ATTRIBUTES_LIST;

    QList<QString> DO_NOT_DISPLAY_LIST; // attributes that can be ignored in the property table
    QHash<QString,int> DO_NOT_DISPLAY_HASH;

    SCState* _initialState;

};

Q_DECLARE_METATYPE (SCState*)


#endif // SCSTATE_H
