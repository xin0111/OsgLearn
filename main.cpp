#include <QApplication>

#include "QEarthWidget.h"
#include "Envelop3D/ChartPicture3DManager.h"

#include <QDir>
#include <QTimer>

#include "ChartControler.h"

int main(int argc, char*argv[])
{
	QApplication a(argc, argv);

	QString basePath = QDir::currentPath();	
	QString fileEarth = basePath + "/../gisdata/earthfile/china.earth";
	QEarthWidget w(fileEarth);	

	std::string testfile = basePath.toStdString() + "/../EnvolopeData.txt";
	static ChartPicture3DManager man;
	std::vector<QVector3D> points;
	ChartPicture3DManager::readEnvelopeDataFromFile(testfile.c_str(), points);
	PosPose pos = PosPose();
	pos.dLon = 120;
	pos.dLat = 39.7814;
	man.createEnvelop3D(100, pos, points);

	ChartControler controler;	

	w.show();
	return a.exec();
}