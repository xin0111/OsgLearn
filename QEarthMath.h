#ifndef KUMAPMATH_3D_H
#define KUMAPMATH_3D_H


#include <osg/Quat>
#include <osg/Matrixd>
#include <osgEarth/SpatialReference>

class QEarthMath
{
public:
/**
     * 正交矩阵求逆.根据正交矩阵的特性,快速求解其逆矩阵.
     * 本函数输入矩阵旋转因子部分的3x3子矩阵必须是正交矩阵,例如旋转生成的矩阵.
     * 本函数只求解其旋转部分的逆.忽略平移部分.
     */
static osg::Matrixd  OrthMatrixInvert_3x3( const osg::Matrixd& mat );
/**
     * 该函数在求解旋转部分的逆之外，还求解了平移部分的逆
    */
static osg::Matrixd  OrthMatrixInvert_4x3(const osg::Matrixd& mat);
/**
     * 旋转四元数转换成欧拉角
     * 约定物体绕Z轴旋转的角度为azimuth_deg，绕Y轴旋转为pitch_deg，绕x轴旋转为roll_deg
     * 四元数转换成欧拉角时，旋转路径不唯一.约定旋转路径为z轴-y轴-x轴
     * quat:旋转四元数
     * azimuth_rad-pitch_rad-roll_rad:方位角-俯仰角-横滚角
     */
static void  QuatToEulerAngle(const osg::Quat& quat, double& azimuth_deg, double& pitch_deg, double& roll_deg);
static void  GetLonLatHByLonLatAziPitch(double lon0, double lat0, double h0, double azi,
                                double pitch,double dis,double& outLon,double& outLat,double& outH,osgEarth::SpatialReference* srs = NULL);
/**
     * 物体在地理坐标系下的位置姿态转换成世界坐标下的位置姿态矩阵
     * 约定物体绕Z轴旋转的角度为azimuth_deg，绕Y轴旋转为pitch_deg，绕x轴旋转为roll_deg
     * lon_deg-lat_deg-alt.经度-纬度-海拔.单位:角度-米
     * x_offset,y_offset,z_offset.相对于经纬点的偏差.单位:米
     * azimuth_rad-pitch_rad-roll_rad:地理坐标系下的方位角-俯仰角-横滚角
     * worldCoordFrame:物体在世界坐标系下的位置姿态矩阵
     * SpatialReference:空间坐标系统
     */
static void  GEOCoordFrameToWorldCoordFrame(const double lon_deg, const double lat_deg, const double alt,
                                     const double x_offset, const double y_offset, const double z_offset,
                                     const double azimuth_deg, const double pitch_deg, const double roll_deg,
                                     osg::Matrixd& worldCoordFrame, const osgEarth::SpatialReference& mapSRS );

/**
     * 物体在地理坐标系下的位置姿态转换成世界坐标下的位置姿态矩阵
     * 约定物体绕Z轴旋转的角度为azimuth_deg，绕Y轴旋转为pitch_deg，绕x轴旋转为roll_deg
     * lon_deg-lat_deg-alt.经度-纬度-海拔.单位:角度-米
     * x_offset,y_offset,z_offset.相对于经纬点的偏差.单位:米
     * x_self_offset,y_self_offset,z_self_offset.相对于世界坐标系的偏差(用于推演时编队)
     * azimuth_rad-pitch_rad-roll_rad:地理坐标系下的方位角-俯仰角-横滚角
     * worldCoordFrame:物体在世界坐标系下的位置姿态矩阵
     * SpatialReference:空间坐标系统
     */
static void  GEOCoordFrameToWorldCoordFrame(const double lon_deg, const double lat_deg, const double alt,
                                    const double x_offset,const double y_offest,const double z_offset,
                                    const double x_self_offset,const double y_self_offset,const double z_self_offset,
                                    const double azimuth_deg,const double pitch_deg,const double roll_deg,
                                    osg::Matrixd& worldCoordFrame,const osgEarth::SpatialReference& mapSRS );

static osg::Vec3d  GEOCoordFrameToWorldCoorFrame(const double lon_deg, const double lat_deg, const double alt,
                                         const double x_offset,const double y_offest,const double z_offset,
                                         const double x_self_offset,const double y_self_offset,const double z_self_offset,
                                         const double azimuth_deg,const double pitch_deg,const double roll_deg,
                                         osgEarth::SpatialReference* mapSRS = NULL);
/**
    * 物体在世界坐标下的位置姿态转换成物体在地理坐标系下的位置姿态
    * 约定物体绕Z轴旋转的角度为azimuth_deg，绕Y轴旋转为pitch_deg，绕x轴旋转为roll_deg
    * lon_deg-lat_deg-alt.经度-纬度-海拔.单位:角度-米
    * azimuth_rad-pitch_rad-roll_rad:方位角-俯仰角-横滚角
    * azimuth_rad-pitch_rad-roll_rad:地理坐标系下的方位角-俯仰角-横滚角
    * worldCoordFrame:物体在世界坐标系下的位置姿态矩阵
    * SpatialReference:空间坐标系统
    */
static void  WorldCoordFrameToGEOCoordFrame(double& lon_deg, double& lat_deg, double& alt,
                                     double& azimuth_deg, double& pitch_deg, double& roll_deg,
                                     const osg::Matrixd& worldCoordFrame, const osgEarth::SpatialReference& mapSRS );

static void  GetRelativePositon(double StartLon, double StartLat, double StartH,
                        double TargetLon,double TargetLat,double TargetH,
                        double &OutX,double &OutY,double &OutZ,
                        osgEarth::SpatialReference* srs = NULL);

};
#endif // KUMAPMATH_3D_H
