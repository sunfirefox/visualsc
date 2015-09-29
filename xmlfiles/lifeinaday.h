// This header file was generated using VisualSC's Export Code Function

// Visual Statechart is an open source project. Visit us at https://github.com/daviddrell/visualsc
#ifndef LIFEINADAY_H
#define LIFEINADAY_H

#include <QObject>
#include <QStateMachine>
#include <QFinalState>

class LifeInADay : public QObject
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
    LifeInADay(QObject*parent=NULL);
    ~LifeInADay();

    //
    //    INPUT EVENT SLOTS
    //
    //    Call these slots to signal event inputs to the machine
    //


public slots:
    //////// State Machine: _lifeInADay ////////
    void Event_startMachine___lifeInADay();
    void Event___goToBed();
    void Event___wakeUp();
    void Event___finishTasks();

    //////// State Machine: _sleep ////////
    void Event___timePasses();
    void Event___timePassesAgain();

    //////// State Machine: _work ////////


signals:
    //
    //    OUTPUT ACTION SIGNALS
    //
    //    Connect to these signals to a slot corresponding to a entryAction or exitAction
    //
    //////// State Machine: _lifeInADay ////////
    void Action___unwind();
    void Action___feelRefreshed();
    void Action___goUnconcious();
    void Action___dream();
    void Action___snore();
    void Action___openEyes();
    void Action___meetPeople();
    void Action___turnOnComputer();
    void Action___getPaid();
    void Action___accomplishGoals();

    //////// State Machine: _sleep ////////

    //////// State Machine: _work ////////


    //
    //    STATE CHANGE MONITOR SIGNALS
    //
    //    Connect to these signals to monitor state transitions
    //
    //////// State Machine: _lifeInADay ////////
    void Signal_StateReady___lifeInADay();
    void Signal_StateEntry___play();
    void Signal_StateExit___play();
    void Signal_StateEntry___sleep();
    void Signal_StateExit___sleep();
    void Signal_StateEntry___work();
    void Signal_StateExit___work();

    //////// State Machine: _sleep ////////
    void Signal_StateEntry___nREM();
    void Signal_StateExit___nREM();
    void Signal_StateEntry___rEM();
    void Signal_StateExit___rEM();

    //////// State Machine: _work ////////
    void Signal_StateEntry___talk();
    void Signal_StateExit___talk();
    void Signal_StateEntry___program();
    void Signal_StateExit___program();
    void Signal_StateEntry___think();
    void Signal_StateExit___think();


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
    //////// State Machine: _lifeInADay ////////
    void Slot_StateEntry___play();
    void Slot_StateExit___play();
    void Slot_StateEntry___sleep();
    void Slot_StateExit___sleep();
    void Slot_StateEntry___work();
    void Slot_StateExit___work();

    //////// State Machine: _sleep ////////
    void Slot_StateEntry___nREM();
    void Slot_StateExit___nREM();
    void Slot_StateEntry___rEM();
    void Slot_StateExit___rEM();

    //////// State Machine: _work ////////
    void Slot_StateEntry___talk();
    void Slot_StateExit___talk();
    void Slot_StateEntry___program();
    void Slot_StateExit___program();
    void Slot_StateEntry___think();
    void Slot_StateExit___think();


signals:
    // A Transition/Event slot's corresponding signal emitted in the slot
    //////// State Machine: _lifeInADay ////////
    void Relay_Event___goToBed_play();
    void Relay_Event___wakeUp_sleep();
    void Relay_Event___finishTasks_work();

    //////// State Machine: _sleep ////////
    void Relay_Event___timePasses_nREM();
    void Relay_Event___timePassesAgain_rEM();

    //////// State Machine: _work ////////


private:
    //////// State Machine: _lifeInADay ////////
    QStateMachine*    _lifeInADay;
    QState*    _play;
    QState*    _sleep;
    QState*    _work;

    //////// State Machine: _sleep ////////
    // child QState declared elsewhere _sleep
    QState*    _nREM;
    QState*    _rEM;

    //////// State Machine: _work ////////
    // child QState declared elsewhere _work
    QState*    _talk;
    QState*    _program;
    QState*    _think;


};

#endif // LIFEINADAY_H
