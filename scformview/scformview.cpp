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

#include "scformview.h"
#include <QHBoxLayout>
#include <QTableWidget>
#include <QVariant>
#include <QtGui>
#include <QLabel>
#include <QList>
#include <QVariant>
#include "customtreewidgetitem.h"
#include "stateselectionwindow.h"
#include "textblock.h"
#include <QToolBar>
#include <QMessageBox>
#include <QAction>
#include <QMenuBar>
#include <QFontComboBox>
#include <QToolButton>
#include <QButtonGroup>
#include <QSignalMapper>

SCFormView::SCFormView(QWidget *parent, SCDataModel *dataModel) :
        QMainWindow(parent, Qt::Window),
        _dm(dataModel),
        _currentlySelected(NULL),
        _previouslySelected(NULL)
{
this->resize(600,1000);
    createActions();
    createMenus();
    createToolbars();

    // central widget and outter layout

    QHBoxLayout *layout = new QHBoxLayout;  // main layout, horizontal
    QWidget *widget = new QWidget;
    widget->setLayout(layout);
    setCentralWidget(widget);

    //  start tree view

    stateChartTreeView = new QTreeWidget();
    stateChartTreeView->setColumnCount(1);
    connect ( stateChartTreeView, SIGNAL(itemClicked(QTreeWidgetItem*,int)), this, SLOT(handleTreeViewItemClicked(QTreeWidgetItem*,int)));

    QVBoxLayout *treeLayout = new QVBoxLayout();
    //selectedChartItem = new QLabel();

    treeLayout->addWidget(editToolBar);             // add the edit tool bar to the tree layout
    //treeLayout->addWidget(selectedChartItem);

    treeLayout->addWidget (stateChartTreeView );    // first item added to layout
    layout->addLayout(treeLayout);


    QVBoxLayout *propertyLayout = new QVBoxLayout;     // property table is a vertical layout
    layout->addLayout(propertyLayout );                // second item added to horizontal layout


    //propertyLayout->addWidget( selectedChartItem);     // sec
    propertyLayout->addWidget( propertyToolBar);

    //  property table

    propertyTable = new QTableWidget();
    propertyTable->setColumnCount(2);

    propertyLayout->addWidget( propertyTable );

    setWindowTitle(tr("State Chart Tree Editor"));
    setUnifiedTitleAndToolBarOnMac(true);

    QList<SCState*> states;
    states.append( _dm->getTopState());

    connect (_dm, SIGNAL(newTransitionSignal(SCTransition*)), this, SLOT(handleNewTransition(SCTransition*)));
    connect (_dm, SIGNAL(newStateSignal(SCState*)), this, SLOT(handleNewState(SCState*)));
    // TODO make a handle new textblock for scformview

    loadTree (NULL, states);


}

void SCFormView::handleNewTransition(SCTransition*t)
{
    // not sure what this does...
    (void)t;
    QList<SCState*> states;
    states.append( _dm->getTopState());

    stateChartTreeView->clear();

    loadTree (NULL, states);

}

/**
 * @brief SCFormView::handleNewState
 * @param s
 *
 * connected to the signal newStateSignal in SCDataModel
 *
 * will create a new state in the formview and reload the tree
 *
 */
void SCFormView::handleNewState(SCState*s)
{
    (void)s;
    // pass the loadTree function a list of top-level states and starting node
    // the top node (NULL) has only one top state
    QList<SCState*> states;
    states.append( _dm->getTopState());

    stateChartTreeView->clear();
    loadTree (NULL, states);
}

void SCFormView :: setSelectedTreeItem(QObject * q){
    _previouslySelected = _currentlySelected;
    _currentlySelected = q;
    // TODO check this if its null first then do some default protocol if null
//    qDebug() << "updated the tree items: " << _previouslySelected << " + " << _currentlySelected;
}

void SCFormView::handlePropertyCellChanged(int r, int c)
{
    if ( c != 1 ) return;

    IAttributeContainer * attributes;
    IAttribute * attr;

    QString key = propertyTable->item(r,0)->text();
    QString value = propertyTable->item(r,1)->text();

    SCItem * item = dynamic_cast<SCItem*>(_currentlySelected);
    attributes = item->getAttributes();
    attr = attributes->value(key);
    attr->setValue(value);
}

void SCFormView::handleTextBlockDeleted(QObject *t)
{



    QList<SCState*> states;
    states.append( _dm->getTopState());

    stateChartTreeView->clear();

    loadTree (NULL, states);
}


void SCFormView::handleTransitionDeleted(QObject *t)
{
    if ( t == _currentlySelected){
        setSelectedTreeItem(NULL);
        // _currentlySelected = NULL;
    }

    QList<SCState*> states;
    states.append( _dm->getTopState());

    stateChartTreeView->clear();

    loadTree (NULL, states);
}

QObject* SCFormView::getNeighborState(QObject*s)
{
    QObject *prev=0;

    QList<SCState*> states;
    _dm->getAllStates(states);
    int c = states.count();

    for(int i = 0; i < c; i++)
    {
        SCState * st = states.at(i);
        if ( s == st)
            return prev;
        prev = st;
    }

    return prev;
}

