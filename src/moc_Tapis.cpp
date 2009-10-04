/****************************************************************************
** Meta object code from reading C++ file 'Tapis.h'
**
** Created: Tue Jan 1 21:37:52 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.3.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "windows/Tapis.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'Tapis.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.3.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

static const uint qt_meta_data_Tapis[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // signals: signature, parameters, type, tag, flags
       7,    6,    6,    6, 0x05,
      29,    6,    6,    6, 0x05,
      53,    6,    6,    6, 0x05,

 // slots: signature, parameters, type, tag, flags
      78,    6,    6,    6, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_Tapis[] = {
    "Tapis\0\0sgnlViewportClicked()\0"
    "sgnlClickCard(GfxCard*)\0"
    "sgnlMoveCursor(GfxCard*)\0setFilter(Filter)\0"
};

const QMetaObject Tapis::staticMetaObject = {
    { &QGraphicsView::staticMetaObject, qt_meta_stringdata_Tapis,
      qt_meta_data_Tapis, 0 }
};

const QMetaObject *Tapis::metaObject() const
{
    return &staticMetaObject;
}

void *Tapis::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Tapis))
	return static_cast<void*>(const_cast< Tapis*>(this));
    return QGraphicsView::qt_metacast(_clname);
}

int Tapis::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QGraphicsView::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: sgnlViewportClicked(); break;
        case 1: sgnlClickCard((*reinterpret_cast< GfxCard*(*)>(_a[1]))); break;
        case 2: sgnlMoveCursor((*reinterpret_cast< GfxCard*(*)>(_a[1]))); break;
        case 3: setFilter((*reinterpret_cast< Filter(*)>(_a[1]))); break;
        }
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void Tapis::sgnlViewportClicked()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void Tapis::sgnlClickCard(GfxCard * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void Tapis::sgnlMoveCursor(GfxCard * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
