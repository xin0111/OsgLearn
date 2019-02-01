#include "ChartControler.h"

#include <QDir>

ChartControler::ChartControler() :
m_pos(PosPose())
{
	m_ID = 1000;
	QString basePath = QDir::currentPath();
	std::vector<QVector3D> points;
	std::string  testfile = basePath.toStdString() + "/../AntennaData.txt";
	ChartPicture3DManager::readEnvelopeDataFromFile(testfile.c_str(), points);
	m_pos.dLon = 100.085;
	m_pos.dLat = 39.7814;
	m_manager.createEnvelop3D(m_ID, m_pos, points, true);

	connect(&m_timer, SIGNAL(timeout()), this, SLOT(updateChart()));
	m_timer.start(1000 * 10);
}

void ChartControler::updateChart()
{
	m_pos.dLon += 0.1;
	m_manager.updateChartPosPose(m_ID, m_pos);
}
