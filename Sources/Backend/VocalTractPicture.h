// ****************************************************************************
// This file is part of VocalTractLab.
// Copyright (C) 2020, Peter Birkholz, Dresden, Germany
// www.vocaltractlab.de
// author: Peter Birkholz
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.
//
// ****************************************************************************

#if defined(__APPLE__)
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif
#include <fstream>
#include <sstream>
#include "VocalTract.h"

#ifndef __VOCALTRACT_PICTURE_H__
#define __VOCALTRACT_PICTURE_H__

// ****************************************************************************
// ****************************************************************************

class VocalTractPicture
{
  // **************************************************************************
  // Public data.
  // **************************************************************************

public:
  static const double DEFAULT_DISTANCE;
  static const double DEFAULT_X_TRANSLATION;
  static const double DEFAULT_Y_TRANSLATION;

  struct PictureSize
  {
    int x;
    int y;
  };
  

  enum ControlPoints
  {
    CP_VELUM,
    CP_HYOID,
    CP_JAW,
    CP_LIP_CORNER,
    CP_LIP_DISTANCE,
    CP_TONGUE_CENTER,
    CP_TONGUE_TIP,
    CP_TONGUE_BLADE,
    CP_TONGUE_BACK,
    CP_CUT_PLANE,
    NUM_CONTROLPOINTS
  };

  enum RenderMode
  {
    RM_NONE, RM_3DSOLID, RM_3DWIRE, RM_2D
  };

  // Position and orientation of the VT and the cutting plane

  double distance_cm;
  double yRotation_deg;
  double zRotation_deg;
  double cutPlanePos_cm;     // Position of the interactive cutting plane

  // The background poster for the vocal tract

  GLvoid *poster;            // RGB-color values
  int posterWidth_pix;
  int posterHeight_pix;
  double posterCenterX_cm;
  double posterCenterY_cm;
  double posterScalingFactor;

  // Display options

  bool showPalatogramDivision;
  bool showPoster;
  bool posterEditing;
  bool showControlPoints;
  bool showCenterLine;
  bool isRoughCenterLine;
  bool showCutVectors;
  bool showTongueCrossSections;
  bool crossSectionWithTongue;
  RenderMode renderMode;
  bool renderBothSides;
  int selectedControlPoint;
  bool showEmaPoints;

  Point3D controlPoint[NUM_CONTROLPOINTS];

  // **************************************************************************
  // Public functions.
  // **************************************************************************

public:
  VocalTractPicture(VocalTract *vocalTract);
  ~VocalTractPicture();

  PictureSize GetSize();
  void GetSize(int &pictureWidth, int &pictureHeight);

  void setVocalTract(VocalTract *vocalTract);

  void projection2D();
  void projection3D();
  void print(int x, int y, char *st);

  void drawTest();
  
  void display();

  Point3D getObjectCoordinates(int screenX, int screenY, const Point3D planePoint,
                               const Point3D planeNormal);
  Point3D getMinDistancePoint(int screenX, int screenY, Point3D *point, int numPoints, int &bestIndex, double &bestT);

  void setControlPoint(int pointIndex, int screenX, int screenY);
  void parameterToControlPoint(int pointIndex);
  void projection(Point3D P, double &screenX, double &screenY, double &screenZ);

  void get2DRegion(double &left_cm, double &right_cm, double &bottom_cm, double &top_cm);

  // Export functions ***********************************************
  // bool saveImageBmp(const wxString &fileName);

  // Set modelview and projection matrices **************************

  void setProjectionMatrix3D(double fovy, double nearPlane, double farPlane);
  void setProjectionMatrix2D(double left, double right, double bottom, double top);
  void setModelViewMatrix3D();
  void setModelViewMatrix2D();

  // **************************************************************************
  // Private data.
  // **************************************************************************

private:
  VocalTract *tract;
  PictureSize picSize;

  // Data for the projection and viewport transformation

  double frustumLeft;
  double frustumRight;
  double frustumBottom;
  double frustumTop;
  double frustumNear;
  double frustumFar;
  double viewportX;
  double viewportY;
  double viewportWidth;
  double viewportHeight;

  bool orthogonalProjection;    // is set to "true" for RM_2D and RM_NONE

  // Arrays *********************************************************
  
  int viewport[4];
  double projectionMatrix[16];
  double modelViewMatrix[16];
  double inverseModelViewMatrix[16];

  // **************************************************************************
  // Private functions.
  // **************************************************************************

private:
  void setLights();

  void renderSolid();
  void render2D();
  void renderWireFrame();
};

// ****************************************************************************

#endif
