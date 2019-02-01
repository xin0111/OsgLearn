#pragma once
#include <osgEarth/MapNode>
#include <osgEarth/ElevationQuery>
#include <osgViewer/viewer>   
#include <osgEarthUtil/Controls>

//查询高程的事件回调
class QueryElevationHandler : public osgGA::GUIEventHandler
{
public:
	QueryElevationHandler(osgEarth::MapNode* mapNode, osg::ref_ptr<osg::Group> pRoot);
protected:
	bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);
	void displayPosLabel(osg::ref_ptr<osg::Group> pRoot);
private:
	osgEarth::MapNode* _mapNode;
	//地形对象
	const osgEarth::Terrain*   _terrain;
	bool             _mouseDown;
	//查询高程使用的对象
	osgEarth::ElevationQuery   _query;
	osg::NodePath    _path;
	osgEarth::Util::Controls::LabelControl*  m_mouseCoords;
};