#include "QEarthMath.h"


osg::Matrixd QEarthMath::OrthMatrixInvert_3x3(const osg::Matrixd& mat)
{
  return osg::Matrixd( mat( 0, 0 ), mat( 1, 0 ), mat( 2, 0 ), 0.0f,
    mat( 0, 1 ), mat( 1, 1 ), mat( 2, 1 ), 0.0f,
    mat( 0, 2 ), mat( 1, 2 ), mat( 2, 2 ), 0.0f,
    0.0f,		  0.0f,		   0.0f,		1.0f );
}

osg::Matrixd QEarthMath::OrthMatrixInvert_4x3(const osg::Matrixd& mat)
{
  osg::Matrixd inverseMat( mat( 0, 0 ), mat( 1, 0 ), mat( 2, 0 ), 0.0f,
    mat( 0, 1 ), mat( 1, 1 ), mat( 2, 1 ), 0.0f,
    mat( 0, 2 ), mat( 1, 2 ), mat( 2, 2 ), 0.0f,
    0.0f,		  0.0f,		   0.0f,		1.0f );

  osg::Vec3d inversePos = -mat.getTrans() * inverseMat;

  inverseMat.setTrans( inversePos );

  return inverseMat;
}

void QEarthMath::QuatToEulerAngle(const osg::Quat& quat, double& azimuth_deg, double& pitch_deg, double& roll_deg)
{
  double azimuth = 0.0f;
  double pitch = 0.0f;
  double roll = 0.0f;

  double quat_x = quat.x();
  double quat_y = quat.y();
  double quat_z = quat.z();
  double quat_w = quat.w();

  // 计算角度
  double sinPitch = -2.0f * ( quat_y * quat_z + quat_w * quat_x );
  // 检查万向锁
  if ( fabs(sinPitch) > 0.9999f )
  {
    // 向正上或正下看
    pitch = osg::PI_2 * sinPitch;
    // 计算heading,roll置0
    azimuth = atan2( -quat_x * quat_z - quat_w * quat_y, 0.5f - quat_y * quat_y - quat_z * quat_z );
    roll = 0.0f;
  }
  else
  {
    pitch = asin( sinPitch );
    azimuth = atan2( quat_x * quat_z - quat_w * quat_y, 0.5f - quat_x * quat_x - quat_y * quat_y );
    roll = atan2( quat_x * quat_y - quat_w * quat_z, 0.5f - quat_x * quat_x - quat_z * quat_z );
  }

  //azimuth_deg = -osg::RadiansToDegrees( roll );
  azimuth_deg = osg::RadiansToDegrees( roll );
  pitch_deg = -osg::RadiansToDegrees( pitch );
  roll_deg = -osg::RadiansToDegrees( azimuth );
}

void QEarthMath::GEOCoordFrameToWorldCoordFrame(const double lon_deg, const double lat_deg, const double alt,
  const double x_offset, const double y_offset, const double z_offset,
  const double azimuth_deg, const double pitch_deg, const double roll_deg,
  osg::Matrixd& worldCoordFrame, const osgEarth::SpatialReference& mapSRS )
{
  mapSRS.getEllipsoid()->computeLocalToWorldTransformFromLatLongHeight( osg::DegreesToRadians( lat_deg ),
    osg::DegreesToRadians( lon_deg ), alt, worldCoordFrame );
  worldCoordFrame.preMultTranslate( osg::Vec3d( x_offset, y_offset, z_offset ));
  worldCoordFrame.preMult( osg::Matrixd::rotate( -osg::DegreesToRadians( azimuth_deg ), osg::Z_AXIS ));
  worldCoordFrame.preMult( osg::Matrixd::rotate( osg::DegreesToRadians( pitch_deg ), osg::X_AXIS ));
  worldCoordFrame.preMult( osg::Matrixd::rotate( osg::DegreesToRadians( roll_deg ), osg::Y_AXIS ));
}

void QEarthMath::GEOCoordFrameToWorldCoordFrame(const double lon_deg, const double lat_deg, const double alt,
  const double x_offset,const double y_offest,const double z_offset,
  const double x_self_offset,const double y_self_offset,const double z_self_offset,
  const double azimuth_deg,const double pitch_deg,const double roll_deg,
  osg::Matrixd& worldCoordFrame,const osgEarth::SpatialReference& mapSRS )
{
  mapSRS.getEllipsoid()->computeLocalToWorldTransformFromLatLongHeight( osg::DegreesToRadians( lat_deg),
    osg::DegreesToRadians( lon_deg), alt, worldCoordFrame);
  worldCoordFrame.preMultTranslate( osg::Vec3d(x_offset,y_offest,z_offset));
  worldCoordFrame.preMultTranslate( osg::Vec3d(x_self_offset,y_self_offset,z_self_offset));
  worldCoordFrame.preMult( osg::Matrixd::rotate( -osg::DegreesToRadians( azimuth_deg ),osg::Z_AXIS ));
  worldCoordFrame.preMult( osg::Matrixd::rotate( osg::DegreesToRadians( pitch_deg ),osg::X_AXIS));
  worldCoordFrame.preMult( osg::Matrixd::rotate( osg::DegreesToRadians( roll_deg ),osg::Y_AXIS));

}
osg::Vec3d QEarthMath::GEOCoordFrameToWorldCoorFrame(const double lon_deg,
  const double lat_deg,const double alt, const double x_offset,
  const double y_offest,const double z_offset, const double x_self_offset,
  const double y_self_offset,const double z_self_offset, const double azimuth_deg,
  const double pitch_deg,const double roll_deg,
  osgEarth::SpatialReference* mapSRS /*= NULL*/)
{
  osg::Matrixd worldCoordFrame;
  if (mapSRS == NULL)
  {
    mapSRS = osgEarth::SpatialReference::create("wgs84");
  }
  GEOCoordFrameToWorldCoordFrame(lon_deg,lat_deg,alt,x_offset,
    y_offest,z_offset,x_self_offset,y_self_offset,
    z_self_offset,azimuth_deg,pitch_deg,roll_deg,worldCoordFrame,*mapSRS);
  osg::Vec3d lla( 0.0f, 0.0f, 0.0f );
  mapSRS->transformFromWorld( worldCoordFrame.getTrans(), lla );
  return lla;
}

