#pragma once
#include <QString>
#include <osgEarth/MapNode>
#include <osg/MatrixTransform>
#include <osgEarth/MapNode>
#include <osgEarthUtil/EarthManipulator>
#include <osgViewer/viewer>   
#include <osgEarthUtil/Sky>

class QEarthManager
{
public:
	static QEarthManager* instance();

	//初始化 地球显示
	bool initEarthView(const QString& earthFile, osgViewer::Viewer* pViewer);

	//获取Map节点
	osg::ref_ptr<osgEarth::MapNode> getMapNode(){ return m_mapNode; }

	//获取根节点
	osg::ref_ptr<osg::Group> getScreenRoot(){ return m_pRoot; }

	/*=========================================================================
	Function Name:   convertObjMatrix
	Description:	 更新三维坐标
	---------------------------------------------------------------------------
	Parameter:
	[IN]   dLon
	[IN]   dLat
	[IN]   dAlt
	[IN]   dAzimuth 	// 方位
	[IN]   dPitch		/ 俯仰
	[IN]   dRoll		// 滚转角
	[IN]   objMatrix
	---------------------------------------------------------------------------
	Return Value:
	=========================================================================*/
	void convertObjMatrix(const double dLon, const double dLat, const double dAlt,
		const double dAzimuth, const double dPitch, 
		const double dRoll, osg::MatrixTransform* matrixNode);

	void worldPointToMapPoint(const double x, const double y, const double z, double& lon, double& lat, double& alt);
	
	void mapPointToWorldPoint(const double lon, const double lat, const double alt, double& x, double& y, double& z);

	double getElevation(const double dLon, const double dLat);
	
	osg::ref_ptr<osg::Group> createLight(osg::ref_ptr<osg::Node> node);

private:
	QEarthManager();
	~QEarthManager();

	void addEarthManipulator(osgViewer::Viewer* pViewer);

	void addSkyNode(osgViewer::Viewer* pViewer);

private:
	osg::ref_ptr<osg::Group> m_pRoot;
	osg::ref_ptr<osgEarth::MapNode>  m_mapNode;
	osg::ref_ptr<osgEarth::Util::EarthManipulator> m_manipulator;
	osg::ref_ptr<osgEarth::Util::SkyNode> m_skyNode;
};

