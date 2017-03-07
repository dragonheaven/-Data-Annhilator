/****************************************************************************
** Meta object code from reading C++ file 'dbupdatedlg.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.7)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "dbupdatedlg.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'dbupdatedlg.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.7. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_DBUpdateDlg[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      13,   12,   12,   12, 0x0a,
      36,   12,   12,   12, 0x0a,
      57,   12,   12,   12, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_DBUpdateDlg[] = {
    "DBUpdateDlg\0\0on_updateBtn_clicked()\0"
    "on_backBtn_clicked()\0on_closeBtn_clicked()\0"
};

void DBUpdateDlg::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        DBUpdateDlg *_t = static_cast<DBUpdateDlg *>(_o);
        switch (_id) {
        case 0: _t->on_updateBtn_clicked(); break;
        case 1: _t->on_backBtn_clicked(); break;
        case 2: _t->on_closeBtn_clicked(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData DBUpdateDlg::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject DBUpdateDlg::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_DBUpdateDlg,
      qt_meta_data_DBUpdateDlg, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &DBUpdateDlg::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *DBUpdateDlg::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *DBUpdateDlg::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_DBUpdateDlg))
        return static_cast<void*>(const_cast< DBUpdateDlg*>(this));
    if (!strcmp(_clname, "Ui::DBUpdateDlg"))
        return static_cast< Ui::DBUpdateDlg*>(const_cast< DBUpdateDlg*>(this));
    return QDialog::qt_metacast(_clname);
}

int DBUpdateDlg::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    }
    return _id;
}
static const uint qt_meta_data_DBAutoUpdateDlg[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

static const char qt_meta_stringdata_DBAutoUpdateDlg[] = {
    "DBAutoUpdateDlg\0"
};

void DBAutoUpdateDlg::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObjectExtraData DBAutoUpdateDlg::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject DBAutoUpdateDlg::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_DBAutoUpdateDlg,
      qt_meta_data_DBAutoUpdateDlg, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &DBAutoUpdateDlg::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *DBAutoUpdateDlg::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *DBAutoUpdateDlg::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_DBAutoUpdateDlg))
        return static_cast<void*>(const_cast< DBAutoUpdateDlg*>(this));
    if (!strcmp(_clname, "Ui::DBAutoUpdateDlg"))
        return static_cast< Ui::DBAutoUpdateDlg*>(const_cast< DBAutoUpdateDlg*>(this));
    return QDialog::qt_metacast(_clname);
}

int DBAutoUpdateDlg::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
QT_END_MOC_NAMESPACE
