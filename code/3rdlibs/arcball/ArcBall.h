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

#pragma once

#ifndef ARCBALL_H
#define ARCBALL_H

#include "StdDefines.h"
#include "Vectors.h"

/** \class ArcBall */
class ArcBall
{
public:
  ArcBall(int iWinWidth=0, int iWinHeight=0, int iWinOffsetX=0, int iWinOffsetY=0, bool bUseTranslation = true);
  //mouse event
  void 	OnMouseMove (int x, int y);
  void 	OnLeftButtonDown (int x, int y);
  void 	OnLeftButtonUp (int x, int y);
  void 	OnMiddleButtonDown (int x, int y);
  void 	OnMiddleButtonUp (int x, int y);
  void 	OnRightButtonDown (int x, int y);
  void 	OnRightButtonUp (int x, int y);

  //set get
  void SetRadius(float fRadius) {m_fRadius = fRadius;}
  void SetTranslation(const FLOATMATRIX4& mTranslation) {m_mTranslation = mTranslation;}
  void SetUseTranslation(bool bUseTranslation) {m_bUseTranslation = bUseTranslation;}
  const FLOATMATRIX4& GetTranslation() const {return m_mTranslation;}
  float* GetTranslationVec() {return m_translateVec;}

  FLOATMATRIX4 GetRotation() {return m_rotationMat;}
 bool GetUseTranslation() {return m_bUseTranslation;}
  void SetWindowSize(int iWinWidth, int iWinHeight);
  void SetWindowOffset(int iWinOffsetX, int iWinOffsetY);

  void Click(INTVECTOR2 vPosition);
  FLOATQUATERNION4 Drag(INTVECTOR2 vPosition);

protected:
  static float ms_fEpsilon;

  FLOATVECTOR3 m_vStartDrag;   ///< Saved click vector  //last location
  INTVECTOR2  m_iWinDim;      ///< window dimensions
  INTVECTOR2   m_iWinOffsets;  ///< Horizontal/Vertical window offset
  float        m_fRadius;      ///< radius of the ball
  bool         m_bUseTranslation; ///< whether to the translation of the ball

  FLOATMATRIX4 m_mTranslation; ///< translation of the ball

  //returned stuff
  FLOATMATRIX4 m_rotationMat;
  float m_translateVec[3];

  //my stuff, translate xy z
  INTVECTOR2 m_lastPosition;

  //mouse status
  bool m_IsLBdown, m_IsMBdown, m_IsRBdown;


  FLOATVECTOR3 MapToSphere(INTVECTOR2 vPosition) const;
};

#endif // ARCBALL_H
