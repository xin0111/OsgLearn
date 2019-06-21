#include <QApplication>

#include "QEarthWidget.h"
#include "Envelop3D/ChartPicture3DManager.h"

#include <QDir>
#include <QTimer>

#include "ChartControler.h"
#include "OverlayImage/xOverlayImage.h"
#include "QEarthManager.h"

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

	xOverlayImage xOverlayImg(QEarthManager::instance()->getMapNode());
	QImage img2Overlay;
	img2Overlay.load("C:/out_cpu_GEOGCSnew.jpeg");
	xOverlayImg.DrawCoverImage(img2Overlay, 114.2667, 32.2667, 117.2333, 35.5167, "1000");

	w.show();
	return a.exec();
}