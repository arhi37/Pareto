#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui>
#include <qwt_plot.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_curve.h>
#include <qwt_symbol.h>
#include <qwt_plot_marker.h>


namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

	bool animRun;

	QString dirStr;
	QVector<double>	data;

	qint32 paretoId;
	qint32 lastPos;

	double *paretoX, *paretoY;
	int paretoSize;

	QwtPlot		plot;
	QwtPlotGrid		grid;
	QwtPlotCurve	curve;
//	QVector<double>	xLogAmpl, yLogAmpl;

	QwtSymbol	symbol;
	QwtPlotMarker marker;

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
	void on_action_Exit_triggered();

	void on_action_Open_triggered();

	void on_sliderPos_valueChanged(int value);

	void on_pushButtonCalc_clicked();

	void on_pushButtonStop_clicked();

	void on_pushButtonRun_clicked();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
