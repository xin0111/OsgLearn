#pragma  once

#include <vector>
#include <QVector3D>
#include "ChartPicture3D.h"
#include <osg\Group>

class ChartPicture3DManager
{
public:
	ChartPicture3DManager();
	~ChartPicture3DManager();

	//创建 三维包络图 
	//数据格式 [方位...]-俯仰-[距离...] 
	void createEnvelop3D(const int envelopID, const PosPose& objPosPose,
		const  std::vector<QVector3D>& Points, 
		bool bAmplification = false, bool bCorrect = true);

	//显隐
	bool setVisibleById(const int& envelopID, bool bShow);
	//获取指定ID显隐状态 -1: 未找到; 1：显示; 0:隐藏
	int getStatus(const int& equipID);
	//显示所有
	void setAllVisible(bool bShow);
	//清除显示内容
	void DelChartDisplayByID(const int& envelopID);
	//从节点中删除所有
	void DelAllChartPicture();
	//更新位置角度
	bool updateChartPosPose(const int& envelopID, const PosPose& objPosPose);
	//从文件中读取 [方位...]-俯仰-[距离...] 
	static bool readEnvelopeDataFromFile(const char* filepathname,
		std::vector<QVector3D>& points);
private:
	
	//显示初始化
	bool mountToScrene(osg::Group* screnceRoot);

	virtual void removefromScrence(osg::Group * screnceRoot);

	//添加
	void AddChart3D(const int envelopID, const PosPose& objPosPose,
		const  std::vector<QVector3D>& Points, bool bAmplification, bool bCorrect);

	//更新
	bool UpDateChart3D(const int envelopID, const PosPose& objPosPose,
		const  std::vector<QVector3D>& Points, bool bAmplification, bool bCorrect);

	void removeChatPicture(ChatPicture3D* chartptr);
	//存储
	osg::ref_ptr<osg::Group> m_envelopsNode;
	std::vector<ChatPicture3D*> 	m_ChartPictures;
	bool m_bEnable;
};
