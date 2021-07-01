#include "performancestatisticsform.h"
#include "ui_performancestatisticsform.h"
#include "common/cappenv.hpp"
#include "DMSNavigation.h"
#include <QLabel>
#include <QClipboard>
#include <QFileInfo>
#include <QSvgGenerator>
#include <QFileDialog>
#include <QPrinter>
//#include <QtCharts/QChartGlobal>
//#include <QtCharts/QBarSeries>
//#include <QtCharts/QLegend>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QPercentBarSeries>

PerformanceStatisticsForm::PerformanceStatisticsForm(SPerformanceStatistics *perform, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PerformanceStatisticsForm),
    m_perform(perform)
{
    ui->setupUi(this);

    m_dmsNavigation = new DMSNavigation(this);
    ui->gridLayout->addWidget(m_dmsNavigation);

    initRcStatistics();

    QLabel *label1 = new QLabel("敬请期待");
    label1->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    QLabel *label2 = new QLabel("敬请期待");
    label2->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    m_dmsNavigation->addTab(label1, tr("Mem使用率"));
    m_dmsNavigation->addTab(label2, tr("周期分析"));

    resize(geometry().size() * CAppEnv::m_dpiFactor);
}

PerformanceStatisticsForm::~PerformanceStatisticsForm()
{
    delete ui;
}


void PerformanceStatisticsForm::initRcStatistics()
{
    m_barSet[0] = new QBarSet("AU");
    m_barSet[1] = new QBarSet("SU");
    m_barSet[2] = new QBarSet("LU");
    m_barSet[3] = new QBarSet("SBOX");
    m_barSet[4] = new QBarSet("BENES");
    m_barSet[5] = new QBarSet("UNUSED");
    QStringList categories;
    for(int i = 0; i < m_perform->xmlPerformList.count(); ++i)
    {
        SXMLPerformanceStatistics performance = m_perform->xmlPerformList.at(i);
        categories << QFileInfo(performance.xmlName).completeBaseName();

        *m_barSet[0] << performance.auUsedCount;
        *m_barSet[1] << performance.suUsedCount;
        *m_barSet[2] << performance.luUsedCount;
        *m_barSet[3] << performance.tuUsedCount;
        *m_barSet[4] << performance.puUsedCount;
        *m_barSet[5] << TOTALRCCOUNT - performance.auUsedCount - performance.luUsedCount
                 - performance.suUsedCount - performance.tuUsedCount - performance.puUsedCount;
    }

    QPercentBarSeries *series = new QPercentBarSeries();
    connect(series, SIGNAL(hovered(bool,int,QBarSet*)), this, SLOT(handlePercentBarSeriesHovered(bool,int,QBarSet*)));
    connect(series, SIGNAL(clicked(int,QBarSet*)), this, SLOT(handlePercentBarSeriesClicked(int,QBarSet*)));
    series->append(m_barSet[0]);
    series->append(m_barSet[1]);
    series->append(m_barSet[2]);
    series->append(m_barSet[3]);
    series->append(m_barSet[4]);
    series->append(m_barSet[5]);

    QChart *chart = new QChart();
    chart->setTheme(QChart::ChartThemeBlueCerulean);
    chart->addSeries(series);
    chart->setTitle(tr("算子使用率"));
    chart->setTitleFont(QFont("Arial", 18));
    chart->setAnimationOptions(QChart::SeriesAnimations);

    QBarCategoryAxis *axis = new QBarCategoryAxis();
    axis->append(categories);
    chart->createDefaultAxes();
    chart->setAxisX(axis, series);

    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignBottom);

    m_rcChartView = new QChartView(chart, this);
    m_rcChartView->setRenderHint(QPainter::Antialiasing);
    m_rcChartView->setToolTipDuration(-1);

    m_dmsNavigation->addTab(m_rcChartView, tr("算子使用率"));
}

void PerformanceStatisticsForm::handlePercentBarSeriesHovered(bool status, int index, QBarSet *barSet)
{
    if(status)
        m_rcChartView->setToolTip(QString::number(barSet->at(index)));
    else
        m_rcChartView->setToolTip("");
}

