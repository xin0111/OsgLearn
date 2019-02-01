#pragma once
#include <QWidget>
#include <osgViewer/viewer>   
#include <osgQt/GraphicsWindowQt>
#include <QTimer>

class QEarthWidget : public QWidget, public osgViewer::Viewer
{
public:
	QEarthWidget(const QString& earthFile);
	~QEarthWidget();

private:
	//创建 GraphicsWindowQt 
	osgQt::GraphicsWindowQt* createGraphicsWindow(int x, int y, int w, int h,
		const std::string& name = "", bool windowDecoration = false);

	//创建 GLWidget
	osgQt::GLWidget*  createViewWidget(osgQt::GraphicsWindowQt* graphic);

	void installFrameTimer();

	virtual void paintEvent(QPaintEvent *event);
private:
	osgQt::GraphicsWindowQt*	m_gw;
	osgQt::GLWidget*     m_earthGLWidget;
	QTimer m_timer;
};

