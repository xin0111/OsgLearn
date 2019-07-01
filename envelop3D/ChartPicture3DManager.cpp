#include "ChartPicture3DManager.h"
#include "QEarthManager.h"


ChartPicture3DManager::ChartPicture3DManager():
m_envelopsNode(new osg::Group())
{
	//添加包络显示
	m_bEnable = mountToScrene(QEarthManager::instance()->getScreenRoot());
}

ChartPicture3DManager::~ChartPicture3DManager()
{
	DelAllChartPicture();
	removefromScrence(QEarthManager::instance()->getScreenRoot());
	m_envelopsNode.release();
}

bool ChartPicture3DManager::mountToScrene(osg::Group* screnceRoot)
{
	if (screnceRoot)
	{
		screnceRoot->addChild(m_envelopsNode);
		return true;
	}	
	return false;
}

void ChartPicture3DManager::removefromScrence(osg::Group * screnceRoot)
{
	if (screnceRoot)
	{
		screnceRoot->removeChild(m_envelopsNode);
	}
}

void ChartPicture3DManager::AddChart3D(const int envelopID, const PosPose& objPosPose,
	const  std::vector<QVector3D>& Points, bool bAmplification, bool bCorrect)
{
	//绘制
	ChatPicture3D*  chartPicturetmp = new ChatPicture3D;
	chartPicturetmp->setAmplificationDisplay(bAmplification);
	chartPicturetmp->setCorrect(bCorrect);
	chartPicturetmp->DrawDirectionOfChart(envelopID, objPosPose, Points);
	chartPicturetmp->SetVisible(true);	
	//地图显示
	m_envelopsNode->addChild(chartPicturetmp->getNodeMatrix());
	m_ChartPictures.push_back(chartPicturetmp);
}

bool ChartPicture3DManager::UpDateChart3D(const int envelopID, const PosPose& objPosPose,
	const  std::vector<QVector3D>& Points, bool bAmplification, bool bCorrect)
{
	for (std::vector<ChatPicture3D*>::iterator it = m_ChartPictures.begin(); it != m_ChartPictures.end(); ++it)
	{
		ChatPicture3D* chartptr = *it;
		if (chartptr->GetEquipID() == envelopID)
		{		
			chartptr->setAmplificationDisplay(bAmplification);
			chartptr->setCorrect(bCorrect);
			bool show = chartptr->getNodeMatrix()->getNodeMask();
			//删除旧数据	
			chartptr->ClearChart();
			////替换为新的数据
			chartptr->DrawDirectionOfChart(envelopID, objPosPose,
				Points);
				
			chartptr->getNodeMatrix()->setNodeMask(show);
				return true;
			
			break;
		}
	}
	return false;
}
bool ChartPicture3DManager::setVisibleById(const int& envelopID, bool bShow)
{
	for (std::vector<ChatPicture3D*>::iterator it = m_ChartPictures.begin(); it != m_ChartPictures.end(); ++it)
	{
		ChatPicture3D* chartptr = *it;
		if (chartptr->GetEquipID() == envelopID)
		{
			chartptr->SetVisible(bShow);
			return true;		
		}
	}
	return false;
}

int ChartPicture3DManager::getStatus(const int& envelopID)
{
	for (std::vector<ChatPicture3D*>::iterator it = m_ChartPictures.begin(); it != m_ChartPictures.end(); ++it)
	{
		ChatPicture3D* chartptr = *it;
		if (chartptr->GetEquipID() == envelopID)
		{
			return chartptr->GetShow();			
		}
	}
	return -1;
}


void ChartPicture3DManager::setAllVisible(bool bShow)
{
	for (std::vector<ChatPicture3D*>::iterator it = m_ChartPictures.begin(); it != m_ChartPictures.end(); ++it)
	{
		ChatPicture3D* chartptr = *it;

		if (chartptr){
			chartptr->SetVisible(bShow);
		}
	}
}

void ChartPicture3DManager::DelChartDisplayByID(const int&  envelopID)
{
	for (std::vector<ChatPicture3D*>::iterator it = m_ChartPictures.begin(); it != m_ChartPictures.end(); ++it)
	{
		ChatPicture3D* chartptr = *it;
		if (chartptr->GetEquipID() == envelopID)
		{
			removeChatPicture(chartptr);
			m_ChartPictures.erase(it);
			break;
		}
	}
}

void ChartPicture3DManager::DelAllChartPicture()
{
	for (std::vector<ChatPicture3D*>::iterator it = m_ChartPictures.begin(); it != m_ChartPictures.end(); ++it)
	{
		ChatPicture3D* chartptr = *it;
		removeChatPicture(chartptr);
	}
	m_ChartPictures.clear();
}

bool ChartPicture3DManager::readEnvelopeDataFromFile(const char* filepathname, 
	std::vector<QVector3D>& points)
{
	points.clear();
	FILE *stream;
	stream = fopen(filepathname, "rt");
	long nRow = 0;
	if (stream == NULL){
		return false;
	}
	else
	{
		float a, r, d;
		//[方位...]-俯仰-[距离...] 
		while (fscanf(stream, "%f %f %f\n", &a, &r, &d) != EOF)
			nRow++;
		if (nRow == 0){
			fclose(stream);
			return false;
		}	//文件无数据

		fseek(stream, 0L, SEEK_SET);
		for (int i = 0; i < nRow; i++)
		{
			fscanf_s(stream, "%f %f %f\n", &a, &r, &d);
			QVector3D vector3D(a, r, d);
			points.push_back(vector3D);
		}

		fclose(stream);
	}
	return true;
}

void ChartPicture3DManager::createEnvelop3D(const int envelopID, const PosPose& objPosPose,
	const std::vector<QVector3D>& Points, bool bAmplification /*= false*/, bool bCorrect /*= true*/)
{
	if (!m_bEnable) return;

	if (!UpDateChart3D(envelopID, objPosPose, Points, bAmplification,bCorrect))
	{
		AddChart3D(envelopID, objPosPose, Points, bAmplification,bCorrect);
	}
}

void ChartPicture3DManager::removeChatPicture(ChatPicture3D* chartptr)
{
	if (!chartptr) return;
	m_envelopsNode->removeChild(chartptr->getNodeMatrix());
	delete chartptr;
	chartptr = NULL;
}

bool ChartPicture3DManager::updateChartPosPose(const int& envelopID, const PosPose& objPosPose)
{
	for (std::vector<ChatPicture3D*>::iterator it = m_ChartPictures.begin(); it != m_ChartPictures.end(); ++it)
	{
		ChatPicture3D* chartptr = *it;
		if (chartptr->GetEquipID() == envelopID)
		{
			chartptr->setChartPosPose(objPosPose);
			return true;
		}
	}
	return false;
}