void PerformanceStatisticsForm::handlePercentBarSeriesClicked(int index, QBarSet *barSet)
{
    Q_UNUSED(barSet)
    m_rcChart = new QChart;
    m_rcChart->setTheme(QChart::ChartThemeBlueCerulean);
    m_rcChart->legend()->setAlignment(Qt::AlignRight);
    m_rcChart->setTitle(tr("%0算子使用率").arg(QFileInfo(m_perform->xmlPerformList.at(index).xmlName).completeBaseName()));
    m_rcChart->setTitleFont(QFont("Arial", 18));
    m_rcChart->setAnimationOptions(QChart::AllAnimations);

    int au, lu, su, pu, tu, unused;
    SXMLPerformanceStatistics performance = m_perform->xmlPerformList.at(index);
    au = performance.auUsedCount;
    su = performance.suUsedCount;
    lu = performance.luUsedCount;
    tu = performance.tuUsedCount;
    pu = performance.puUsedCount;
    unused = TOTALRCCOUNT - performance.auUsedCount - performance.luUsedCount
            - performance.suUsedCount - performance.tuUsedCount - performance.puUsedCount;

    // create series
    QPieSeries *series = new QPieSeries();
    *series << new QtCharts::QPieSlice("AU", au);
    *series << new QtCharts::QPieSlice("SU", su);
    *series << new QtCharts::QPieSlice("LU", lu);
    *series << new QtCharts::QPieSlice("SBOX", tu);
    *series << new QtCharts::QPieSlice("BENES", pu);
    *series << new QtCharts::QPieSlice("UNUSED:", unused);

    series->setLabelsVisible();
    m_rcChart->addSeries(series);

    for(int i = 0; i < series->count(); ++i)
    {
        QPieSlice *slice = series->slices().at(i);
        QString label = slice->label();
        label += ":" + QString::number(slice->value()) + ", ";
        label += QString::number(slice->percentage() * 100, 'f', 1);
        label += "%";
        slice->setLabel(label);
        slice->setLabelVisible(slice->value());
        slice->setBrush(m_barSet[i]->brush());
        connect(slice, SIGNAL(hovered(bool)), this, SLOT(handlePieSliceHovered(bool)));
    }

    QChartView *chartView = new QChartView(this);
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->setChart(m_rcChart);

//    connect(m_series, SIGNAL(clicked(QPieSlice*)), this, SLOT(handleSliceClicked(QPieSlice*)));


    QDialog dlg(this);
    dlg.setWindowTitle(tr("算法统计评估"));
    QGridLayout *layout = new QGridLayout();
    layout->setMargin(0);
    layout->addWidget(chartView);
    dlg.setLayout(layout);
    dlg.resize(this->size());
    dlg.move(this->x(),this->y());
    dlg.exec();
}

void PerformanceStatisticsForm::handlePieSliceHovered(bool flag)
{
    QPieSlice *pieSlice = qobject_cast<QPieSlice *>(sender());
    if(pieSlice)
    {
        pieSlice->setExploded(flag);
    }
}

void PerformanceStatisticsForm::keyPressEvent(QKeyEvent *event)
{
    if(event->modifiers() == Qt::ControlModifier && event->key() == Qt::Key_S)
    {
        QString fileName = QFileDialog::getSaveFileName(this, tr("保存"), "", tr(u8"SVG 文件 (*.svg)"));
        if(fileName.isEmpty())
        {
            event->accept();
            return;
        }
        QSvgGenerator svg;
        QPrinter printer(QPrinter::HighResolution);
        svg.setFileName(fileName);
        svg.setSize(printer.paperSize(QPrinter::Point).toSize());
        QPainter painter(&svg);
        m_rcChart->scene()->render(&painter);
        CAppEnv::showHintMessage(tr(u8"已保存到文件"), this);
        event->accept();
        return;
    }
    else if(event->modifiers() == Qt::ControlModifier && event->key() == Qt::Key_C)
    {
        QString str = "XML\\PE\tAU\tSU\tLU\tSBOX\tBENES\tUNUSED\n";
        foreach (const SXMLPerformanceStatistics &xmlPerfomance, m_perform->xmlPerformList) {
            str += xmlPerfomance.xmlName + "\t" +
                    QString::number(xmlPerfomance.auUsedCount) + "\t" +
                    QString::number(xmlPerfomance.suUsedCount) + "\t" +
                    QString::number(xmlPerfomance.luUsedCount) + "\t" +
                    QString::number(xmlPerfomance.tuUsedCount) + "\t" +
                    QString::number(xmlPerfomance.puUsedCount) + "\t" +
                    QString::number(TOTALRCCOUNT - xmlPerfomance.auUsedCount - xmlPerfomance.luUsedCount
                                    - xmlPerfomance.suUsedCount - xmlPerfomance.tuUsedCount - xmlPerfomance.puUsedCount) + "\n";
        }
        QApplication::clipboard()->setText(str);
        CAppEnv::showHintMessage(tr(u8"已复制到剪贴板"), this);
        event->accept();
        return;
    }
    QDialog::keyPressEvent(event);
}