void SCFormView::handleStateDeleted(QObject *s)
{
    if ( s == _currentlySelected)
    {

        // find a neighbor state to select after this one is gone
        setSelectedTreeItem(getNeighborState(s));
        //_currentlySelected = getNeighborState(s);
    }

    QList<SCState*> states;
    states.append( _dm->getTopState());

    stateChartTreeView->clear();

    loadTree (NULL, states);

}

void SCFormView::reloadTree()
{
    QList<SCState*> states;
    states.append( _dm->getTopState());
    stateChartTreeView->clear();
    loadTree (NULL, states);
}

// recursively walk through the state tree and build the tree view

void SCFormView::loadTree ( CustomTreeWidgetItem * parentItem , QList<SCState*> & states)
{

    int c = states.count();

    for(int i = 0; i < c; i++)
    {
        SCState * st = states.at(i);

        if ( !  st ) continue;

        connect(st, SIGNAL(destroyed(QObject*)), this, SLOT(handleStateDeleted(QObject*)), Qt::QueuedConnection);


        CustomTreeWidgetItem * item=0;

        if (parentItem == 0)
        {
            item = new CustomTreeWidgetItem();
            item->setExpanded(true);
            stateChartTreeView->addTopLevelItem((QTreeWidgetItem*)item);
        }
        else
        {
            item = new CustomTreeWidgetItem(parentItem);
        }


        item->setData( st);

        item->setText(0, st->attributes.value("name")->asString());

        item->setIcon(0,QIcon(":/SCFormView/statebutton.bmp"));

        // get all transitions in this state
        QList<SCTransition*> transitions;

        st->getTransitions(transitions);

        loadTree (item, transitions);


        // get all substates of this state
        QList<SCState*> subStates;

        st->getStates(subStates);

        loadTree (item, subStates);

        // get the state's text block
        SCTextBlock  * textBlock = st->getIDTextBlock();
        loadTree (item, textBlock);

    }

    // now re-selete the previously selected item
    if ( parentItem)
    {
        QTreeWidgetItemIterator it(parentItem);
        while (*it)
        {
            CustomTreeWidgetItem *customItem = dynamic_cast<CustomTreeWidgetItem*>(*it);
            if (customItem==0)break;
            if ( customItem->data() == _currentlySelected)
            {
                customItem->setSelected(true);
                handleTreeViewItemClicked(customItem,0);
                break;
            }
            ++it;
        }
        parentItem->setExpanded(true);
    }
}


void SCFormView::loadTree ( CustomTreeWidgetItem * parentItem , SCTextBlock* textBlock)
{

    CustomTreeWidgetItem * item=0;

    connect(textBlock, SIGNAL(destroyed(QObject*)), this, SLOT(handleTextBlockDeleted(QObject*)), Qt::QueuedConnection);

    item = new CustomTreeWidgetItem(parentItem);
    item->setData(textBlock);

    QString text = textBlock->getText();
    item->setText(0, text);

    item->setIcon(0,QIcon(":/SCFormView/textblock.bmp"));




}


void SCFormView::loadTree ( CustomTreeWidgetItem * parentItem , QList<SCTransition*> & transitions)
{

    int c = transitions.count();

    for(int i = 0; i < c; i++)
    {
        SCTransition * tr = transitions.at(i);

        if ( !  tr ) continue;

        connect(tr, SIGNAL(destroyed(QObject*)), this, SLOT(handleTransitionDeleted(QObject*)), Qt::QueuedConnection);

        CustomTreeWidgetItem * item=0;

        item = new CustomTreeWidgetItem(parentItem);

        item->setData( tr);

        item->setText(0, tr->attributes.value("target")->asString());

        item->setIcon(0,QIcon(":/SCFormView/transitionbutton.bmp"));


        // load the transition's text blocks



        for(int i = 0; i < tr->getTextBlocks().count();i++)
        {
            SCTextBlock  * textBlock = tr->getTextBlocks().at(i);
            loadTree (item, textBlock);
        }

        // TODO
        //connect(tr, SIGNAL(transitionAddTextBlock(SCTextBlock*)), parentItem, SLOT(treeAddTextBlock(SCTextBlock*)));        // connect the transition to the tree when creating a new text block
        // connect(tr, SIGNAL(transitionAddTextBlock(SCTextBlock*)),

    }

}


IAttributeContainer * SCFormView::getCurrentlySelectedAttributes()
{
    IAttributeContainer * attributes=NULL;
    SCState * st = dynamic_cast<SCState *>(  _currentlySelected );
    SCTransition * transition = dynamic_cast<SCTransition *>(  _currentlySelected );
    SCTextBlock * textBlock  = dynamic_cast<SCTextBlock *>(  _currentlySelected );

    if ( st!= NULL)
    {
        attributes =  & st->attributes;
    }
    else if ( transition != NULL )
    {
        attributes =  & transition->attributes;
    }
    else if (textBlock != NULL)
    {
        attributes = & textBlock->attributes;
    }

    return attributes;
}


