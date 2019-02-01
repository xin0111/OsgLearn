#pragma once
#include <QObject>
#include <QTimer>
#include "Envelop3D\ChartPicture3DManager.h"

class ChartControler :public QObject
{
	Q_OBJECT
public:
	ChartControler();
	~ChartControler(){}
	public slots:
	void updateChart();
public:
	ChartPicture3DManager m_manager;
	int m_ID;
	PosPose m_pos;
	QTimer m_timer;
};

