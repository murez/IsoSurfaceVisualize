/*
   For more information, please see: http://software.sci.utah.edu

   The MIT License

   Copyright (c) 2008 Scientific Computing and Imaging Institute,
   University of Utah.


   Permission is hereby granted, free of charge, to any person obtaining a
   copy of this software and associated documentation files (the "Software"),
   to deal in the Software without restriction, including without limitation
   the rights to use, copy, modify, merge, publish, distribute, sublicense,
   and/or sell copies of the Software, and to permit persons to whom the
   Software is furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included
   in all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
   OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
   THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
   FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
   DEALINGS IN THE SOFTWARE.
*/

/**
  \file    ArcBall.cpp
  \author  Jens Krueger
           SCI Institute
           University of Utah
           Based on the NeHe Tutorial 48
  \date    October 2008
*/

#include "ArcBall.h"

float ArcBall::ms_fEpsilon = 1.0e-5f;

ArcBall::ArcBall(int iWinWidth, int iWinHeight, int iWinOffsetX, int iWinOffsetY, bool bUseTranslation) :
    m_vStartDrag(),
    m_iWinDim(iWinWidth, iWinHeight),
    m_iWinOffsets(iWinOffsetX, iWinOffsetY),
    m_fRadius(1.0f),
    m_bUseTranslation(bUseTranslation),
  m_IsLBdown(false), m_IsMBdown(false), m_IsRBdown(false)
{
  m_translateVec[0]=0;
  m_translateVec[1]=0;
  m_translateVec[2]=0;
}

void ArcBall::SetWindowSize(int iWinWidth, int iWinHeight) {
    m_iWinDim = INTVECTOR2(iWinWidth, iWinHeight);
}

void ArcBall::SetWindowOffset(int iWinOffsetX, int iWinOffsetY) {
  m_iWinOffsets = INTVECTOR2(iWinOffsetX, iWinOffsetY);
}

void ArcBall::Click(INTVECTOR2 vPosition) {

  m_vStartDrag = MapToSphere(vPosition);
}

FLOATQUATERNION4 ArcBall::Drag(INTVECTOR2 vPosition) {
  FLOATQUATERNION4 qRotation;

  // Map the point to the sphere
  FLOATVECTOR3 vCurrent = MapToSphere(vPosition);

  // Compute the vector perpendicular to the begin and end vectors
  FLOATVECTOR3 vCross(vCurrent % m_vStartDrag);
  float        fDot(vCurrent ^ m_vStartDrag);
  m_vStartDrag = vCurrent; //update the start position

  FLOATQUATERNION4 rotQ;
  if (vCross.length() > ms_fEpsilon)    //if its non-zero
      rotQ =  FLOATQUATERNION4(vCross.x, vCross.y, vCross.z, fDot);
  else
      rotQ =  FLOATQUATERNION4(0,0,0,0);

  FLOATMATRIX4 rotMat = rotQ.ComputeRotation();
  m_rotationMat = m_rotationMat*rotMat;

  return rotQ;
}

FLOATVECTOR3 ArcBall::MapToSphere(INTVECTOR2 vPosition) const {
  FLOATVECTOR3 vResult;

  // normalize position to [-1 ... 1]
  FLOATVECTOR2 vNormPosition;
  vNormPosition.x =  -(((vPosition.x-m_iWinOffsets.x) / (float(m_iWinDim.x - 1) / 2.0f)) - 1.0f);
  vNormPosition.y =  ((vPosition.y-m_iWinOffsets.y) / (float(m_iWinDim.y - 1) / 2.0f)) - 1.0f;

  if (m_bUseTranslation) {
    FLOATMATRIX4 mTranslation(m_mTranslation);
    mTranslation.m43 = 0;
    vNormPosition = (FLOATVECTOR4(vNormPosition,0.0f,1.0f) * mTranslation).xy();
  }

  // Compute the length of the vector to the point from the center
  float length = vNormPosition.length();

  // If the point is mapped outside of the sphere... (length > radius)
  if (length > m_fRadius) {
      // Compute a normalizing factor (radius / length)
      float norm = float(m_fRadius / length);

      // Return the "normalized" vector, a point on the sphere
      vResult.x = vNormPosition.x * norm;
      vResult.y = vNormPosition.y * norm;
      vResult.z = 0.0f;
  } else    // Else it's on the inside
  {
      // Return a vector to a point mapped inside the sphere
      vResult.x = vNormPosition.x;
      vResult.y = vNormPosition.y;
      vResult.z = length-m_fRadius;
  }

  return vResult;
}



//=======
void 	ArcBall::OnMouseMove (int x, int y)
{
  INTVECTOR2 vp(x,y);
  if(m_IsLBdown)
  {
    Drag(vp);
  }

  //TODO the moving is too slow when the object is far away from camera
  if(m_IsMBdown)
  {
    //translate rotate
    //uint !!!! big time
    m_translateVec[0] += float(float(vp.x)/float(m_iWinDim.x) - float(m_lastPosition.x)/float(m_iWinDim.x));
    m_translateVec[1] -= float(float(vp.y)/float(m_iWinDim.y) - float(m_lastPosition.y)/float(m_iWinDim.y));

    m_lastPosition = vp;
  }

  if(m_IsRBdown)
  {
    if(y-m_lastPosition.y > 0)
      m_translateVec[2] -= 3*float(y-m_lastPosition.y)/float(m_iWinDim.y);
    else if(y-m_lastPosition.y<0)
      m_translateVec[2] += 3*float(m_lastPosition.y-y)/float(m_iWinDim.y);

    m_lastPosition = vp;
  }

}

void 	ArcBall::OnLeftButtonDown (int x, int y)
{
  INTVECTOR2 vp(x,y);
  m_IsLBdown = true;
  Click(vp);
}
void 	ArcBall::OnMiddleButtonDown (int x, int y)
{
  INTVECTOR2 vp(x,y);
  m_IsMBdown = true;
  m_lastPosition = vp;
}
void 	ArcBall::OnRightButtonDown (int x, int y)
{
  INTVECTOR2 vp(x,y);
  m_IsRBdown = true;
  m_lastPosition = vp;
}
//Up============================
void 	ArcBall::OnLeftButtonUp (int x, int y)
{
  INTVECTOR2 vp(x,y);
  m_IsLBdown = false;
}
void 	ArcBall::OnMiddleButtonUp (int x, int y)
{
  INTVECTOR2 vp(x,y);
  m_IsMBdown = false;
}
void 	ArcBall::OnRightButtonUp (int x, int y)
{
  INTVECTOR2 vp(x,y);
  m_IsRBdown = false;
}