IAttributeContainer * SCFormView:: getPreviouslySelectedAttributes(){
    IAttributeContainer * attributes=NULL;
    SCState * st = dynamic_cast<SCState *>(  _previouslySelected );
    SCTransition * transition = dynamic_cast<SCTransition *>(  _previouslySelected );
    SCTextBlock * textBlock  = dynamic_cast<SCTextBlock *>(  _previouslySelected );

    if ( st!= NULL)
    {
        attributes =  & st->attributes;
    }
    else if ( transition != NULL )
    {
        attributes =  & transition->attributes;
    }
    else if (textBlock != NULL)
    {
        attributes = & textBlock->attributes;
    }

    return attributes;
}

/**
   returns the name of the currently selected tree item

   state: returns name attribute

   transition: returns target state attribute

   textblock: returns "TextBlock"

 */
QString SCFormView::getCurrentlySelectedTitle()
{
    QString title;

    IAttributeContainer * attributes;
    SCState * st = dynamic_cast<SCState *>(  _currentlySelected );
    SCTransition * transition = dynamic_cast<SCTransition *>(  _currentlySelected );
    SCTextBlock * textBlock  = dynamic_cast<SCTextBlock *>(  _currentlySelected );

    if ( st!= NULL)
    {
        attributes =  & st->attributes;
        IAttribute* attr = attributes->value("name")  ;
        title = attr->asString();

    }
    else if (transition != NULL )
    {
        attributes =  & transition->attributes;
        IAttribute* attr = attributes->value("target")  ;
        title = attr->asString();
    }
    else if (  textBlock != NULL )
    {
        title = QString("TextBlock");
    }

    return title;
}


/**
   returns a string of the currently selected tree view item's type
 */
QString SCFormView::getCurrentlySelectedType()
{

    SCState * st = dynamic_cast<SCState *>(  _currentlySelected );
    SCTransition * transition  = dynamic_cast<SCTransition *>(  _currentlySelected );
    SCTextBlock * textBlock  = dynamic_cast<SCTextBlock *>(  _currentlySelected );

    if ( st!= NULL)
    {
        return QString("State");
    }
    else if (transition != NULL )
    {
        return QString("Transition");
    }
    else if (  textBlock != NULL )
    {
        return QString("TextBlock");
    }

    return QString();
}

/**
 set attribtue Connections
 Connects or disconnects all attributes in the given attribute container to handleproperty changed.


 */
void SCFormView::setAttributeConnections(IAttributeContainer * atts, bool shouldConnect)
{
    if(!(atts)) // return if atts is null
    {
        //qDebug() << "Atts is null";
        return;
    }

    if(shouldConnect)
    {
        int row = 0;
        QMapIterator<QString,IAttribute*> i(*atts);
        while (i.hasNext())
        {
            QString key  = i.next().key();
            IAttribute* attr = atts->value(key)  ;

            connect ( attr, SIGNAL(changed(IAttribute*)), this, SLOT(handlePropertyChanged(IAttribute*)));

            QTableWidgetItem * propName = new QTableWidgetItem(key);

            propName->setFlags( (propName->flags() & (~Qt::ItemIsEditable)) | ((Qt::ItemIsEnabled)));

            QTableWidgetItem * propValue = new QTableWidgetItem(attr->asString());

            propValue->setFlags(propValue->flags() | (Qt::ItemIsEditable) | (Qt::ItemIsEnabled));

            propertyTable->setItem(row, 0, propName);
            propertyTable->setItem(row++, 1, propValue);

        }
    } else {

        QMapIterator<QString,IAttribute*> i(*atts);
        while (i.hasNext())
        {

            QString key  = i.next().key();
            IAttribute* attr = atts->value(key)  ;
            //qDebug() <<  "deleting attribute connection :" << attr->asString();
            disconnect ( attr, SIGNAL(changed(IAttribute*)), this, SLOT(handlePropertyChanged(IAttribute*)));
        }
    }
}

void SCFormView::clearPropertyTable()
{
    for (int r =0; r <propertyTable->rowCount(); r++ )
    {
        QTableWidgetItem * item = propertyTable->itemAt(r,0);
        delete item;
        item = propertyTable->itemAt(r,1);
        delete item;
    }
    propertyTable->clear();
    disconnect(propertyTable, SIGNAL(cellChanged(int,int)), this, SLOT(handlePropertyCellChanged(int,int)));
}

/**
 * @brief SCFormView::handleTreeViewItemClicked
 * Updates the attribute information for the currently highlight tree object
 * First clears the table of all attributes then
 * loads each attribute of the highlighted tree object
 * Disconnects handlePropertyCellChanged
 * connects handlePropertyCellChanged
 * @param qitem
 */
