#ifndef PERFORMANCESTATISTICSFORM_H
#define PERFORMANCESTATISTICSFORM_H

#include <QDialog>
#include <QtCharts/QChart>
#include <QtCharts/QBarSet>
#include <QtCharts/QChartView>
#include <QtCharts/QPieSeries>
#include "../datafile.h"

class DMSNavigation;

QT_CHARTS_USE_NAMESPACE

namespace Ui {
class PerformanceStatisticsForm;
}

class PerformanceStatisticsForm : public QDialog
{
    Q_OBJECT

public:
    explicit PerformanceStatisticsForm(SPerformanceStatistics *perform, QWidget *parent = 0);
    ~PerformanceStatisticsForm();

public slots:
    void handlePercentBarSeriesHovered(bool status,int index, QBarSet* barSet);
    void handlePercentBarSeriesClicked(int index, QBarSet* barSet);
    void handlePieSliceHovered(bool flag);

protected:
    void keyPressEvent(QKeyEvent *event);

private:
    Ui::PerformanceStatisticsForm *ui;
    DMSNavigation *m_dmsNavigation;

    SPerformanceStatistics *m_perform;
    QChart *m_rcChart;

    QChartView *m_rcChartView = nullptr;
    QBarSet *m_barSet[6];
    QPieSeries *m_cycleSeries = nullptr;

    void initRcStatistics();
};

#endif // PERFORMANCESTATISTICSFORM_H
