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

#include "iattribute.h"
#include <QDebug>
#include <QMetaType>

// IAttribute - a base class for attributes


IAttribute::IAttribute() : QObject(), _key()
{
    qRegisterMetaType<IAttribute>("IAttribute");
    qRegisterMetaType<IAttribute*>("IAttribute*");

}

IAttribute::IAttribute(QObject *parent, QString initkey) : QObject(parent), _key(initkey)
{
    qRegisterMetaType<IAttribute>("IAttribute");
    qRegisterMetaType<IAttribute*>("IAttribute*");
}

IAttribute::IAttribute(const IAttribute& a) : QObject(a.parent()), _key(a.key())
{
    qRegisterMetaType<IAttribute>("IAttribute");
    qRegisterMetaType<IAttribute*>("IAttribute*");
}

IAttribute& IAttribute::operator =(const IAttribute& a)
{
    QObject(a.parent());
    _key = a._key;
    return *this;
}

QString IAttribute::key() const {return _key; }


// GenericAttribute


GenericAttribute::GenericAttribute() : IAttribute()
{
}

GenericAttribute::GenericAttribute(QObject *parent, QString initkey, QString initValue) :
    IAttribute(parent, initkey),
    _value(initValue)
{
}

GenericAttribute::GenericAttribute(const GenericAttribute& a) : IAttribute(a)
{
}

GenericAttribute& GenericAttribute::operator =(const GenericAttribute& a)
{
    IAttribute::operator =(a);
    _value = a._value;
    return *this;
}

QString GenericAttribute::asString()
{
    return _value;
}

void GenericAttribute::setValue(const QString value)
{
    if(_value!=value)
    {
//        qDebug() << "GenericAttribute::setValue " << this->_key<<":" << this->_value;
        _value = value;
        emit changed(this);
    }
}



// container - a Map of IAttribute-subclasses


IAttributeContainer::IAttributeContainer(QObject *parent, QString key) :
        QObject(parent), QMap<QString,IAttribute*>(),
        _containerName(key)
{
    qRegisterMetaType<IAttribute>("IAttribute");
}

IAttributeContainer::IAttributeContainer(const IAttributeContainer& c):
        QObject(NULL), // can't copy QObjects
        QMap<QString,IAttribute*>(c),
        _containerName(c._containerName)
{
    qRegisterMetaType<IAttribute>("IAttribute");
}

IAttributeContainer::IAttributeContainer():
        QObject(), // can't copy QObjects
        QMap<QString,IAttribute*>(),
        _containerName()
{
    qRegisterMetaType<IAttribute>("IAttribute");
}

IAttributeContainer::~IAttributeContainer()
{
    QMapIterator<QString,IAttribute*> i(*this);
    while (i.hasNext())
    {
        QString key  = i.next().key();
        IAttribute* attr = this->value(key)  ;
        delete attr;
    }
}

QString IAttributeContainer::key() {return _containerName; }



bool IAttributeContainer::addItem( IAttribute* attr)
{
    if (  this->contains( attr->key() ) )
    {
        IAttribute * a = this->value( attr->key());
        a->setValue( attr->asString());
        return false;
    }
    else
    {
        QString k = attr->key();
        this->insert(k, attr);
        emit attributeAdded(attr);
        return true;
    }
}


/**
      * \fn operator=
      * \abstract sets the values of the abtribute list to the same values as the passed-in list, equivalent to setAttributes
      */
IAttributeContainer& IAttributeContainer::operator=( const IAttributeContainer& copyFrom )
{
    setAttributes(copyFrom);
    return *this;
}