void SCFormView::handleTreeViewItemClicked(QTreeWidgetItem* qitem,int ){

    CustomTreeWidgetItem * item = dynamic_cast<CustomTreeWidgetItem*>(qitem);

    // display the type : State or Transition
    // display the name of the item
    // load its attributes in the table



    //_currentlySelected =  item->data();
    setSelectedTreeItem(item->data());
    //qDebug() << "handleTreeViewItemClicked Object name: "<<_currentlySelected->objectName();


    // load the Title

    QString selectedItemTitle = getCurrentlySelectedType()  + " : " +  getCurrentlySelectedTitle();

    //selectedChartItem->setText(selectedItemTitle );

    if (getCurrentlySelectedType() == "TextBlock" )
    {
        textToolBar->setEnabled(true);
    }
    else
    {
        textToolBar->setEnabled(false);
    }

    // clear the tabel, delete the old table items
    clearPropertyTable();
    // delete the old attribute connects
    IAttributeContainer * previousAttributes = getPreviouslySelectedAttributes();
    setAttributeConnections(previousAttributes, false);

    // load the new attributes
    IAttributeContainer * currentAttributes =  getCurrentlySelectedAttributes();

    qDebug()<< "loading current attributes with count " << currentAttributes->count();

    propertyTable->setRowCount(currentAttributes->count());
    setAttributeConnections(currentAttributes, true);



    // watch for user changes to the attributes
    connect(propertyTable, SIGNAL(cellChanged(int,int)), this, SLOT(handlePropertyCellChanged(int,int)));


}


/**
 * @brief SCFormView::reloadPropertyTable
 * reloads the property table with the currentAttributes
 */
void SCFormView::reloadPropertyTable()
{
    IAttributeContainer * currentAttributes =  getCurrentlySelectedAttributes();
    clearPropertyTable();                   // clear out the table

    propertyTable->setRowCount(currentAttributes->count());     // load the table to have a set number of elements
    setAttributeConnections(currentAttributes, false);      // set up the attribute connections but do not call connect
}

/**
 * @brief SCFormView::handlePropertyChanged
 * When a property change is detected from the FormView's cell or the GraphicsView, update the attribute value
 * @param attr
 */
void SCFormView::handlePropertyChanged(IAttribute *attr)
{
    // user changed something, walk the table to find the attribute that the user changed,
    // and update the data model

    QString parentName;
    QString currentTreeItemName;

    if((attr->parent()))
        parentName = attr->parent()->objectName();
    else
        parentName = "";



    currentTreeItemName = propertyTable->item(0,1)->text();


    //qDebug()<< "handlePropertyChanged: "<<attr->asString() << " parent: " << parentName << " currentTreeItem: " << currentTreeItemName;
    //qDebug()<< propertyTable->parent()->objectName();

        for (int r =0; r <propertyTable->rowCount(); r++ )
        {
            QString txt = propertyTable->item(r,0)->text();
            QString key = attr->key() ;


            if (  txt == key )
            {
                propertyTable->item(r,1)->setText( attr->asString() );

                if ( key =="name")
                {
                    // the user changed the name
                    // selectedChartItem->setText( attr->asString()  );


                    // reload the tree
                    stateChartTreeView->currentItem()->setText( 0, attr->asString() );
                    stateChartTreeView->currentItem()->child(0)->setText( 0, attr->asString() );


                }
            }

        }

}

void SCFormView::backgroundButtonGroupClicked(QAbstractButton *)
{

}


void SCFormView::buttonGroupClicked(int )
{

}



/**
 * @brief SCFormView::itemPromptTextBox
 *
 * connected to the QAction button insertTextBox
 *
 * will create a pop up for the user to name their new text box
 * and call itemInsertTextBox with the string input
 *
 */
void SCFormView::itemPromptTextBox()
{
    if(getCurrentlySelectedType().isEmpty())
    {
        this->sendMessage("Error","Please select a tree object");
        return;
    }

    // prompt a user for a text box name
    bool ok;
    QString input = promptTextInput("New Text Box", "Text Box Title","", &ok);

    if(input.isEmpty()) // should be allowed to do this actually.
    {

    }

    if(ok)
    {
        itemInsertTextBlock(dynamic_cast<SCItem*>(_currentlySelected), input);
    }
}

/**
 * @brief SCFormView::itemInsertTextBox
 * @param item
 * @param name
 *
 * creates a new text block and adds it to the QList of textblocks that belongs to item
 * and adds it to the tree
 */
void SCFormView::itemInsertTextBlock(SCItem *item, const QString name)
{
    qDebug() << "item inesrt text box called ";
    // add the text block to the data model
    if(!_dm->insertNewTextBlock(item, name)){
        qDebug() << "failed here";
        return;
    }

    // insert the new text box into the tree and sc graphics view


    // loadTree (item, item->getTextBlock());

    SCTransition* trans = dynamic_cast<SCTransition*>(item);
    SCState* state = dynamic_cast<SCState*>(item);
    if(state)
    {

    }
    else if(trans)
    {
        qDebug() << "trans texblock object name "<<trans->getTextBlock(name)->objectName();
        //loadTree(item, trans->getTextBlock(name));
        connect(trans->getTextBlock(name), SIGNAL(destroyed(QObject*)), this, SLOT(handleTextBlockDeleted(QObject*)), Qt::QueuedConnection);

        // add the textblock to the tree view and reload the tree.
        emit trans->transitionAddTextBlock(trans->getTextBlock(name));
        this->reloadTree();

       // add the textblock to the sc graphics view



    }

}

/**
   sendMessage

   will create a pop up message
   can set the window title and the message using the arguments

   has an OK button to dismiss the pop up
 */

