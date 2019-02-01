#pragma  once

#include <osg/Vec4d>
#include <osg/ref_ptr>
#include <osg/Group>
#include <QVector3D>
#include <osg/Geometry>
#include <osg/MatrixTransform>

namespace ChatDirection
{
	typedef struct{
		unsigned char r;
		unsigned char g;
		unsigned char b;
	}RGB, *PRGB;
}
struct PosPose
{//目标位置姿态
	double      dLon;
	double      dLat;
	double      dAlt;
	double      dAzimuth;// 方位
	double      dPitch;	// 俯仰
	double      dRoll;	// 滚转角
};

class ChatPicture3D
{
public:
	ChatPicture3D();
	~ChatPicture3D();
	//root node
	osg::ref_ptr<osg::MatrixTransform> getNodeMatrix(){ return m_nodeMatrix; }

	//画图 数据格式 [方位...]-俯仰-[距离...] 
	void  DrawDirectionOfChart(const int envelopID, const PosPose& objPosPose,
		const std::vector<QVector3D>& points3D);

	//设置位置姿态
	void setChartPosPose(const PosPose& objPosPose);

	int GetEquipID() { return m_chartID; }

	//清空显示数据
	void  ClearChart();

	//设置节点是否显示
	void SetVisible(const bool show);

	//获取显示控制是否显示
	bool  GetShow() { return m_Show; }

	//是否放大显示
	void setAmplificationDisplay(bool bAmplification);

	//同步2D角度矫正
	void setCorrect(bool bCorrect);
private:

	void initGeometry();

	void initEnvelopMatrix();

	osg::Vec3d computerVHPosition(double vdegree, double hdegree, double r);

	ChatDirection::RGB GetGridColor_gradient(double dcurrent_value, 
		double dthreshold_value, double dmax_value, unsigned int alpha);

	void ColorRing(unsigned int count, ChatDirection::PRGB rgb, unsigned int color_intensity);

	void Coe2Color(double coe, ChatDirection::PRGB rgb, 
		int color_intensity, int color_start, int color_end);

	osg::ref_ptr<osg::MatrixTransform> m_nodeMatrix;
	osg::ref_ptr<osg::Group>  m_ChartGroup;
	osg::ref_ptr<osg::MatrixTransform> m_chartMatrix;
	osg::ref_ptr<osg::Geometry> m_geometry;
	bool m_Show;
	int m_chartID;
	bool m_bAmplification;
	bool m_bCorrect;
	float  m_scaleSize;
	float m_aziCorrect;
	float m_pitchCorrect;
};