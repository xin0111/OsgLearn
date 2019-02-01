#include "QEarthWidget.h"
#include <osg/group>   
#include <osgDB/ReadFile>
#include <osgViewer/ViewerEventHandlers>
#include <osgGA/MultiTouchTrackballManipulator>
#include <osgGA/StateSetManipulator>
#include <osg/Node>
#include <QGridLayout>
#include <QApplication>
#include "Envelop3D/ChartPicture3DManager.h"
#include "QEarthManager.h"


QEarthWidget::QEarthWidget(const QString& earthFile) :
m_gw(NULL),
m_earthGLWidget(NULL)
{
	this->resize(700, 600);
	this->setThreadingModel(osgViewer::ViewerBase::SingleThreaded);
	this->setKeyEventSetsDone(0);

	m_gw = createGraphicsWindow(0, 0, 200, 200);
	
	m_earthGLWidget = createViewWidget(m_gw);
	
	QEarthManager::instance()->initEarthView(earthFile, this);

	QGridLayout* grid = new QGridLayout(this);
	grid->addWidget(m_earthGLWidget, 0, 0);
	setLayout(grid);

	installFrameTimer();
}


QEarthWidget::~QEarthWidget()
{
	if (m_earthGLWidget)
	{
		delete m_earthGLWidget;
		m_earthGLWidget = NULL;
	}
}

osgQt::GLWidget*  QEarthWidget::createViewWidget(osgQt::GraphicsWindowQt* graphic)
{
	osg::Camera* camera = this->getCamera();
	camera->setGraphicsContext(graphic);

	const osg::GraphicsContext::Traits* traits = graphic->getTraits();

	camera->setClearColor(osg::Vec4(0.2, 0.2, 0.6, 1.0));
	camera->setViewport(new osg::Viewport(0, 0, traits->width, traits->height));
	camera->setProjectionMatrixAsPerspective(30.0f,
		static_cast<double>(traits->width) / static_cast<double>(traits->height), 1.0f, 10000.0f);

	//状态显示-事件回调
	this->addEventHandler(new osgViewer::StatsHandler());
	//窗口改变-事件回调
	this->addEventHandler(new osgViewer::WindowSizeHandler());
	this->addEventHandler(new osgGA::StateSetManipulator(this->getCamera()->getOrCreateStateSet()));
	this->setCameraManipulator(new osgGA::MultiTouchTrackballManipulator);
	graphic->setTouchEventsEnabled(true);

	return graphic->getGLWidget();
}

osgQt::GraphicsWindowQt* QEarthWidget::createGraphicsWindow(int x, int y, int w, int h,
	const std::string& name /*= ""*/, bool windowDecoration /*= false*/)
{
	osg::DisplaySettings* ds = osg::DisplaySettings::instance().get();
	osg::ref_ptr<osg::GraphicsContext::Traits> traits = new osg::GraphicsContext::Traits;
	traits->windowName = name;
	traits->windowDecoration = windowDecoration;
	traits->x = x;
	traits->y = y;
	traits->width = w;
	traits->height = h;
	traits->doubleBuffer = true;
	traits->alpha = ds->getMinimumNumAlphaBits();
	traits->stencil = ds->getMinimumNumStencilBits();
	traits->sampleBuffers = ds->getMultiSamples();
	traits->samples = /*ds->getNumMultiSamples()*/4;

	return new osgQt::GraphicsWindowQt(traits);
}

void QEarthWidget::paintEvent(QPaintEvent *event)
{
	this->frame();
}

void QEarthWidget::installFrameTimer()
{
	connect(&m_timer, SIGNAL(timeout()), this, SLOT(update()));
	m_timer.start(10);
}