void SCFormView::sendMessage(QString title, QString message)
{
    // TODO check if anything is highlighted
    QMessageBox msgBox;
    msgBox.setWindowTitle(title);
    msgBox.setText(message);
    //msgBox.setDetailedText(message);
    //msgBox.setInformativeText(message);
    //msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.setDefaultButton(QMessageBox::Ok);

    msgBox.setWindowFlags(Qt::WindowStaysOnTopHint);
    int ret = msgBox.exec();

    switch (ret)
    {
    case QMessageBox::Ok:
        // ok was pressed;
        break;

    default:
        // should never be reached
        break;
    }
}

/**
   promptTextInput

   Opens a popup with a text input field

   The arguments will set the pop up's window title, the message, and set the intial input text field's text

   returns the input text by the user or (not tested) an empty string if nothing is returned

 */
const QString SCFormView::promptTextInput(QString windowTitle, QString message, QString defaultText, bool* ok)
{
    QInputDialog qid;
    qid.setInputMode(QInputDialog::TextInput);
    //qid.setWindowFlags(Qt);
    int tet = Qt::WindowStaysOnTopHint | Qt::Popup | Qt::WindowMinMaxButtonsHint; //& (~(Qt::WindowContextHelpButtonHint));
    //Qt::WindowCloseButtonHint |
    qDebug() << "test: "<<tet;
    qid.setWindowFlags(static_cast<Qt::WindowFlags>(tet));
 //   qid.set
    return qid.getText(NULL, windowTitle, message,QLineEdit::Normal,defaultText, ok);
}

/**
 * @brief SCFormView::itemPromptProperty
 *
 * connected to the QAction insertProperty
 *
 * prompts the user for a property name to add to the currently highlighted item when the button is pressed
 *
 *
 * button pressed > itemPromptProperty() for the name > itemInsertProperty() to update the datamodel and the table
 *
 *
 */
void SCFormView::itemPromptProperty()
{

    // check if there is a highlighted tree object to add a property to
    if(getCurrentlySelectedType().isEmpty())
    {
        this->sendMessage("Error","Please select a tree object");
        return;
    }


    // prompt the user for a property name
    bool ok;
    QString input = this->promptTextInput("New Property", getCurrentlySelectedType()+" Property", "", &ok);

    // check if the property name has a space (not supported by scxml reading)
    if(input.contains(' '))
    {
        sendMessage("Error", "Property Name cannot contain spaces");
        return;
    }
    else if(input.isEmpty())
    {
        sendMessage("Error", "Property Name cannot be empty");
        return;
    }

    // check if the input is valid
    if(ok && !input.isEmpty())
    {
        // the user inputted something so insert this as a new property
        itemInsertProperty(dynamic_cast<SCItem*>(_currentlySelected),input);
    }
}

/**
 * @brief SCFormView::itemInsertProperty
 * @param item
 * @param propertyName
 * Creates a new property for the highlighted tree object with given property name

   will update the datamodel for both SCTransitions and SCStates

   will also update the current property table, adding the new property to the top of the table
 */
void SCFormView::itemInsertProperty(SCItem* item, QString propertyName)
{
    QString itemType = getCurrentlySelectedType();

    qDebug() << "item Type " << itemType;
    if(!_dm->insertNewProperty(item, propertyName))
        return; // failed to insert, so halt here.

    // insert the new table item
    QTableWidgetItem * propName = new QTableWidgetItem(propertyName);
    propName->setFlags( (propName->flags() & (~Qt::ItemIsEditable)) | ((Qt::ItemIsEnabled)));

    QTableWidgetItem * propValue = new QTableWidgetItem("");
    propValue->setFlags(propValue->flags() | (Qt::ItemIsEditable) | (Qt::ItemIsEnabled));

    // insert the new property at the top of the table
    propertyTable->insertRow(0);
    propertyTable->setItem(0, 0, propName);
    propertyTable->setItem(0, 1, propValue);
}


/**
   itemDeleteSelectedProperty()

  Deletes the currently selected propertyTable property if one is currently selected
  will remove it from the scform propertyTable and from the dataModel

 */
void SCFormView::itemDeleteSelectedProperty()
{
    // check for the highlighted row
    QString propertyName;
    QTableWidgetItem* marked = NULL;
    marked = propertyTable->currentItem();

    if(!marked)
    {
        sendMessage("Error", "Please select a property");
        return;
    }

    propertyName = propertyTable->item(marked->row(),0)->text();    // get the property name regardless of the column

    qDebug() <<"propertyName : " << propertyName;

    //propertyName = propertyTable->selectedItems().at(0);



    //propertyTable->getSelectedItem();
    qDebug() << "deleting";



    SCItem* item = dynamic_cast<SCItem*>(_currentlySelected);



    if(!_dm->deleteProperty(item, propertyName))
    {
        qDebug() << "failed to delete!";
        return;
    }

    propertyTable->removeRow(marked->row());    // remove the deleted property from the property table


}


