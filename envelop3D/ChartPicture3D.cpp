#include "ChartPicture3D.h"
#include <stdlib.h>


#include <osg/Geode>
#include <osg/PolygonMode>
#include <osg/ShapeDrawable>
#include <osg/LightSource>
#include <osgUtil/SmoothingVisitor>
#include <osg/BlendFunc>
#include <osg/CullFace>
#include <osg/Depth>
#include "QEarthManager.h"


ChatPicture3D::ChatPicture3D():
m_nodeMatrix(new osg::MatrixTransform()),
m_bAmplification(false),
m_bCorrect(true),
m_scaleSize(1.0),
m_aziCorrect(0.0),
m_pitchCorrect(0.0)
{
	m_chartID = -1;
	m_Show = true;
	m_ChartGroup = new  osg::Group;
	m_geometry = new osg::Geometry;
	m_chartMatrix = new osg::MatrixTransform;
	initGeometry();
	initEnvelopMatrix();
	//
	m_chartMatrix->addChild(m_geometry);
	m_ChartGroup->addChild(m_chartMatrix);
	m_nodeMatrix->addChild(m_ChartGroup);
}

ChatPicture3D::~ChatPicture3D()
{
	m_chartMatrix->removeChild(m_geometry);
	m_ChartGroup->removeChild(m_chartMatrix);	
	m_nodeMatrix->removeChild(m_ChartGroup);
	m_nodeMatrix.release();
}

void ChatPicture3D::DrawDirectionOfChart(const int envelopID, const PosPose& objPosPose,
	const std::vector<QVector3D>& points3D)
{
	m_chartID = envelopID;

	osg::ref_ptr<osg::Vec3Array> vertexArray = new osg::Vec3Array;
	osg::ref_ptr<osg::Vec4Array> vertexColorArray = new osg::Vec4Array;
	osg::ref_ptr<osg::DrawElementsUInt> drawElement = new osg::DrawElementsUInt;

	//计算模型的顶点位置，并加入到顶点数组中
	std::vector<float> lenVec;
	float maxLen = 0, minLen = 0;
	std::set<int> pitchSet, aziSet;
	for (auto it = points3D.begin(); it != points3D.end(); ++it)
	{
		vertexArray->push_back(computerVHPosition(it->y(), it->x(), it->z()));
		lenVec.push_back(it->z());
		float  number = it->z();
		maxLen = std::max(number, maxLen);
		minLen = std::min(number, minLen);
		pitchSet.insert(it->y());
		aziSet.insert(it->x());
	}
	int pitchCount = pitchSet.size();
	
	int aziCount = aziSet.size();

	if (pitchCount * aziCount != points3D.size())
	{
		OE_WARN << "envelop data has error." << std::endl;
	}

	for (size_t i = 0; i < lenVec.size(); i++)
	{
		ChatDirection::RGB rgb = GetGridColor_gradient(maxLen - lenVec[i], minLen, maxLen, 255);
		osg::Vec4 rgba(osg::Vec4((float)rgb.r / 255, (float)rgb.g / 255, (float)rgb.b / 255, 0.2f));
		vertexColorArray->push_back(rgba);
	}

	for (long i = 0; i < pitchCount; i++)
	{
		for (long j = 0; j < aziCount; j++)
		{
			unsigned int p0 = i * aziCount + j;
			unsigned int p1 = i * aziCount + j + 1;
			unsigned int p2 = (i + 1) * aziCount + j;
			unsigned int p3 = (i + 1) * aziCount + j + 1;

			// 0 3 1
			drawElement->push_back(p0);
			drawElement->push_back(p3);
			drawElement->push_back(p1);

			// 0 2 3
			drawElement->push_back(p0);
			drawElement->push_back(p2);
			drawElement->push_back(p3);
		}
	}

	m_geometry->setVertexArray(vertexArray);
	//填充顶点色
	m_geometry->setColorArray(vertexColorArray, osg::Array::Binding::BIND_PER_VERTEX);

	//充填方式
	drawElement->setMode(osg::PrimitiveSet::TRIANGLE_STRIP);// 显示 点
	m_geometry->addPrimitiveSet(drawElement);//add first

	m_scaleSize = m_bAmplification ? (100000.0 / maxLen) : 1.0; //放大比例
		
	setChartPosPose(objPosPose);

	m_ChartGroup->setName(std::to_string(envelopID));
}

void ChatPicture3D::setChartPosPose(const PosPose& objPosPose)
{
	//更新坐标 
	QEarthManager::instance()->convertObjMatrix(objPosPose.dLon,
		objPosPose.dLat, objPosPose.dAlt, objPosPose.dAzimuth,
		m_bCorrect ? 0.0 : objPosPose.dPitch,
		objPosPose.dRoll, m_nodeMatrix);

	//匹配2D 俯仰-方位
	m_aziCorrect = m_bCorrect ? 180.0 : 0.0;
	m_pitchCorrect = m_bCorrect ? -objPosPose.dPitch : 0.0;
	m_chartMatrix->setMatrix(osg::Matrixd::scale(m_scaleSize, m_scaleSize, m_scaleSize)
		* (osg::Matrixd::rotate(osg::DegreesToRadians(m_pitchCorrect), osg::Y_AXIS)
		* osg::Matrixd::rotate(osg::DegreesToRadians(m_aziCorrect), osg::Z_AXIS))
		);
}

