#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "../LabVIEWgrad/clibs/pareto/pareto.h"
#include "../LabVIEWgrad/clibs/BreakPoint/breakpoint.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
	dirStr="";

	plot.setCanvasBackground(Qt::white);
	plot.setAxisTitle(QwtPlot::xBottom, tr("Десятичный логарифм нормированных данных"));
	plot.setAxisTitle(QwtPlot::yLeft, tr("Десятичный логарифм порядкового номера"));
	grid.enableXMin(true);
	grid.enableYMin(true);
	grid.setMajPen(QPen(Qt::black, 0, Qt::DotLine));
	grid.setMinPen(QPen(Qt::gray, 0, Qt::DotLine));
	grid.attach(&plot);
	curve.setStyle(QwtPlotCurve::NoCurve);
	symbol.setStyle(QwtSymbol::Rect);
	symbol.setSize(1);
	symbol.setBrush(Qt::blue);
	symbol.setPen(QPen(Qt::blue));
	curve.setSymbol(symbol);
	curve.setPen(QPen(Qt::blue));
	curve.attach(&plot);
	plot.setAutoReplot(false);

	marker.setLinePen(QPen(Qt::red));
	marker.setLineStyle(QwtPlotMarker::VLine);
	marker.attach(&plot);

	((QVBoxLayout*)(ui->centralWidget->layout()))->insertWidget(0, &plot, 1);

	paretoX=0;
	paretoY=0;
	lastPos=0;

	animRun=false;

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_action_Exit_triggered()
{
	close();
}

void MainWindow::on_action_Open_triggered()
{
	QString fileName=QFileDialog::getOpenFileName(this, "", dirStr, "*.txt");
	if(fileName.isEmpty())return;

	dirStr=QFileInfo(fileName).path();

	QFile file(fileName);

	if(!file.open(QIODevice::ReadOnly))return;

	data.clear();
	QTextStream in(&file);
	QString str;

	str=in.readLine();
	while(!str.isNull()){
		data<<str.toDouble();
		str=in.readLine();
	}

	if(data.size()<10){
		data.clear();
		return;
	}

	lastPos=0;

	removePareto(paretoId);
	paretoId=initPareto();

	ui->sliderPos->setRange(10, data.size());
	ui->sliderPos->setValue(data.size());
}

void MainWindow::on_sliderPos_valueChanged(int value)
{
	if(data.size()<value)return;

	delete paretoX;
	delete paretoY;

	paretoSize=ui->sliderPos->value();

	paretoX=new double[paretoSize];
	paretoY=new double[paretoSize];

	if(lastPos>paretoSize){
		lastPos=0;
		resetData(paretoId);
	}

	if(!addData(paretoId, data.data()+lastPos, paretoSize-lastPos)){
		paretoSize=0;
		return;
	}

	lastPos=paretoSize;

	if(!getPareto(paretoId, paretoX, paretoY, paretoSize)){
		paretoSize=0;
		return;
	}



	ui->doubleSpinBoxMaxSearch->setRange(paretoX[0], paretoX[paretoSize-1]);
	ui->doubleSpinBoxMaxUse->setRange(paretoX[0], paretoX[paretoSize-1]);
	ui->doubleSpinBoxMinSearch->setRange(paretoX[0], paretoX[paretoSize-1]);
	ui->doubleSpinBoxMinUse->setRange(paretoX[0], paretoX[paretoSize-1]);

	curve.setData(paretoX, paretoY, paretoSize);

	marker.detach();

	plot.replot();
}

void MainWindow::on_pushButtonCalc_clicked()
{
	double sMin, sMax, uMin, uMax;
	sMin=ui->doubleSpinBoxMinSearch->value();
	sMax=ui->doubleSpinBoxMaxSearch->value();
	uMin=ui->doubleSpinBoxMinUse->value();
	uMax=ui->doubleSpinBoxMaxUse->value();

	if(uMax>paretoX[paretoSize-1] || uMax<paretoX[0]
			|| uMin>paretoX[paretoSize-1] || uMin<paretoX[0]){
		return;
	}

	if(uMax<=uMin){
		return;
	}


	int uBegin=-1, uEnd=-1;

	for(int i=0;i<paretoSize;i++){
		if(paretoX[i]<=uMin)uBegin=i;
		if(paretoX[paretoSize-1-i]>=uMax)uEnd=paretoSize-i-1;
	}

	if(uBegin<0){
		uBegin=0;
		uMin=paretoX[uBegin];
	}
	if(uEnd<0 || uEnd>=paretoSize){
		uEnd=paretoSize-1;
		uMax=paretoX[uEnd];
	}


	if(sMax>uMax || sMax<uMin || sMin>uMax || sMin<uMin){
		return;
	}

	if(sMax<=sMin){
		return;
	}

	double F, X;

	if(!BreakPoint(paretoX+uBegin, paretoY+uBegin, uEnd-uBegin+1, &F, &X, sMin, sMax)){
		QMessageBox::about(this, "l", "хана");
		return;
	}

	marker.setXValue(X);
	marker.attach(&plot);
	ui->label_8->setText(tr("Точка перелома: %1, критерий Фишера = %2")
			.arg(X).arg(F));
	plot.replot();
}

void MainWindow::on_pushButtonStop_clicked()
{
	animRun=false;
}

void MainWindow::on_pushButtonRun_clicked()
{
	animRun=true;
	int step=ui->spinBoxStep->value();
	int timeToStep=ui->spinBoxMinTime->value();


	QTime	time=QTime::currentTime();
	do{
		if(ui->sliderPos->value()+step>data.size())
			step=data.size()-ui->sliderPos->value();

		if(step==0){
			animRun=false;
			return;
		}
		ui->sliderPos->setValue(ui->sliderPos->value()+step);

		step=ui->spinBoxStep->value();

		do{
			qApp->processEvents();
		}while(time.msecsTo(QTime::currentTime()) < timeToStep);

		time=QTime::currentTime();

	}while(animRun);
}

void MainWindow::on_action_ExportTxt_triggered()
{
	QString str=QFileDialog::getSaveFileName(this, tr("Сохранить в.."), dirStr, "*.txt");

	if(str.isEmpty())return;

	QFile file(str);
	if(!file.open(QIODevice::WriteOnly))return;
	QTextStream out(&file);

	for(int i=0;i<paretoSize;i++)
		out<<paretoX[i]<<'\t'<<paretoY[i]<<'\n';

	file.close();
}