void SCFormView::deleteItem()
{
    // TODO check if anything is highlighted
    QMessageBox msgBox;
    QString itemType = getCurrentlySelectedType();

    msgBox.setText("Confirm delete");
    msgBox.setInformativeText("Are you sure you want to delete item: " + _currentlySelected->objectName() + " (" + itemType + ")");
    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Cancel);
    msgBox.setWindowFlags( Qt::WindowStaysOnTopHint);
    int ret = msgBox.exec();

    switch (ret)
    {
    case QMessageBox::Ok:
        deleteItem(_currentlySelected);
        break;
    case QMessageBox::Cancel:
        // Cancel was clicked
        break;
    default:
        // should never be reached
        break;
    }
}

void SCFormView::deleteItem(QObject * item)
{

    _dm->deleteItem(item);

}

void SCFormView::pointerGroupClicked(int)
{

}


void SCFormView::bringToFront()
{

}


void SCFormView::insertTransition()
{
    SCState * st = dynamic_cast<SCState *> (_currentlySelected);
    if ( st == NULL ) return;

    if ( st->parent() == NULL)
    {
        QMessageBox msgBox;
        msgBox.setText("cannot add state from root machine");
        msgBox.exec();
        return;
    }

    // need to force the user to select a target state for this new transition

    // create a new state tree window to select a state from
    _targetStateSelectionWindow = new StateSelectionWindow(NULL, _dm);

    connect( _targetStateSelectionWindow, SIGNAL(stateSelected(SCState*,QString)), this, SLOT(handleStateSelectionWindowStateSelected(SCState*,QString)));
    _targetStateSelectionWindow->show();


}

void SCFormView::handleStateSelectionWindowStateSelected(SCState* ,QString targetName)
{
    // user has clicked on a new state, create the transition with this target state

    SCState * st  = dynamic_cast<SCState*>(_currentlySelected);
    _dm->insertNewTransition(st, targetName);
    _targetStateSelectionWindow->close();

    delete _targetStateSelectionWindow;
    _targetStateSelectionWindow = NULL;

}

#include <QDebug>

void SCFormView::insertState()
{
    SCState * st  = dynamic_cast<SCState*>(_currentlySelected);
    if ( st == NULL ) return;

    qDebug() << "inserting new state into parent = " + _currentlySelected->objectName();

    _dm->insertNewState(st);

}

void SCFormView::sendToBack()
{

}




void SCFormView::textInserted(QGraphicsTextItem *)
{

}


void SCFormView::currentFontChanged(const QFont &)
{

}


void SCFormView::fontSizeChanged(const QString &)
{

}


void SCFormView::sceneScaleChanged(const QString &)
{

}


void SCFormView::textColorChanged()
{

}


void SCFormView::itemColorChanged()
{

}


void SCFormView::lineColorChanged()
{

}


void SCFormView::textButtonTriggered()
{

}


void SCFormView::fillButtonTriggered()
{

}





void SCFormView::lineButtonTriggered()
{

}


void SCFormView::handleFontChange()
{

}




void SCFormView::about()
{
    QMessageBox::about(this, tr("About Visual State Chart"),
                       tr("<b>VisualSC</b>  is a State Chart Editor "
                          "which read/writes from/to SCXML"));
}




