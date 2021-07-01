#ifndef CPROJECTSETTINGWIDGET_H
#define CPROJECTSETTINGWIDGET_H

#include <QWidget>

class CProjectSettingWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CProjectSettingWidget(QWidget *parent = nullptr);

signals:
    void save();

public slots:

protected:
    void keyPressEvent(QKeyEvent *event);
};

#endif // CPROJECTSETTINGWIDGET_H
