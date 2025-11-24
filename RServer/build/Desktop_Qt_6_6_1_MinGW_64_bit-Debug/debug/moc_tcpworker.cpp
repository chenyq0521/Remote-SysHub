/****************************************************************************
** Meta object code from reading C++ file 'tcpworker.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.6.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../tcpworker.h"
#include <QtCore/qmetatype.h>

#if __has_include(<QtCore/qtmochelpers.h>)
#include <QtCore/qtmochelpers.h>
#else
QT_BEGIN_MOC_NAMESPACE
#endif


#include <memory>

#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'tcpworker.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.6.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {

#ifdef QT_MOC_HAS_STRINGDATA
struct qt_meta_stringdata_CLASSTcpWorkerENDCLASS_t {};
static constexpr auto qt_meta_stringdata_CLASSTcpWorkerENDCLASS = QtMocHelpers::stringData(
    "TcpWorker",
    "dataPrased",
    "",
    "CONTEXT_OBJECT*",
    "ctx",
    "connected",
    "disconnected",
    "finished",
    "Start",
    "OnReadyRead",
    "SendData"
);
#else  // !QT_MOC_HAS_STRING_DATA
struct qt_meta_stringdata_CLASSTcpWorkerENDCLASS_t {
    uint offsetsAndSizes[22];
    char stringdata0[10];
    char stringdata1[11];
    char stringdata2[1];
    char stringdata3[16];
    char stringdata4[4];
    char stringdata5[10];
    char stringdata6[13];
    char stringdata7[9];
    char stringdata8[6];
    char stringdata9[12];
    char stringdata10[9];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_CLASSTcpWorkerENDCLASS_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_CLASSTcpWorkerENDCLASS_t qt_meta_stringdata_CLASSTcpWorkerENDCLASS = {
    {
        QT_MOC_LITERAL(0, 9),  // "TcpWorker"
        QT_MOC_LITERAL(10, 10),  // "dataPrased"
        QT_MOC_LITERAL(21, 0),  // ""
        QT_MOC_LITERAL(22, 15),  // "CONTEXT_OBJECT*"
        QT_MOC_LITERAL(38, 3),  // "ctx"
        QT_MOC_LITERAL(42, 9),  // "connected"
        QT_MOC_LITERAL(52, 12),  // "disconnected"
        QT_MOC_LITERAL(65, 8),  // "finished"
        QT_MOC_LITERAL(74, 5),  // "Start"
        QT_MOC_LITERAL(80, 11),  // "OnReadyRead"
        QT_MOC_LITERAL(92, 8)   // "SendData"
    },
    "TcpWorker",
    "dataPrased",
    "",
    "CONTEXT_OBJECT*",
    "ctx",
    "connected",
    "disconnected",
    "finished",
    "Start",
    "OnReadyRead",
    "SendData"
};
#undef QT_MOC_LITERAL
#endif // !QT_MOC_HAS_STRING_DATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSTcpWorkerENDCLASS[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,   56,    2, 0x06,    1 /* Public */,
       5,    1,   59,    2, 0x06,    3 /* Public */,
       6,    1,   62,    2, 0x06,    5 /* Public */,
       7,    0,   65,    2, 0x06,    7 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       8,    0,   66,    2, 0x0a,    8 /* Public */,
       9,    0,   67,    2, 0x0a,    9 /* Public */,
      10,    0,   68,    2, 0x0a,   10 /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

Q_CONSTINIT const QMetaObject TcpWorker::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_CLASSTcpWorkerENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSTcpWorkerENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSTcpWorkerENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<TcpWorker, std::true_type>,
        // method 'dataPrased'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<CONTEXT_OBJECT *, std::false_type>,
        // method 'connected'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<CONTEXT_OBJECT *, std::false_type>,
        // method 'disconnected'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<CONTEXT_OBJECT *, std::false_type>,
        // method 'finished'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'Start'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'OnReadyRead'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'SendData'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void TcpWorker::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<TcpWorker *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->dataPrased((*reinterpret_cast< std::add_pointer_t<CONTEXT_OBJECT*>>(_a[1]))); break;
        case 1: _t->connected((*reinterpret_cast< std::add_pointer_t<CONTEXT_OBJECT*>>(_a[1]))); break;
        case 2: _t->disconnected((*reinterpret_cast< std::add_pointer_t<CONTEXT_OBJECT*>>(_a[1]))); break;
        case 3: _t->finished(); break;
        case 4: _t->Start(); break;
        case 5: _t->OnReadyRead(); break;
        case 6: _t->SendData(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (TcpWorker::*)(CONTEXT_OBJECT * );
            if (_t _q_method = &TcpWorker::dataPrased; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (TcpWorker::*)(CONTEXT_OBJECT * );
            if (_t _q_method = &TcpWorker::connected; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (TcpWorker::*)(CONTEXT_OBJECT * );
            if (_t _q_method = &TcpWorker::disconnected; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (TcpWorker::*)();
            if (_t _q_method = &TcpWorker::finished; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 3;
                return;
            }
        }
    }
}

const QMetaObject *TcpWorker::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *TcpWorker::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSTcpWorkerENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int TcpWorker::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 7)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 7;
    }
    return _id;
}

// SIGNAL 0
void TcpWorker::dataPrased(CONTEXT_OBJECT * _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void TcpWorker::connected(CONTEXT_OBJECT * _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void TcpWorker::disconnected(CONTEXT_OBJECT * _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void TcpWorker::finished()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}
QT_WARNING_POP
