#ifndef CBENESFORMATCONVERT_H
#define CBENESFORMATCONVERT_H

#include <QList>

class CBenesFormatConvert
{
public:
    CBenesFormatConvert();
    ~CBenesFormatConvert();

    void setValue(QList<uint> *valueList);
    int** getConfig() const {return config;}
    void convert128To26(int level, int n);
    void convert26To128();

private:
    int **x;
    int **config;
//    int Output[];

    void sort(int *a, int n);
};

#endif // CBENESFORMATCONVERT_H