void SCFormView::createActions()
{
    // TODO button
    // insert properties

    // Property Table stuff
    /*
    addPropertyToolButton = new QToolButton();
    addPropertyToolButton->setPopupMode(QToolButton::MenuButtonPopup);

    propertyToolBar = addToolBar(tr("Add"));
    propertyToolBar->addWidget(addPropertyToolButton);
    */

    insertStateAction = new QAction(QIcon(":/SCFormView/statebutton.bmp"), tr("Insert State"), this);
    insertStateAction->setShortcut(tr("Ctrl+I"));
    insertStateAction->setStatusTip(tr("Insert State"));
    connect(insertStateAction, SIGNAL(triggered()), this, SLOT(insertState()));


    insertTransitionAction = new QAction(QIcon(":/SCFormView/transitionbutton.bmp"), tr("Insert Transition"), this);
    insertTransitionAction->setShortcut(tr("Ctrl+I"));
    insertTransitionAction->setStatusTip(tr("Insert Transition"));
    connect(insertTransitionAction, SIGNAL(triggered()), this, SLOT(insertTransition()));


    toFrontAction = new QAction(QIcon(":/SCFormView/bringtofront.png"),
                                tr("Bring to &Front"), this);
    toFrontAction->setShortcut(tr("Ctrl+F"));
    toFrontAction->setStatusTip(tr("Bring item to front"));
    connect(toFrontAction, SIGNAL(triggered()), this, SLOT(bringToFront()));


    sendBackAction = new QAction(QIcon(":/SCFormView/sendtoback.png"),
                                 tr("Send to &Back"), this);
    sendBackAction->setShortcut(tr("Ctrl+B"));
    sendBackAction->setStatusTip(tr("Send item to back"));
    connect(sendBackAction, SIGNAL(triggered()), this, SLOT(sendToBack()));

    deleteAction = new QAction(QIcon(":/SCFormView/delete.png"),
                               tr("&Delete"), this);
    deleteAction->setShortcut(tr("Delete"));
    deleteAction->setStatusTip(tr("Delete item from diagram"));
    connect(deleteAction, SIGNAL(triggered()), this, SLOT(deleteItem()));

    exitAction = new QAction(tr("E&xit"), this);
    exitAction->setShortcuts(QKeySequence::Quit);
    exitAction->setStatusTip(tr("Quit VisualSC Editor"));
    connect(exitAction, SIGNAL(triggered()), this, SLOT(close()));

    boldAction = new QAction(tr("Bold"), this);
    boldAction->setCheckable(true);
    QPixmap pixmap(":/SCFormView/bold.png");
    boldAction->setIcon(QIcon(pixmap));
    boldAction->setShortcut(tr("Ctrl+B"));
    connect(boldAction, SIGNAL(triggered()), this, SLOT(handleBoldChanged()));

    italicAction = new QAction(QIcon(":/SCFormView/italic.png"),   tr("Italic"), this);
    italicAction->setCheckable(true);
    italicAction->setShortcut(tr("Ctrl+I"));
    connect(italicAction, SIGNAL(triggered()), this, SLOT(handleFontChange()));

    underlineAction = new QAction(QIcon(":/SCFormView/underline.png"),  tr("Underline"), this);
    underlineAction->setCheckable(true);
    underlineAction->setShortcut(tr("Ctrl+U"));
    connect(underlineAction, SIGNAL(triggered()), this, SLOT(handleFontChange()));

    aboutAction = new QAction(tr("A&bout"), this);
    aboutAction->setShortcut(tr("Ctrl+B"));
    connect(aboutAction, SIGNAL(triggered()), this, SLOT(about()));


    insertTextBox = new QAction(QIcon(":/SCFormView/textbox.png"),
                               tr("Insert Textbox"), this);
    insertTextBox->setShortcut(tr("Ctrl+T"));
    insertTextBox->setStatusTip(tr("Insert a text box for the selected item"));
    connect(insertTextBox, SIGNAL(triggered()), this, SLOT(itemPromptTextBox()));

    insertProperty = new QAction(QIcon(":/SCFormView/roundadd.png"), tr("Add Property"),this);
    insertProperty->setShortcut(tr("Ctrl+A"));
    insertProperty->setStatusTip(tr("Insert a property for the selected item"));
    connect(insertProperty, SIGNAL(triggered()), this, SLOT(itemPromptProperty()));

    deleteProperty = new QAction(QIcon(":/SCFormView/roundminus.png"), tr("Remove Property"), this);
    deleteProperty->setShortcut(tr("Ctrl+A"));
    deleteProperty->setStatusTip(tr("Insert a property for the selected item"));
    connect(deleteProperty, SIGNAL(triggered()), this, SLOT(itemDeleteSelectedProperty()));

}

void SCFormView::handleBoldChanged()
{
    SCTextBlock * tb = SCDataModel::getAsTextBlock(_currentlySelected);
    if ( ! tb ) return;

    IAttribute* attr = tb->attributes.value("font-bold")  ;

    if ( ! attr ) return;

    if ( boldAction->isChecked() )
        attr->setValue("true");
    else
        attr->setValue("false");

}

void SCFormView::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(exitAction);

    itemMenu = menuBar()->addMenu(tr("&Item"));
    itemMenu->addAction(deleteAction);
    itemMenu->addSeparator();
    itemMenu->addAction(toFrontAction);
    itemMenu->addAction(sendBackAction);
    itemMenu->addAction(insertTransitionAction);
    itemMenu->addAction(insertStateAction);

    aboutMenu = menuBar()->addMenu(tr("&Help"));
    aboutMenu->addAction(aboutAction);
}


