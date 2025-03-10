#ifndef CWSTATEMACHINE_H
#define CWSTATEMACHINE_H

#include "cwstate.h"
#include "cwtransition.h"

class CWStateMachine :  public QObject, public CWState
{
    Q_OBJECT
public:
//    CWStateMachine(QString theStateName,QString theEntryRelaySlot, QString theExitRelaySlot, QString theEntryRelaySignal,QString theExitRelaySignal,QString comments);
    CWStateMachine(SCState* state, QHash<SCState*, CWState*> *stateHash);
    ~CWStateMachine();
    void createSignalsAndSlots();

    // direct descendants of this state
    QList<CWState*>     _states;

    //SCState*    _myState;
    SCState*    _initialState;
    bool _parallel;

    bool isParallel();
    void setParallel(bool);

    QList<SCTransition*> _finishedTransitions;

    void createChildren();
    QList<CWState*> getStates();

    // this needs to be a pointer to a QHash
    QHash<SCState*, CWState*> * _stateHash;

    void createTransition(CWState* state);
    void createTransitions();


    QString _startEventName;
signals:
    void newTransition(CWTransition*);

};

#endif // CWSTATEMACHINE_H