//通过极坐标计算直角坐标系位置
osg::Vec3d ChatPicture3D::computerVHPosition(double vdegree, double hdegree, double r)
{
	osg::Vec3d	pos;
	double v = osg::DegreesToRadians(vdegree);
	double h = osg::DegreesToRadians(hdegree);
	pos.x() = r * cos(v) * cos(h);
	pos.y() = r * cos(v) * sin(h);
	pos.z() = r * sin(v);
	return pos;
}

ChatDirection::RGB ChatPicture3D::GetGridColor_gradient(double dcurrent_value, double dthreshold_value, double dmax_value, unsigned int alpha)
{
	ChatDirection::RGB rgb;
	rgb.r = 0;
	rgb.g = 0;
	rgb.b = 0;

	if (dcurrent_value < dthreshold_value)
		return 	rgb;

	//渐变色
	float fCoe = (dcurrent_value - dthreshold_value) / (dmax_value - dthreshold_value);
	Coe2Color(fCoe, &rgb, 255, 0, 5);
	return rgb;
}

void ChatPicture3D::ColorRing(unsigned int count, ChatDirection::PRGB rgb, unsigned int color_intensity)
{
	count %= (6 * color_intensity);//防止count溢出
	if (count <= color_intensity){
		rgb->r = 0;
		rgb->g = count;
		rgb->b = color_intensity;
	}
	else if (count > color_intensity && count <= 2 * color_intensity){
		rgb->r = 0;
		rgb->g = color_intensity;
		rgb->b = 2 * color_intensity - count;
	}
	else if (count > 2 * color_intensity && count <= 3 * color_intensity){
		rgb->r = count - 2 * color_intensity;
		rgb->g = color_intensity;
		rgb->b = 0;
	}
	else if (count > 3 * color_intensity && count <= 4 * color_intensity){
		rgb->r = color_intensity;
		rgb->g = 4 * color_intensity - count;
		rgb->b = 0;
	}
	else if (count > 4 * color_intensity && count <= 5 * color_intensity){
		rgb->r = color_intensity;
		rgb->g = 0;
		rgb->b = count - 4 * color_intensity;
	}
	else{
		rgb->r = 6 * color_intensity - count;
		rgb->g = 0;
		rgb->b = color_intensity;
	}
}

void ChatPicture3D::Coe2Color(double coe, ChatDirection::PRGB rgb, int color_intensity, int color_start, int color_end)
{
	int count = color_start*color_intensity
		+ coe*color_intensity*(color_end - color_start);
	ColorRing(count, rgb, color_intensity);
}
void ChatPicture3D::ClearChart()
{		
	m_geometry->removePrimitiveSet(0);  //remove first
}

void ChatPicture3D::SetVisible(const bool show)
{
	m_ChartGroup->setNodeMask(show);
	m_Show = show;
}

void ChatPicture3D::initGeometry()
{
	osg::StateSet* geoStateSet = m_geometry->getOrCreateStateSet();
	geoStateSet->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
	osg::ref_ptr<osg::BlendFunc> blendFunc = new osg::BlendFunc(
		osg::BlendFunc::SRC_ALPHA, osg::BlendFunc::ONE_MINUS_SRC_ALPHA);
	geoStateSet->setAttributeAndModes(blendFunc.get());
	geoStateSet->setAttributeAndModes(new osg::CullFace(osg::CullFace::FRONT_AND_BACK),
		osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE);
	osg::ref_ptr<osg::Depth> depth = new osg::Depth;
	depth->setWriteMask(false);
	geoStateSet->setAttributeAndModes(depth, osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
	geoStateSet->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);

	geoStateSet->setMode(GL_RESCALE_NORMAL, osg::StateAttribute::ON);
	geoStateSet->setMode(GL_BLEND, osg::StateAttribute::ON);

	m_geometry->setDataVariance(osg::Object::DYNAMIC);
}

void ChatPicture3D::initEnvelopMatrix()
{
	osg::StateSet* eMatrixStateSet = m_chartMatrix->getOrCreateStateSet();
	eMatrixStateSet->setMode(GL_RESCALE_NORMAL, osg::StateAttribute::ON);
}

void ChatPicture3D::setAmplificationDisplay(bool bAmplification)
{
	m_bAmplification = bAmplification;
}

void ChatPicture3D::setCorrect(bool bCorrect)
{
	m_bCorrect = bCorrect;
}