void SCFormView::createToolbars()
{

    propertyToolBar = new QToolBar("Property",this);
    propertyToolBar->addAction(insertProperty);
    propertyToolBar->addAction(deleteProperty);
    propertyToolBar->addAction(insertTextBox);

    //editToolBar = addToolBar(tr("Edit"));
    editToolBar = new QToolBar("Edit", this);
    editToolBar->addAction(insertStateAction);
    editToolBar->addAction(insertTransitionAction);

    editToolBar->addAction(deleteAction);
    editToolBar->addAction(toFrontAction);
    editToolBar->addAction(sendBackAction);


    //editToolBar->addAction(insertTextBox);
    //editToolBar->addAction(insertProperty);

    fontCombo = new QFontComboBox();
    connect(fontCombo, SIGNAL(currentFontChanged(QFont)),
            this, SLOT(currentFontChanged(QFont)));

    fontSizeCombo = new QComboBox;
    fontSizeCombo->setEditable(true);
    for (int i = 8; i < 30; i = i + 2)
        fontSizeCombo->addItem(QString().setNum(i));
    QIntValidator *validator = new QIntValidator(2, 64, this);
    fontSizeCombo->setValidator(validator);
    connect(fontSizeCombo, SIGNAL(currentIndexChanged(QString)),
            this, SLOT(fontSizeChanged(QString)));

    fontColorToolButton = new QToolButton;
    fontColorToolButton->setPopupMode(QToolButton::MenuButtonPopup);
    fontColorToolButton->setMenu(createColorMenu(SLOT(textColorChanged()),
                                                 Qt::black));
    textAction = fontColorToolButton->menu()->defaultAction();
    fontColorToolButton->setIcon(createColorToolButtonIcon(
            ":/SCFormView/textpointer.png", Qt::black));
    fontColorToolButton->setAutoFillBackground(true);
    connect(fontColorToolButton, SIGNAL(clicked()),
            this, SLOT(textButtonTriggered()));




    fillColorToolButton = new QToolButton;
    fillColorToolButton->setPopupMode(QToolButton::MenuButtonPopup);
    fillColorToolButton->setMenu(createColorMenu(SLOT(itemColorChanged()),
                                                 Qt::white));
    fillAction = fillColorToolButton->menu()->defaultAction();
    fillColorToolButton->setIcon(createColorToolButtonIcon(
            ":/SCFormView/floodfill.png", Qt::white));
    connect(fillColorToolButton, SIGNAL(clicked()),
            this, SLOT(fillButtonTriggered()));



    lineColorToolButton = new QToolButton;
    lineColorToolButton->setPopupMode(QToolButton::MenuButtonPopup);
    lineColorToolButton->setMenu(createColorMenu(SLOT(lineColorChanged()),
                                                 Qt::black));
    lineAction = lineColorToolButton->menu()->defaultAction();
    lineColorToolButton->setIcon(createColorToolButtonIcon(
            ":/SCFormView/linecolor.png", Qt::black));
    connect(lineColorToolButton, SIGNAL(clicked()),
            this, SLOT(lineButtonTriggered()));

    textToolBar = addToolBar(tr("Font"));
    textToolBar->addWidget(fontCombo);
    textToolBar->addWidget(fontSizeCombo);
    textToolBar->addAction(boldAction);
    textToolBar->addAction(italicAction);
    textToolBar->addAction(underlineAction);
    textToolBar->setEnabled(false);


    colorToolBar = addToolBar(tr("Color"));
    colorToolBar->addWidget(fontColorToolButton);
    colorToolBar->addWidget(fillColorToolButton);
    colorToolBar->addWidget(lineColorToolButton);

    QToolButton *pointerButton = new QToolButton;
    pointerButton->setCheckable(true);
    pointerButton->setChecked(true);
    pointerButton->setIcon(QIcon(":/SCFormView/pointer.png"));
    QToolButton *linePointerButton = new QToolButton;
    linePointerButton->setCheckable(true);
    linePointerButton->setIcon(QIcon(":/SCFormView/linepointer.png"));

    pointerTypeGroup = new QButtonGroup(this);
    pointerTypeGroup->addButton(pointerButton, int(0));
    pointerTypeGroup->addButton(linePointerButton,
                                int(1));
    connect(pointerTypeGroup, SIGNAL(buttonClicked(int)),
            this, SLOT(pointerGroupClicked(int)));

    sceneScaleCombo = new QComboBox;
    QStringList scales;
    scales << tr("50%") << tr("75%") << tr("100%") << tr("125%") << tr("150%");
    sceneScaleCombo->addItems(scales);
    sceneScaleCombo->setCurrentIndex(2);
    connect(sceneScaleCombo, SIGNAL(currentIndexChanged(QString)),
            this, SLOT(sceneScaleChanged(QString)));

    pointerToolbar = addToolBar(tr("Pointer type"));
    pointerToolbar->addWidget(pointerButton);
    pointerToolbar->addWidget(linePointerButton);
    pointerToolbar->addWidget(sceneScaleCombo);


}





QMenu *SCFormView::createColorMenu(const char *slot, QColor defaultColor)
{
    QList<QColor> colors;
    colors << Qt::black << Qt::white << Qt::red << Qt::blue << Qt::yellow;
    QStringList names;
    names << tr("black") << tr("white") << tr("red") << tr("blue")
            << tr("yellow");

    QMenu *colorMenu = new QMenu(this);
    for (int i = 0; i < colors.count(); ++i) {
        QAction *action = new QAction(names.at(i), this);
        action->setData(colors.at(i));
        action->setIcon(createColorIcon(colors.at(i)));
        connect(action, SIGNAL(triggered()),
                this, slot);
        colorMenu->addAction(action);
        if (colors.at(i) == defaultColor) {
            colorMenu->setDefaultAction(action);
        }
    }
    return colorMenu;
}


QIcon SCFormView::createColorToolButtonIcon(const QString &imageFile,
                                                  QColor color)
{
    QPixmap pixmap(50, 80);
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    QPixmap image(imageFile);
    QRect target(0, 0, 50, 60);
    QRect source(0, 0, 42, 42);
    painter.fillRect(QRect(0, 60, 50, 80), color);
    painter.drawPixmap(target, image, source);

    return QIcon(pixmap);
}


QIcon SCFormView::createColorIcon(QColor color)
{
    QPixmap pixmap(20, 20);
    QPainter painter(&pixmap);
    painter.setPen(Qt::NoPen);
    painter.fillRect(QRect(0, 0, 20, 20), color);

    return QIcon(pixmap);
}
