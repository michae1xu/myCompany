#ifndef DEBUG_H
#define DEBUG_H

#include <QDebug>
#include <QTextStreamFunction>
#include <QTextStreamManipulator>
QT_FORWARD_DECLARE_CLASS(QPainterPath)

enum class LogType {
    Reset = 0,

    Bold,
    Unbold,

    FrontBlack,
    FrontRed,
    FrontGreen,
    FrontYellow,
    FrontBlue,
    FrontPurple,
    FrontCyan,
    FrontWhite,
    BackBlack,
    BackRed,
    BackGreen,
    BackYellow,
    BackBlue,
    BackPurple,
    BackCyan,
    BackWhite,

    TypeCount
};

static const char* logCommands[] = {
    "\033[0m",
    "\033[1m",
    "\033[2m",
    "\033[30m",
    "\033[31m",
    "\033[32m",
    "\033[33m",
    "\033[34m",
    "\033[35m",
    "\033[36m",
    "\033[37m",
    "\033[40m",
    "\033[41m",
    "\033[42m",
    "\033[43m",
    "\033[44m",
    "\033[45m",
    "\033[46m",
    "\033[47m",
};

template <typename EnumType, typename IntType = int> int enumToInt(EnumType enumValue)
{
    static_assert(std::is_enum<EnumType>::value, "EnumType must be enum");
    return static_cast<IntType>(enumValue);
}

class CNoDebug {
public:
    CNoDebug() = default;
    inline CNoDebug& operator<<(const QStringList&) { return *this; }
    inline CNoDebug& operator<<(const QPoint&) { return *this; }
    inline CNoDebug& operator<<(const QPointF&) { return *this; }
    inline CNoDebug& operator<<(const QRect&) { return *this; }
    inline CNoDebug& operator<<(const QRectF&) { return *this; }
    inline CNoDebug& operator<<(const QDateTime&) { return *this; }
    inline CNoDebug& operator<<(const QPainterPath&) { return *this; }
    inline CNoDebug& operator<<(const Qt::MouseButton&) { return *this; }
    inline CNoDebug& operator<<(const QVariant&) { return *this; }
    inline CNoDebug& operator<<(const Qt::KeyboardModifiers&) { return *this; }
    inline CNoDebug& operator<<(const QList<int>*) { return *this; }
    inline CNoDebug& operator<<(QChar) { return *this; }
    inline CNoDebug& operator<<(bool) { return *this; }
    inline CNoDebug& operator<<(char) { return *this; }
    inline CNoDebug& operator<<(signed short) { return *this; }
    inline CNoDebug& operator<<(unsigned short) { return *this; }
#ifdef Q_COMPILER_UNICODE_STRINGS
    inline CNoDebug& operator<<(char16_t) { return *this; }
    inline CNoDebug& operator<<(char32_t) { return *this; }
#endif
    inline CNoDebug& operator<<(signed int)
    {
        return *this;
        ;
    }
    inline CNoDebug& operator<<(unsigned int) { return *this; }
    inline CNoDebug& operator<<(signed long) { return *this; }
    inline CNoDebug& operator<<(unsigned long) { return *this; }
    inline CNoDebug& operator<<(qint64) { return *this; }
    inline CNoDebug& operator<<(quint64) { return *this; }
    inline CNoDebug& operator<<(float) { return *this; }
    inline CNoDebug& operator<<(double) { return *this; }
    inline CNoDebug& operator<<(const char*) { return *this; }
    inline CNoDebug& operator<<(const QString&) { return *this; }
    inline CNoDebug& operator<<(const QStringRef&) { return *this; }
    inline CNoDebug& operator<<(QLatin1String) { return *this; }
    inline CNoDebug& operator<<(const QByteArray&) { return *this; }
    inline CNoDebug& operator<<(const void*) { return *this; }
#ifdef Q_COMPILER_NULLPTR
    inline CNoDebug& operator<<(std::nullptr_t) { return *this; }
#endif
    inline CNoDebug& operator<<(QTextStreamFunction) { return *this; }
    inline CNoDebug& operator<<(QTextStreamManipulator) { return *this; }
};
CNoDebug& noDebug();

template <class T> CNoDebug& operator<<(CNoDebug& noDebug, const QList<T>&) { return noDebug; }
template <class T, class Alloc> CNoDebug& operator<<(CNoDebug& noDebug, const std::list<T, Alloc>&)
{
    return noDebug;
}
template <class T> CNoDebug& operator<<(CNoDebug& noDebug, const QVector<T>&) { return noDebug; }
template <class T, class Alloc>
CNoDebug& operator<<(CNoDebug& noDebug, const std::vector<T, Alloc>&)
{
    return noDebug;
}
template <class T> CNoDebug& operator<<(CNoDebug& noDebug, const QSet<T>&) { return noDebug; }
template <class Key, class T> CNoDebug& operator<<(CNoDebug& noDebug, const QMap<Key, T>&)
{
    return noDebug;
}
template <class Key, class T, class Compare, class Alloc>
CNoDebug& operator<<(CNoDebug& noDebug, const std::map<Key, T, Compare, Alloc>&)
{
    return noDebug;
}
template <class Key, class T, class Compare, class Alloc>
CNoDebug& operator<<(CNoDebug& noDebug, const std::multimap<Key, T, Compare, Alloc>&)
{
    return noDebug;
}
template <class Key, class T> CNoDebug& operator<<(CNoDebug& noDebug, const QHash<Key, T>&)
{
    return noDebug;
}
template <class T1, class T2> CNoDebug& operator<<(CNoDebug& noDebug, const QPair<T1, T2>&)
{
    return noDebug;
}
template <class T> CNoDebug& operator<<(CNoDebug& noDebug, const QFlags<T>&) { return noDebug; }

#ifndef ZJ_NO_DEBUG_OUTPUT // rpudesigner.pro中注释后可开启/关闭日志
#define DBG                                                                                        \
    qDebug() << QString("[DBG] <FILE>%1 <FUNC>%2() <LINE>%3 ==>")                                  \
                    .arg(__FILE__)                                                                 \
                    .arg(__FUNCTION__)                                                             \
                    .arg(__LINE__)
#else
#define DBG noDebug()
#endif // ZJ_NO_DEBUG_OUTPUT

#endif // DEBUG_H
