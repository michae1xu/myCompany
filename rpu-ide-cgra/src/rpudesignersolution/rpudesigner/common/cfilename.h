#ifndef CFILENAME_H
#define CFILENAME_H

#include <QString>
#include <QFileInfo>

class CFileName : private QString
{
public:
    CFileName();
    explicit CFileName(const QFileInfo &info);
    QFileInfo toFileInfo() const;
    static CFileName fromString(const QString &filename);
    static CFileName fromString(const QString &filename, const QString &defaultExtension);
    static CFileName fromLatin1(const QByteArray &filename);
    static CFileName fromUserInput(const QString &filename);
    static CFileName fromUtf8(const char *filename, int filenameSize = -1);
    const QString &toString() const;
    QString toUserOutput() const;
    QString fileName(int pathComponents = 0) const;
    bool exists() const;

    CFileName parentDir() const;

    bool operator==(const CFileName &other) const;
    bool operator!=(const CFileName &other) const;
    bool operator<(const CFileName &other) const;
    bool operator<=(const CFileName &other) const;
    bool operator>(const CFileName &other) const;
    bool operator>=(const CFileName &other) const;

    bool isChildOf(const CFileName &s) const;
    bool isChildOf(const QDir &dir) const;
    bool endsWith(const QString &s) const;

    CFileName relativeChildPath(const CFileName &parent) const;
    CFileName &appendPath(const QString &s);
    CFileName &appendString(const QString &str);
    CFileName &appendString(QChar str);

    using QString::size;
    using QString::count;
    using QString::length;
    using QString::isEmpty;
    using QString::isNull;
    using QString::clear;
private:
    CFileName(const QString &string);
};

#endif // CFILENAME_H
