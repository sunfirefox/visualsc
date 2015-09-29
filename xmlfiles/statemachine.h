// This header file was generated using VisualSC's Export Code Function

// Visual Statechart is an open source project. Visit us at https://github.com/daviddrell/visualsc
#ifndef STATEMACHINE_H
#define STATEMACHINE_H

#include <QObject>
#include <QStateMachine>
#include <QFinalState>

class StateMachine : public QObject
{
    Q_OBJECT

    //////////////////
    //
    //    Public interface
    //
    //    Connect to public slots to drive events into
    //    the state machine
    //
    //    Connect to the public signals to do work
    //    and monitor state changes
    //
    //////////////////

public:
    StateMachine(QObject*parent=NULL);
    ~StateMachine();

    //
    //    INPUT EVENT SLOTS
    //
    //    Call these slots to signal event inputs to the machine
    //


public slots:
    //////// State Machine: _stateMachine ////////
    void Event_startMachine___stateMachine();


signals:
    //
    //    OUTPUT ACTION SIGNALS
    //
    //    Connect to these signals to a slot corresponding to a entryAction or exitAction
    //
    //////// State Machine: _stateMachine ////////
    void Action___wowDoCoolStuff();
    void Action___goodPeformance();
    void Action___and();
    void Action___handling();
    void Action___testingLeadingSpaces();
    void Action___testingTrailingSpaces();


    //
    //    STATE CHANGE MONITOR SIGNALS
    //
    //    Connect to these signals to monitor state transitions
    //
    //////// State Machine: _stateMachine ////////
    void Signal_StateReady___stateMachine();
    void Signal_StateEntry___stateMachine_1();
    void Signal_StateExit___stateMachine_1();
    void Signal_StateEntry___stateMachine_2();
    void Signal_StateExit___stateMachine_2();
    void Signal_StateEntry___lkasefJlsaefJalsefJasdlfjae4fIafEjStateMachine();
    void Signal_StateExit___lkasefJlsaefJalsefJasdlfjae4fIafEjStateMachine();


    //////////////////
    //
    //    Private below here
    //
    //////////////////


    /*
     *  A consequence of OOB encapsulation is that this class does know about its users, and so cannot listen
     * to their signals. Therefore this class exposes public slots that users can call. These slots
     * are then translated into private signals that the private state machine can listen to.
     *
     * For outbound signals, I choose to keep the statemachine objects private so that means I need
     * to translate private state machine signals into public signals.
     *
     * This all leads to a lot of confusion and possible name collisions. Therefore its better for this
     * file to be autogenerated from the visual state chart editor.
     *
     */


private:


private slots:
    // The Entry/Exit Slots that belong to QStates
    //////// State Machine: _stateMachine ////////
    void Slot_StateEntry___stateMachine_1();
    void Slot_StateExit___stateMachine_1();
    void Slot_StateEntry___stateMachine_2();
    void Slot_StateExit___stateMachine_2();
    void Slot_StateEntry___lkasefJlsaefJalsefJasdlfjae4fIafEjStateMachine();
    void Slot_StateExit___lkasefJlsaefJalsefJasdlfjae4fIafEjStateMachine();


signals:
    // A Transition/Event slot's corresponding signal emitted in the slot
    //////// State Machine: _stateMachine ////////


private:
    //////// State Machine: _stateMachine ////////
    QStateMachine*    _stateMachine;
    QState*    _stateMachine_1;
    QState*    _stateMachine_2;
    QState*    _lkasefJlsaefJalsefJasdlfjae4fIafEjStateMachine;


};

#endif // STATEMACHINE_H
