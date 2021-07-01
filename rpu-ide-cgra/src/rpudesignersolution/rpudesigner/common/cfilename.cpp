#include <QDir>
#include <common/cappenv.hpp>
#include "cfilename.h"


/*! \class Utils::CFileName

    \brief The CFileName class is a light-weight convenience class for filenames.

    On windows filenames are compared case insensitively.
*/

CFileName::CFileName()
    : QString()
{

}

/// Constructs a CFileName from \a info
CFileName::CFileName(const QFileInfo &info)
    : QString(info.absoluteFilePath())
{
}

/// \returns a QFileInfo
QFileInfo CFileName::toFileInfo() const
{
    return QFileInfo(*this);
}

/// \returns a QString for passing on to QString based APIs
const QString &CFileName::toString() const
{
    return *this;
}

/// \returns a QString to display to the user
/// Converts the separators to the native format
QString CFileName::toUserOutput() const
{
    return QDir::toNativeSeparators(toString());
}

QString CFileName::fileName(int pathComponents) const
{
    if (pathComponents < 0)
        return *this;
    const QChar slash = QLatin1Char('/');
    Q_ASSERT(!endsWith(slash));
    int i = lastIndexOf(slash);
    if (pathComponents == 0 || i == -1)
        return mid(i + 1);
    int component = i + 1;
    // skip adjacent slashes
    while (i > 0 && at(--i) == slash);
    while (i >= 0 && --pathComponents >= 0) {
        i = lastIndexOf(slash, i);
        component = i + 1;
        while (i > 0 && at(--i) == slash);
    }

    // If there are no more slashes before the found one, return the entire string
    if (i > 0 && lastIndexOf(slash, i) != -1)
        return mid(component);
    return *this;
}

/// \returns a bool indicating whether a file with this
/// CFileName exists.
bool CFileName::exists() const
{
    return !isEmpty() && QFileInfo::exists(*this);
}

/// Find the parent directory of a given directory.

/// Returns an empty CFileName if the current directory is already
/// a root level directory.

/// \returns \a CFileName with the last segment removed.
CFileName CFileName::parentDir() const
{
    const QString basePath = toString();
    if (basePath.isEmpty())
        return CFileName();

    const QDir base(basePath);
    if (base.isRoot())
        return CFileName();

    const QString path = basePath + QLatin1String("/..");
    const QString parent = QDir::cleanPath(path);

    return CFileName::fromString(parent);
}

/// Constructs a CFileName from \a filename
/// \a filename is not checked for validity.
CFileName CFileName::fromString(const QString &filename)
{
    return CFileName(filename);
}

/// Constructs a CFileName from \a fileName. The \a defaultExtension is appended
/// to \a filename if that does not have an extension already.
/// \a fileName is not checked for validity.
CFileName CFileName::fromString(const QString &filename, const QString &defaultExtension)
{
    if (filename.isEmpty() || defaultExtension.isEmpty())
        return filename;

    QString rc = filename;
    QFileInfo fi(filename);
    // Add extension unless user specified something else
    const QChar dot = QLatin1Char('.');
    if (!fi.fileName().contains(dot)) {
        if (!defaultExtension.startsWith(dot))
            rc += dot;
        rc += defaultExtension;
    }
    return rc;
}

/// Constructs a CFileName from \a fileName
/// \a fileName is not checked for validity.
CFileName CFileName::fromLatin1(const QByteArray &filename)
{
    return CFileName(QString::fromLatin1(filename));
}

/// Constructs a CFileName from \a fileName
/// \a fileName is only passed through QDir::cleanPath
CFileName CFileName::fromUserInput(const QString &filename)
{
    QString clean = QDir::cleanPath(filename);
    if (clean.startsWith(QLatin1String("~/")))
        clean = QDir::homePath() + clean.mid(1);
    return CFileName(clean);
}

/// Constructs a CFileName from \a fileName, which is encoded as UTF-8.
/// \a fileName is not checked for validity.
CFileName CFileName::fromUtf8(const char *filename, int filenameSize)
{
    return CFileName(QString::fromUtf8(filename, filenameSize));
}

CFileName::CFileName(const QString &string)
    : QString(string)
{

}

bool CFileName::operator==(const CFileName &other) const
{
    return QString::compare(*this, other, CAppEnv::m_caseSensitivity) == 0;
}

bool CFileName::operator!=(const CFileName &other) const
{
    return !(*this == other);
}

bool CFileName::operator<(const CFileName &other) const
{
    return QString::compare(*this, other, CAppEnv::m_caseSensitivity) < 0;
}

bool CFileName::operator<=(const CFileName &other) const
{
    return QString::compare(*this, other, CAppEnv::m_caseSensitivity) <= 0;
}

bool CFileName::operator>(const CFileName &other) const
{
    return other < *this;
}

bool CFileName::operator>=(const CFileName &other) const
{
    return other <= *this;
}

/// \returns whether CFileName is a child of \a s
bool CFileName::isChildOf(const CFileName &s) const
{
    if (s.isEmpty())
        return false;
    if (!QString::startsWith(s, CAppEnv::m_caseSensitivity))
        return false;
    if (size() <= s.size())
        return false;
    // s is root, '/' was already tested in startsWith
    if (s.QString::endsWith(QLatin1Char('/')))
        return true;
    // s is a directory, next character should be '/' (/tmpdir is NOT a child of /tmp)
    return at(s.size()) == QLatin1Char('/');
}

/// \overload
bool CFileName::isChildOf(const QDir &dir) const
{
    return isChildOf(CFileName::fromString(dir.absolutePath()));
}

/// \returns whether CFileName endsWith \a s
bool CFileName::endsWith(const QString &s) const
{
    return QString::endsWith(s, CAppEnv::m_caseSensitivity);
}

/// \returns the relativeChildPath of CFileName to parent if CFileName is a child of parent
/// \note returns a empty CFileName if CFileName is not a child of parent
/// That is, this never returns a path starting with "../"
CFileName CFileName::relativeChildPath(const CFileName &parent) const
{
    if (!isChildOf(parent))
        return CFileName();
    return CFileName(QString::mid(parent.size() + 1, -1));
}

/// Appends \a s, ensuring a / between the parts
CFileName &CFileName::appendPath(const QString &s)
{
    if (s.isEmpty())
        return *this;
    if (!isEmpty() && !QString::endsWith(QLatin1Char('/')))
        appendString(QLatin1Char('/'));
    appendString(s);
    return *this;
}

CFileName &CFileName::appendString(const QString &str)
{
    QString::append(str);
    return *this;
}

CFileName &CFileName::appendString(QChar str)
{
    QString::append(str);
    return *this;
}

QTextStream &operator<<(QTextStream &s, const CFileName &fn)
{
    return s << fn.toString();
}
