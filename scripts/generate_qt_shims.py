#!/usr/bin/env python3
"""Generate lightweight Qt C++ stub headers for offline IDE indexing & SonarLint.

Ensures that Clangd, SonarLint, and C/C++ extensions can parse Notepad Next translation
units without requiring a 20GB local Qt SDK installation.
"""

from pathlib import Path


def main():
    root = Path(__file__).resolve().parent.parent
    shims_dir = root / ".deps" / "qt_shims"
    shims_dir.mkdir(parents=True, exist_ok=True)

    qt_core_dir = shims_dir / "QtCore"
    qt_gui_dir = shims_dir / "QtGui"
    qt_widgets_dir = shims_dir / "QtWidgets"
    qt_print_dir = shims_dir / "QtPrintSupport"

    for d in (qt_core_dir, qt_gui_dir, qt_widgets_dir, qt_print_dir):
        d.mkdir(parents=True, exist_ok=True)

    base_header = shims_dir / "qt_shim_base.h"
    base_content = """#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>
#include <memory>
#include <iostream>
#include <utility>
#include <map>
#include <unordered_map>
#include <set>

#ifndef QT_BEGIN_NAMESPACE
#define QT_BEGIN_NAMESPACE
#define QT_END_NAMESPACE
#endif

#ifndef Q_NULLPTR
#define Q_NULLPTR nullptr
#endif

#ifndef signals
#define signals public
#endif

#ifndef slots
#define slots
#endif

#ifndef emit
#define emit
#endif

#ifndef Q_OBJECT
#define Q_OBJECT
#endif

#ifndef Q_PROPERTY
#define Q_PROPERTY(...)
#endif

#ifndef Q_ENUM
#define Q_ENUM(...)
#endif

#ifndef Q_ENUM_NS
#define Q_ENUM_NS(...)
#endif

#ifndef Q_FLAG
#define Q_FLAG(...)
#endif

#ifndef Q_DECLARE_FLAGS
#define Q_DECLARE_FLAGS(Flags, Enum) using Flags = int;
#endif

#ifndef Q_FLAGS
#define Q_FLAGS(...)
#endif

#ifndef Q_INVOKABLE
#define Q_INVOKABLE
#endif

#ifndef QStringLiteral
#define QStringLiteral(str) QString(str)
#endif

#ifndef QByteArrayLiteral
#define QByteArrayLiteral(str) QByteArray(str)
#endif

#ifndef qUtf8Printable
#define qUtf8Printable(str) ((str).toUtf8().constData())
#endif

#ifndef Q_FUNC_INFO
#define Q_FUNC_INFO __PRETTY_FUNCTION__
#endif

#ifndef qobject_cast
#define qobject_cast dynamic_cast
#endif

namespace Qt {
    enum AlignmentFlag { AlignLeft = 1, AlignRight = 2, AlignHCenter = 4, AlignVCenter = 8, AlignCenter = 12, AlignJustify = 16 };
    enum WindowType { Widget = 0, Window = 1, Dialog = 2 };
    enum WindowModality { NonModal = 0, WindowModal = 1, ApplicationModal = 2 };
    enum WidgetAttribute { WA_DeleteOnClose = 55 };
    enum FindChildOption { FindDirectChildrenOnly = 0, FindChildrenRecursively = 1 };
    enum ConnectionType { AutoConnection = 0, DirectConnection = 1, QueuedConnection = 2, UniqueConnection = 0x80 };
    enum CaseSensitivity { CaseInsensitive = 0, CaseSensitive = 1 };
    enum Orientation { Horizontal = 0x1, Vertical = 0x2 };
    enum Key { Key_Escape = 0x01000000, Key_Return = 0x01000004, Key_Enter = 0x01000005 };
    enum SortOrder { AscendingOrder = 0, DescendingOrder = 1 };
    enum EOLMode { EOL_CRLF = 0, EOL_CR = 1, EOL_LF = 2 };
}

class QEvent {
public:
    enum Type { None = 0, ActivationChange = 99, WindowActivate = 24, WindowDeactivate = 25 };
    virtual ~QEvent() = default;
    Type type() const { return None; }
    void accept() {}
    void ignore() {}
};

class QShowEvent : public QEvent {};
class QCloseEvent : public QEvent {};
class QKeyEvent : public QEvent {};
class QMouseEvent : public QEvent {};
class QWheelEvent : public QEvent {};
class QDragEnterEvent : public QEvent {};
class QDropEvent : public QEvent {};

class QByteArray {
    std::string m_data;
public:
    QByteArray() = default;
    QByteArray(const char *s) : m_data(s ? s : "") {}
    QByteArray(const char *s, int len) : m_data(s ? std::string(s, len) : "") {}
    const char *constData() const { return m_data.c_str(); }
    const char *data() const { return m_data.c_str(); }
    int size() const { return static_cast<int>(m_data.size()); }
    int length() const { return size(); }
    bool isEmpty() const { return m_data.empty(); }
    bool isNull() const { return m_data.empty(); }
};

class QString {
    std::string m_str;
public:
    QString() = default;
    QString(const char *s) : m_str(s ? s : "") {}
    QString(const std::string &s) : m_str(s) {}
    static QString fromUtf8(const char *s, int len = -1) { return QString(s ? (len >= 0 ? std::string(s, len) : std::string(s)) : std::string()); }
    static QString fromUtf8(const QByteArray &b) { return QString(b.constData()); }
    static QString fromLatin1(const char *s) { return QString(s); }
    static QString number(long long) { return QString(); }
    static QString number(double, char = 'g', int = 6) { return QString(); }
    bool isEmpty() const { return m_str.empty(); }
    bool isNull() const { return m_str.empty(); }
    int size() const { return static_cast<int>(m_str.size()); }
    int length() const { return size(); }
    QString toLower() const { return *this; }
    QString toUpper() const { return *this; }
    QString trimmed() const { return *this; }
    QByteArray toUtf8() const { return QByteArray(m_str.c_str(), static_cast<int>(m_str.size())); }
    QByteArray toLatin1() const { return toUtf8(); }
    QByteArray toLocal8Bit() const { return toUtf8(); }
    QString &replace(const char *, const char *) { return *this; }
    QString &replace(const QString &, const QString &) { return *this; }
    QString arg(const QString &) const { return *this; }
    QString arg(int) const { return *this; }
    QString arg(long long) const { return *this; }
    bool startsWith(const QString &, int = 0) const { return false; }
    bool endsWith(const QString &, int = 0) const { return false; }
    bool contains(const QString &, int = 0) const { return false; }
    bool operator==(const QString &other) const { return m_str == other.m_str; }
    bool operator!=(const QString &other) const { return m_str != other.m_str; }
    bool operator<(const QString &other) const { return m_str < other.m_str; }
};

class QObject {
public:
    explicit QObject(QObject *parent = nullptr) {}
    virtual ~QObject() = default;
    template <typename T> T findChild(const char* = nullptr, int = 0) const { return nullptr; }
    template <typename T> T findChild(const QString&, int = 0) const { return nullptr; }
    template <typename Sender, typename Signal, typename Receiver, typename Slot>
    static bool connect(Sender*, Signal, Receiver*, Slot, int = 0) { return true; }
    template <typename Sender, typename Signal, typename Slot>
    static bool connect(Sender*, Signal, Slot, int = 0) { return true; }
    template <typename Sender, typename Signal, typename Receiver, typename Slot>
    static bool disconnect(Sender*, Signal, Receiver*, Slot) { return true; }
    template <typename Sender, typename Signal, typename Slot>
    static bool disconnect(Sender*, Signal, Slot) { return true; }
    QObject *sender() const { return nullptr; }
    static QString tr(const char *sourceText, const char * = nullptr, int = -1) { return QString(sourceText); }
};

class QWidget : public QObject {
public:
    explicit QWidget(QWidget *parent = nullptr, int = 0) : QObject(parent) {}
    virtual ~QWidget() = default;
    void show() {}
    void hide() {}
    void close() {}
    void setVisible(bool) {}
    bool isVisible() const { return true; }
    void setEnabled(bool) {}
    void setDisabled(bool) {}
    bool isEnabled() const { return true; }
    bool isActiveWindow() const { return true; }
    void setAttribute(int, bool = true) {}
    void setWindowTitle(const QString &) {}
    void update() {}
    void repaint() {}
};

class QPaintDevice {
public:
    virtual ~QPaintDevice() = default;
};

class QAbstractScrollArea : public QWidget {
public:
    explicit QAbstractScrollArea(QWidget *parent = nullptr) : QWidget(parent) {}
};

class QIODevice : public QObject {
public:
    enum OpenModeFlag { NotOpen = 0x0000, ReadOnly = 0x0001, WriteOnly = 0x0002, ReadWrite = 0x0003, Append = 0x0004, Truncate = 0x0008, Text = 0x0010 };
    explicit QIODevice(QObject *parent = nullptr) : QObject(parent) {}
    virtual bool open(int) { return true; }
    virtual void close() {}
};

class QMimeData : public QObject {
public:
    bool hasUrls() const { return false; }
    std::vector<class QUrl> urls() const { return {}; }
    bool hasText() const { return false; }
    QString text() const { return QString(); }
    QObject *source() const { return nullptr; }
};

class QElapsedTimer {
public:
    void start() {}
    long long elapsed() const { return 0; }
};

class QDialog : public QWidget {
public:
    explicit QDialog(QWidget *parent = nullptr, int = 0) : QWidget(parent) {}
    virtual int exec() { return 0; }
    virtual void accept() {}
    virtual void reject() {}
    virtual void showEvent(QShowEvent *e) {}
    virtual void closeEvent(QCloseEvent *e) {}
    virtual bool event(QEvent *e) { return true; }
};

class QMainWindow : public QWidget {
public:
    explicit QMainWindow(QWidget *parent = nullptr, int = 0) : QWidget(parent) {}
    virtual void changeEvent(QEvent *e) {}
    virtual void closeEvent(QCloseEvent *e) {}
    virtual void dragEnterEvent(QDragEnterEvent *e) {}
    virtual void dropEvent(QDropEvent *e) {}
};

class QAction : public QObject {
public:
    explicit QAction(QObject *parent = nullptr) : QObject(parent) {}
    explicit QAction(const QString &, QObject *parent = nullptr) : QObject(parent) {}
    void setEnabled(bool) {}
    void setDisabled(bool) {}
    void setVisible(bool) {}
    void setChecked(bool) {}
    bool isChecked() const { return false; }
    void setText(const QString &) {}
    void trigger() {}
};

class QMenu : public QWidget {
public:
    explicit QMenu(QWidget *parent = nullptr) : QWidget(parent) {}
    explicit QMenu(const QString &, QWidget *parent = nullptr) : QWidget(parent) {}
    QAction *addAction(const QString &) { return nullptr; }
    void addAction(QAction *) {}
    void addSeparator() {}
    void clear() {}
};

class QMenuBar : public QWidget {
public:
    explicit QMenuBar(QWidget *parent = nullptr) : QWidget(parent) {}
    QMenu *addMenu(const QString &) { return nullptr; }
    void addMenu(QMenu *) {}
};

class QToolBar : public QWidget {
public:
    explicit QToolBar(QWidget *parent = nullptr) : QWidget(parent) {}
    QAction *addAction(const QString &) { return nullptr; }
    void addAction(QAction *) {}
    void addSeparator() {}
};

class QStatusBar : public QWidget {
public:
    explicit QStatusBar(QWidget *parent = nullptr) : QWidget(parent) {}
    void showMessage(const QString &, int = 0) {}
    void clearMessage() {}
};

class QTabBar : public QWidget {
public:
    explicit QTabBar(QWidget *parent = nullptr) : QWidget(parent) {}
    int currentIndex() const { return 0; }
    void setCurrentIndex(int) {}
    int count() const { return 0; }
};

class QPushButton : public QWidget {
public:
    explicit QPushButton(QWidget *parent = nullptr) : QWidget(parent) {}
    explicit QPushButton(const QString &, QWidget *parent = nullptr) : QWidget(parent) {}
};

class QLabel : public QWidget {
public:
    explicit QLabel(QWidget *parent = nullptr) : QWidget(parent) {}
    explicit QLabel(const QString &, QWidget *parent = nullptr) : QWidget(parent) {}
    void setText(const QString &) {}
};

class QLineEdit : public QWidget {
public:
    explicit QLineEdit(QWidget *parent = nullptr) : QWidget(parent) {}
    QString text() const { return QString(); }
    void setText(const QString &) {}
    void selectAll() {}
};

class QComboBox : public QWidget {
public:
    explicit QComboBox(QWidget *parent = nullptr) : QWidget(parent) {}
    QString currentText() const { return QString(); }
    void setCurrentText(const QString &) {}
    QLineEdit *lineEdit() const { return nullptr; }
};

class QCoreApplication : public QObject {
public:
    QCoreApplication(int &, char **) {}
    static QCoreApplication *instance() { return nullptr; }
};

class QGuiApplication : public QCoreApplication {
public:
    QGuiApplication(int &, char **) : QCoreApplication(char_ref, nullptr) {}
    static QGuiApplication *instance() { return nullptr; }
    static class QStyleHints *styleHints() { return nullptr; }
private:
    static int char_ref;
};

class QApplication : public QGuiApplication {
public:
    QApplication(int &argc, char **argv) : QGuiApplication(argc, argv) {}
    static QApplication *instance() { return nullptr; }
};

class QStyleHints : public QObject {
public:
    void *colorSchemeChanged;
};

class QDateTime {
public:
    QDateTime() = default;
    static QDateTime currentDateTime() { return QDateTime(); }
    bool isValid() const { return true; }
    QString toString() const { return QString(); }
    QDateTime addDays(qint64) const { return *this; }
    bool operator<(const QDateTime &) const { return false; }
};

class QFileDevice : public QIODevice {
public:
    enum FileError { NoError = 0, ReadError = 1, WriteError = 2, FatalError = 3, ResourceError = 4, OpenError = 5, AbortError = 6, TimeOutError = 7, UnspecifiedError = 8, RemoveError = 9, RenameError = 10, PositionError = 11, ResizeError = 12, PermissionsError = 13, CopyError = 14 };
};

class QFile : public QFileDevice {
public:
    QFile() = default;
    explicit QFile(const QString &) {}
    bool exists() const { return false; }
    bool open(int) { return true; }
    void close() {}
};

class QFileInfo {
public:
    QFileInfo() = default;
    explicit QFileInfo(const QString &) {}
    bool exists() const { return false; }
    QString absoluteFilePath() const { return QString(); }
    QString fileName() const { return QString(); }
};

class QDir {
public:
    QDir() = default;
    explicit QDir(const QString &) {}
    static QString homePath() { return QString(); }
    static QString currentPath() { return QString(); }
};

class QSettings : public QObject {
public:
    QSettings() = default;
    explicit QSettings(const QString &, const QString & = QString()) {}
    void setValue(const QString &, const QString &) {}
    void beginGroup(const QString &) {}
    void endGroup() {}
};

class QColor {
public:
    QColor() = default;
    explicit QColor(const char *) {}
    explicit QColor(int, int, int, int = 255) {}
    uint32_t rgb() const { return 0; }
    bool isValid() const { return true; }
};

class QPalette {
public:
    enum ColorRole { Window = 0, WindowText = 1, Base = 2, Text = 3, Button = 4, ButtonText = 5, Highlight = 6, HighlightedText = 7 };
    QPalette() = default;
    QColor color(ColorRole) const { return QColor(); }
    void setColor(ColorRole, const QColor &) {}
};

class QFont {
public:
    QFont() = default;
    explicit QFont(const QString &, int = -1, int = -1, bool = false) {}
    void setPointSize(int) {}
    void setBold(bool) {}
};

class QPixmap {
public:
    QPixmap() = default;
    explicit QPixmap(const QString &) {}
    bool isNull() const { return true; }
};

class QIcon {
public:
    QIcon() = default;
    explicit QIcon(const QString &) {}
    explicit QIcon(const QPixmap &) {}
};

class QPoint {
public:
    int x = 0;
    int y = 0;
    QPoint() = default;
    QPoint(int x_, int y_) : x(x_), y(y_) {}
};

class QSize {
public:
    int width = 0;
    int height = 0;
    QSize() = default;
    QSize(int w, int h) : width(w), height(h) {}
};

class QRect {
public:
    QRect() = default;
    QRect(int, int, int, int) {}
};

template <typename T>
class QVector : public std::vector<T> {
public:
    using std::vector<T>::vector;
    void append(const T &val) { this->push_back(val); }
    int count() const { return static_cast<int>(this->size()); }
    bool isEmpty() const { return this->empty(); }
    const T &first() const { return this->front(); }
    const T &last() const { return this->back(); }
};

template <typename T>
class QList : public QVector<T> {
    using QVector<T>::QVector;
};

class QStringList : public QVector<QString> {
public:
    using QVector<QString>::QVector;
    QString join(const QString &) const { return QString(); }
    QStringList &operator<<(const QString &s) { this->push_back(s); return *this; }
};

template <typename Key, typename T>
class QMap : public std::map<Key, T> {};

template <typename Key, typename T>
class QHash : public std::unordered_map<Key, T> {};

template <typename T>
class QSet : public std::set<T> {};

template <typename T>
class QScopedPointer {
    std::unique_ptr<T> m_ptr;
public:
    QScopedPointer(T *p = nullptr) : m_ptr(p) {}
    T *data() const { return m_ptr.get(); }
    T *get() const { return m_ptr.get(); }
    T *operator->() const { return m_ptr.get(); }
    T &operator*() const { return *m_ptr; }
    void reset(T *p = nullptr) { m_ptr.reset(p); }
    bool isNull() const { return !m_ptr; }
};

template <typename T>
class QPointer {
    T *m_ptr = nullptr;
public:
    QPointer(T *p = nullptr) : m_ptr(p) {}
    T *data() const { return m_ptr; }
    T *operator->() const { return m_ptr; }
    T &operator*() const { return *m_ptr; }
    operator T*() const { return m_ptr; }
    bool isNull() const { return m_ptr == nullptr; }
};

class QMetaEnum {
public:
    template <typename T>
    static QMetaEnum fromType() { return QMetaEnum(); }
    const char *key(int) const { return ""; }
    int value(int) const { return 0; }
    int keyCount() const { return 0; }
};

class QDebug {
public:
    template <typename T>
    QDebug &operator<<(const T &) { return *this; }
};

inline QDebug qInfo() { return QDebug(); }
inline QDebug qDebug() { return QDebug(); }
inline QDebug qWarning() { return QDebug(); }
inline QDebug qCritical() { return QDebug(); }
inline void qInfo(const char *, ...) {}
inline void qDebug(const char *, ...) {}
inline void qWarning(const char *, ...) {}
inline void qCritical(const char *, ...) {}
"""

    base_header.write_text(base_content, encoding="utf-8")

    # List of all Qt classes to generate stub include files for
    headers = [
        "QAbstractItemModel",
        "QAbstractItemView",
        "QAbstractScrollArea",
        "QAbstractTableModel",
        "QAction",
        "QActionGroup",
        "QApplication",
        "QBitArray",
        "QButtonGroup",
        "QByteArray",
        "QByteArrayView",
        "QCheckBox",
        "QClipboard",
        "QColor",
        "QColorDialog",
        "QComboBox",
        "QCommandLineParser",
        "QCoreApplication",
        "QDataStream",
        "QDateTime",
        "QDebug",
        "QDesktopServices",
        "QDialog",
        "QDialogButtonBox",
        "QDir",
        "QDirIterator",
        "QDockWidget",
        "QElapsedTimer",
        "QEvent",
        "QFile",
        "QFileDevice",
        "QFileDialog",
        "QFileInfo",
        "QFileSystemModel",
        "QFileSystemWatcher",
        "QFont",
        "QFontDatabase",
        "QFormLayout",
        "QFrame",
        "QIODevice",
        "QMimeData",
        "QPaintDevice",
        "QGraphicsOpacityEffect",
        "QGroupBox",
        "QGuiApplication",
        "QHBoxLayout",
        "QHash",
        "QHeaderView",
        "QInputDialog",
        "QItemSelectionModel",
        "QJsonArray",
        "QJsonDocument",
        "QJsonObject",
        "QJsonParseError",
        "QKeyEvent",
        "QKeySequence",
        "QLabel",
        "QLineEdit",
        "QList",
        "QListWidget",
        "QListWidgetItem",
        "QLocale",
        "QMainWindow",
        "QMap",
        "QMenu",
        "QMessageBox",
        "QMetaEnum",
        "QModelIndex",
        "QMouseEvent",
        "QObject",
        "QPainter",
        "QPalette",
        "QPixmap",
        "QPointer",
        "QPrintPreviewDialog",
        "QPrinter",
        "QProcess",
        "QPropertyAnimation",
        "QPushButton",
        "QRadioButton",
        "QRegularExpression",
        "QRegularExpressionMatch",
        "QSaveFile",
        "QScreen",
        "QScrollBar",
        "QSet",
        "QSettings",
        "QShortcut",
        "QSpinBox",
        "QStandardPaths",
        "QStatusBar",
        "QString",
        "QStringDecoder",
        "QStringEncoder",
        "QStringList",
        "QStringListModel",
        "QStyle",
        "QStyleHints",
        "QStyledItemDelegate",
        "QSysInfo",
        "QTabBar",
        "QTabWidget",
        "QTableWidget",
        "QTextBoundaryFinder",
        "QTextBrowser",
        "QTextCodec",
        "QTextStream",
        "QTimer",
        "QToolBar",
        "QToolButton",
        "QTranslator",
        "QTreeWidgetItem",
        "QUrl",
        "QUuid",
        "QVBoxLayout",
        "QVector",
        "QWheelEvent",
        "QWidget",
        "QWindow",
        "QXmlStreamReader",
        "QXmlStreamWriter",
        "Qt",
        "QtGlobal",
        "QtMessageHandler",
    ]

    include_stub = '#pragma once\n#include "qt_shim_base.h"\n'

    for h in headers:
        header_file = shims_dir / h
        header_file.write_text(include_stub, encoding="utf-8")

    # Namespaced headers
    sub_headers = {
        qt_core_dir: [
            "QVariant",
            "QObject",
            "QString",
            "QByteArray",
            "QDateTime",
            "QFile",
            "QFileInfo",
            "QDir",
            "qglobal.h",
        ],
        qt_gui_dir: [
            "QAction",
            "QColor",
            "QFont",
            "QIcon",
            "QPainter",
            "QPalette",
            "QPixmap",
        ],
        qt_widgets_dir: [
            "QApplication",
            "QMainWindow",
            "QDialog",
            "QWidget",
            "QMenu",
            "QMenuBar",
            "QToolBar",
            "QPushButton",
            "QStatusBar",
            "QTabBar",
            "QLineEdit",
            "QComboBox",
        ],
        qt_print_dir: ["QPrinter", "QPrintPreviewDialog"],
    }

    sub_include_stub = '#pragma once\n#include "../qt_shim_base.h"\n'
    for sdir, file_list in sub_headers.items():
        for f in file_list:
            (sdir / f).write_text(sub_include_stub, encoding="utf-8")

    print(f"Generated {len(headers)} Qt header stubs in {shims_dir}")


if __name__ == "__main__":
    main()
