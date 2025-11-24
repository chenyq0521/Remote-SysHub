/****************************************************************************
** Meta object code from reading C++ file 'serverdlg.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.6.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../serverdlg.h"
#include <QtCore/qmetatype.h>

#if __has_include(<QtCore/qtmochelpers.h>)
#include <QtCore/qtmochelpers.h>
#else
QT_BEGIN_MOC_NAMESPACE
#endif


#include <memory>

#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'serverdlg.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_CLASSServerDlgENDCLASS_t {};
static constexpr auto qt_meta_stringdata_CLASSServerDlgENDCLASS = QtMocHelpers::stringData(
    "ServerDlg",
    "on_ListenButton_clicked",
    "",
    "on_CmdButton_clicked",
    "on_ProcessButton_clicked",
    "on_WindowButton_clicked",
    "HandlePacket",
    "CONTEXT_OBJECT*",
    "ctx",
    "HandleClientLogin",
    "AddLoginToClientTable",
    "LOGIN_INFORMAITON",
    "info",
    "RemoveClient",
    "qintptr",
    "socketDescriptor",
    "CreateWorker",
    "istoken",
    "on_RegistryButton_clicked"
);
#else  // !QT_MOC_HAS_STRING_DATA
struct qt_meta_stringdata_CLASSServerDlgENDCLASS_t {
    uint offsetsAndSizes[38];
    char stringdata0[10];
    char stringdata1[24];
    char stringdata2[1];
    char stringdata3[21];
    char stringdata4[25];
    char stringdata5[24];
    char stringdata6[13];
    char stringdata7[16];
    char stringdata8[4];
    char stringdata9[18];
    char stringdata10[22];
    char stringdata11[18];
    char stringdata12[5];
    char stringdata13[13];
    char stringdata14[8];
    char stringdata15[17];
    char stringdata16[13];
    char stringdata17[8];
    char stringdata18[26];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_CLASSServerDlgENDCLASS_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_CLASSServerDlgENDCLASS_t qt_meta_stringdata_CLASSServerDlgENDCLASS = {
    {
        QT_MOC_LITERAL(0, 9),  // "ServerDlg"
        QT_MOC_LITERAL(10, 23),  // "on_ListenButton_clicked"
        QT_MOC_LITERAL(34, 0),  // ""
        QT_MOC_LITERAL(35, 20),  // "on_CmdButton_clicked"
        QT_MOC_LITERAL(56, 24),  // "on_ProcessButton_clicked"
        QT_MOC_LITERAL(81, 23),  // "on_WindowButton_clicked"
        QT_MOC_LITERAL(105, 12),  // "HandlePacket"
        QT_MOC_LITERAL(118, 15),  // "CONTEXT_OBJECT*"
        QT_MOC_LITERAL(134, 3),  // "ctx"
        QT_MOC_LITERAL(138, 17),  // "HandleClientLogin"
        QT_MOC_LITERAL(156, 21),  // "AddLoginToClientTable"
        QT_MOC_LITERAL(178, 17),  // "LOGIN_INFORMAITON"
        QT_MOC_LITERAL(196, 4),  // "info"
        QT_MOC_LITERAL(201, 12),  // "RemoveClient"
        QT_MOC_LITERAL(214, 7),  // "qintptr"
        QT_MOC_LITERAL(222, 16),  // "socketDescriptor"
        QT_MOC_LITERAL(239, 12),  // "CreateWorker"
        QT_MOC_LITERAL(252, 7),  // "istoken"
        QT_MOC_LITERAL(260, 25)   // "on_RegistryButton_clicked"
    },
    "ServerDlg",
    "on_ListenButton_clicked",
    "",
    "on_CmdButton_clicked",
    "on_ProcessButton_clicked",
    "on_WindowButton_clicked",
    "HandlePacket",
    "CONTEXT_OBJECT*",
    "ctx",
    "HandleClientLogin",
    "AddLoginToClientTable",
    "LOGIN_INFORMAITON",
    "info",
    "RemoveClient",
    "qintptr",
    "socketDescriptor",
    "CreateWorker",
    "istoken",
    "on_RegistryButton_clicked"
};
#undef QT_MOC_LITERAL
#endif // !QT_MOC_HAS_STRING_DATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSServerDlgENDCLASS[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   74,    2, 0x08,    1 /* Private */,
       3,    0,   75,    2, 0x08,    2 /* Private */,
       4,    0,   76,    2, 0x08,    3 /* Private */,
       5,    0,   77,    2, 0x08,    4 /* Private */,
       6,    1,   78,    2, 0x08,    5 /* Private */,
       9,    1,   81,    2, 0x08,    7 /* Private */,
      10,    1,   84,    2, 0x08,    9 /* Private */,
      13,    1,   87,    2, 0x08,   11 /* Private */,
      16,    1,   90,    2, 0x08,   13 /* Private */,
      18,    0,   93,    2, 0x08,   15 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 7,    8,
    QMetaType::Void, 0x80000000 | 7,    8,
    QMetaType::Void, 0x80000000 | 11,   12,
    QMetaType::Void, 0x80000000 | 14,   15,
    QMetaType::Void, QMetaType::UChar,   17,
    QMetaType::Void,

       0        // eod
};

Q_CONSTINIT const QMetaObject ServerDlg::staticMetaObject = { {
    QMetaObject::SuperData::link<QMainWindow::staticMetaObject>(),
    qt_meta_stringdata_CLASSServerDlgENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSServerDlgENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSServerDlgENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<ServerDlg, std::true_type>,
        // method 'on_ListenButton_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_CmdButton_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_ProcessButton_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_WindowButton_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'HandlePacket'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<CONTEXT_OBJECT *, std::false_type>,
        // method 'HandleClientLogin'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<CONTEXT_OBJECT *, std::false_type>,
        // method 'AddLoginToClientTable'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const LOGIN_INFORMAITON &, std::false_type>,
        // method 'RemoveClient'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<qintptr, std::false_type>,
        // method 'CreateWorker'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<unsigned char, std::false_type>,
        // method 'on_RegistryButton_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void ServerDlg::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<ServerDlg *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->on_ListenButton_clicked(); break;
        case 1: _t->on_CmdButton_clicked(); break;
        case 2: _t->on_ProcessButton_clicked(); break;
        case 3: _t->on_WindowButton_clicked(); break;
        case 4: _t->HandlePacket((*reinterpret_cast< std::add_pointer_t<CONTEXT_OBJECT*>>(_a[1]))); break;
        case 5: _t->HandleClientLogin((*reinterpret_cast< std::add_pointer_t<CONTEXT_OBJECT*>>(_a[1]))); break;
        case 6: _t->AddLoginToClientTable((*reinterpret_cast< std::add_pointer_t<LOGIN_INFORMAITON>>(_a[1]))); break;
        case 7: _t->RemoveClient((*reinterpret_cast< std::add_pointer_t<qintptr>>(_a[1]))); break;
        case 8: _t->CreateWorker((*reinterpret_cast< std::add_pointer_t<uchar>>(_a[1]))); break;
        case 9: _t->on_RegistryButton_clicked(); break;
        default: ;
        }
    }
}

const QMetaObject *ServerDlg::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ServerDlg::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSServerDlgENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int ServerDlg::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 10)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 10)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 10;
    }
    return _id;
}
QT_WARNING_POP