void QEarthMath::WorldCoordFrameToGEOCoordFrame(double& lon_deg, double& lat_deg, double& alt,
  double& azimuth_deg, double& pitch_deg, double& roll_deg,
  const osg::Matrixd& worldCoordFrame, const osgEarth::SpatialReference& mapSRS )
{
  osg::Vec3d lla( 0.0f, 0.0f, 0.0f );
  mapSRS.transformFromWorld( worldCoordFrame.getTrans(), lla );
  lon_deg = lla.x();
  lat_deg = lla.y();
  alt = lla.z();
  /**地理坐标系相对于地心坐标系的旋转*/
  osg::Matrixd geoMat;
  mapSRS.getEllipsoid()->computeCoordinateFrame( osg::DegreesToRadians( lat_deg ), osg::DegreesToRadians( lon_deg ), geoMat );

  osg::Matrixd localMat = worldCoordFrame * OrthMatrixInvert_3x3( geoMat );
  osg::Quat rotQuat = localMat.getRotate();
  QuatToEulerAngle( rotQuat, azimuth_deg, pitch_deg, roll_deg );
}

void QEarthMath::GetLonLatHByLonLatAziPitch(double lon0, double lat0, double h0,
  double azi, double pitch,double dis,double& outLon,
  double& outLat,double& outH,osgEarth::SpatialReference* srs)
{
  osg::Matrixd worldCoordFrame;
  if (srs == NULL)
  {
    srs = osgEarth::SpatialReference::create("wgs84");
  }
  srs->getEllipsoid()->computeLocalToWorldTransformFromLatLongHeight( osg::DegreesToRadians( lat0 ),
    osg::DegreesToRadians(lon0),h0, worldCoordFrame );
  double outx,outy,outz;
  outx = dis * sin(osg::DegreesToRadians(azi)) * cos(osg::DegreesToRadians(pitch)) ;
  outy = dis * cos(osg::DegreesToRadians(azi)) * cos(osg::DegreesToRadians(pitch));
  outz = dis * sin(osg::DegreesToRadians(pitch));
  worldCoordFrame.preMultTranslate( osg::Vec3d(outx,outy,outz));
  worldCoordFrame.preMult( osg::Matrixd::rotate( -osg::DegreesToRadians( azi ),osg::Z_AXIS ));
  worldCoordFrame.preMult( osg::Matrixd::rotate( osg::DegreesToRadians( pitch ),osg::X_AXIS));
  worldCoordFrame.preMult( osg::Matrixd::rotate( osg::DegreesToRadians( 0.0 ),osg::Y_AXIS));
  osg::Vec3d lla( 0.0f, 0.0f, 0.0f );
  srs->transformFromWorld( worldCoordFrame.getTrans(), lla );
  outLon = lla.x();
  outLat = lla.y();
  outH = lla.z();
}

void QEarthMath::GetRelativePositon(double StartLon, double StartLat, double StartH,
  double TargetLon,double TargetLat,double TargetH,
  double &OutX,double &OutY,double &OutZ,
  osgEarth::SpatialReference* srs)
{
  if (srs == NULL)
  {
    srs = osgEarth::SpatialReference::create("wgs84");
  }
  osg::Vec3d TargetXYZ;
  osg::Matrixd LocalToworld,WorldToLocal;
  srs->getEllipsoid()->computeLocalToWorldTransformFromLatLongHeight(
    osg::DegreesToRadians( StartLat ),
    osg::DegreesToRadians( StartLon ),
    StartH, LocalToworld );
  WorldToLocal.invert( LocalToworld );

  osg::Vec3d targetBLH(TargetLon,TargetLat,TargetH);
  srs->transformToWorld( targetBLH,TargetXYZ );
  osg::Vec3d TargetLocal = TargetXYZ * WorldToLocal;
  OutX = TargetLocal.x();
  OutY = TargetLocal.y();
  OutZ = TargetLocal.z();
}

