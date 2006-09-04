/**************************************************************************
 * Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 *                                                                        *
 * Author: The ALICE Off-line Project.                                    *
 * Contributors are mentioned in the code where appropriate.              *
 *                                                                        *
 * Permission to use, copy, modify and distribute this software and its   *
 * documentation strictly for non-commercial purposes is hereby granted   *
 * without fee, provided that the above copyright notice appears in all   *
 * copies and that both the copyright notice and this permission notice   *
 * appear in the supporting documentation. The authors make no claims     *
 * about the suitability of this software for any purpose. It is          *
 * provided "as is" without express or implied warranty.                  *
 **************************************************************************/

/* $Id$ */

//-------------------------------------------------------------------------
//               Implementation of the ESD V0 vertex class
//            This class is part of the Event Data Summary
//            set of classes and contains information about
//            V0 kind vertexes generated by a neutral particle
//     Origin: Iouri Belikov, IReS, Strasbourg, Jouri.Belikov@cern.ch
//-------------------------------------------------------------------------

#include <Riostream.h>
#include <TMath.h>
#include <TDatabasePDG.h>
#include <TPDGCode.h>
#include <TParticlePDG.h>

#include "AliLog.h"
#include "AliESDv0.h"
#include "AliExternalTrackParam.h"

ClassImp(AliESDv0)

AliESDv0::AliESDv0() :
  TObject(),
  fPdgCode(kK0Short),
  fEffMass(TDatabasePDG::Instance()->GetParticle(kK0Short)->Mass()),
  fDcaV0Daughters(0),
  fChi2V0(1.e+33),
  fNidx(0),
  fPidx(0)
{
  //--------------------------------------------------------------------
  // Default constructor  (K0s)
  //--------------------------------------------------------------------

  for (Int_t i=0; i<3; i++) {
    fPos[i] = 0.;
    fNmom[i] = 0.;
    fPmom[i] = 0.;
  }

  for (Int_t i=0; i<6; i++) {
    fPosCov[i]= 0.;
    fNmomCov[i] = 0.;
    fPmomCov[i] = 0.;
  }
}

AliESDv0::AliESDv0(const AliESDv0& rAliESDv0) :
  TObject(rAliESDv0),
  fPdgCode(rAliESDv0.fPdgCode),
  fEffMass(rAliESDv0.fEffMass),
  fDcaV0Daughters(rAliESDv0.fDcaV0Daughters),
  fChi2V0(rAliESDv0.fChi2V0),
  fNidx(rAliESDv0.fNidx),
  fPidx(rAliESDv0.fPidx)
{

  for (int i=0; i<3; i++) {
    fPos[i]  = rAliESDv0.fPos[i];
    fNmom[i] = rAliESDv0.fNmom[i];
    fPmom[i] = rAliESDv0.fPmom[i];
  }
  for (int i=0; i<6; i++) {
    fPosCov[i]  = rAliESDv0.fPosCov[i];
    fNmomCov[i] = rAliESDv0.fNmomCov[i];
    fPmomCov[i] = rAliESDv0.fPmomCov[i];
  }
}


AliESDv0& AliESDv0::operator=(const AliESDv0& rAliESDv0)
{
  if (this!=&rAliESDv0) {
    TObject::operator=(rAliESDv0);
    fPdgCode        = rAliESDv0.fPdgCode;
    fEffMass        = rAliESDv0.fEffMass;
    fDcaV0Daughters = rAliESDv0.fDcaV0Daughters;
    fChi2V0         = rAliESDv0.fChi2V0;
    fNidx           = rAliESDv0.fNidx;
    fPidx           = rAliESDv0.fPidx;

    for (int i=0; i<3; i++) {
      fPos[i]  = rAliESDv0.fPos[i];
      fNmom[i] = rAliESDv0.fNmom[i];
      fPmom[i] = rAliESDv0.fPmom[i];
    }
    for (int i=0; i<6; i++) {
      fPosCov[i]  = rAliESDv0.fPosCov[i];
      fNmomCov[i] = rAliESDv0.fNmomCov[i];
      fPmomCov[i] = rAliESDv0.fPmomCov[i];
    }
  }
  return *this;
}

AliESDv0::AliESDv0(const AliExternalTrackParam &t1, Int_t i1,
                   const AliExternalTrackParam &t2, Int_t i2) :
  TObject(),
  fPdgCode(kK0Short),
  fEffMass(TDatabasePDG::Instance()->GetParticle(kK0Short)->Mass()),
  fDcaV0Daughters(0),
  fChi2V0(1.e+33),
  fNidx(i1),
  fPidx(i2)
{
  //--------------------------------------------------------------------
  // Main constructor  (K0s)
  //--------------------------------------------------------------------

  for (Int_t i=0; i<6; i++) {
    fPosCov[i]= 0.;
    fNmomCov[i] = 0.;
    fPmomCov[i] = 0.;
  }

  //Trivial estimation of the vertex parameters
  Double_t x=t1.GetX(), alpha=t1.GetAlpha();
  const Double_t *par=t1.GetParameter();
  Double_t pt=1./TMath::Abs(par[4]), 
           phi=TMath::ASin(par[2]) + alpha, 
           cs=TMath::Cos(alpha), sn=TMath::Sin(alpha);

  Double_t px1=pt*TMath::Cos(phi), py1=pt*TMath::Sin(phi), pz1=pt*par[3];
  Double_t x1=x*cs - par[0]*sn;
  Double_t y1=x*sn + par[0]*cs;
  Double_t z1=par[1];
  const Double_t ss=0.0005*0.0005;//a kind of a residual misalignment precision
  Double_t sx1=sn*sn*t1.GetSigmaY2()+ss, sy1=cs*cs*t1.GetSigmaY2()+ss; 



  x=t2.GetX(); alpha=t2.GetAlpha(); par=t2.GetParameter();
  pt=1./TMath::Abs(par[4]);
  phi=TMath::ASin(par[2]) + alpha;  
  cs=TMath::Cos(alpha); sn=TMath::Sin(alpha);

  Double_t px2=pt*TMath::Cos(phi), py2=pt*TMath::Sin(phi), pz2=pt*par[3];
  Double_t x2=x*cs - par[0]*sn;
  Double_t y2=x*sn + par[0]*cs;
  Double_t z2=par[1];
  Double_t sx2=sn*sn*t2.GetSigmaY2()+ss, sy2=cs*cs*t2.GetSigmaY2()+ss; 
    
  Double_t sz1=t1.GetSigmaZ2(), sz2=t2.GetSigmaZ2();
  Double_t wx1=sx2/(sx1+sx2), wx2=1.- wx1;
  Double_t wy1=sy2/(sy1+sy2), wy2=1.- wy1;
  Double_t wz1=sz2/(sz1+sz2), wz2=1.- wz1;
  fPos[0]=wx1*x1 + wx2*x2; fPos[1]=wy1*y1 + wy2*y2; fPos[2]=wz1*z1 + wz2*z2;

  //fPos[0]=0.5*(x1+x2); fPos[1]=0.5*(y1+y2); fPos[2]=0.5*(z1+z2);
  fNmom[0]=px1; fNmom[1]=py1; fNmom[2]=pz1; 
  fPmom[0]=px2; fPmom[1]=py2; fPmom[2]=pz2;

  Double_t e1=TMath::Sqrt(0.13957*0.13957 + px1*px1 + py1*py1 + pz1*pz1);
  Double_t e2=TMath::Sqrt(0.13957*0.13957 + px2*px2 + py2*py2 + pz2*pz2);
  fEffMass=TMath::Sqrt((e1+e2)*(e1+e2)-
    (px1+px2)*(px1+px2)-(py1+py2)*(py1+py2)-(pz1+pz2)*(pz1+pz2));

  fChi2V0=7.;   

}

AliESDv0::~AliESDv0(){
  //--------------------------------------------------------------------
  // Empty destructor
  //--------------------------------------------------------------------
}



Double_t AliESDv0::ChangeMassHypothesis(Int_t code) {
  //--------------------------------------------------------------------
  // This function changes the mass hypothesis for this V0
  // and returns the "kinematical quality" of this hypothesis 
  //--------------------------------------------------------------------
  Double_t nmass=0.13957, pmass=0.13957, mass=0.49767, ps=0.206;

  fPdgCode=code;

  switch (code) {
  case kLambda0:
    nmass=0.13957; pmass=0.93827; mass=1.1157; ps=0.101; break;
  case kLambda0Bar:
    pmass=0.13957; nmass=0.93827; mass=1.1157; ps=0.101; break;
  case kK0Short: 
    break;
  default:
    AliError("invalide PDG code ! Assuming K0s...");
    fPdgCode=kK0Short;
    break;
  }

  Double_t pxn=fNmom[0], pyn=fNmom[1], pzn=fNmom[2]; 
  Double_t pxp=fPmom[0], pyp=fPmom[1], pzp=fPmom[2];

  Double_t en=TMath::Sqrt(nmass*nmass + pxn*pxn + pyn*pyn + pzn*pzn);
  Double_t ep=TMath::Sqrt(pmass*pmass + pxp*pxp + pyp*pyp + pzp*pzp);
  Double_t pxl=pxn+pxp, pyl=pyn+pyp, pzl=pzn+pzp;
  Double_t pl=TMath::Sqrt(pxl*pxl + pyl*pyl + pzl*pzl);

  fEffMass=TMath::Sqrt((en+ep)*(en+ep)-pl*pl);

  Double_t beta=pl/(en+ep);
  Double_t pln=(pxn*pxl + pyn*pyl + pzn*pzl)/pl;
  Double_t plp=(pxp*pxl + pyp*pyl + pzp*pzl)/pl;

  Double_t pt2=pxp*pxp + pyp*pyp + pzp*pzp - plp*plp;

  Double_t a=(plp-pln)/(plp+pln);
  a -= (pmass*pmass-nmass*nmass)/(mass*mass);
  a = 0.25*beta*beta*mass*mass*a*a + pt2;

  return (a - ps*ps);
  
}

void AliESDv0::GetPxPyPz(Double_t &px, Double_t &py, Double_t &pz) const {
  //--------------------------------------------------------------------
  // This function returns V0's momentum (global)
  //--------------------------------------------------------------------
  px=fNmom[0]+fPmom[0]; 
  py=fNmom[1]+fPmom[1]; 
  pz=fNmom[2]+fPmom[2]; 
}

void AliESDv0::GetXYZ(Double_t &x, Double_t &y, Double_t &z) const {
  //--------------------------------------------------------------------
  // This function returns V0's position (global)
  //--------------------------------------------------------------------
  x=fPos[0]; 
  y=fPos[1]; 
  z=fPos[2]; 
}

Double_t AliESDv0::GetD(Double_t x0, Double_t y0, Double_t z0) const {
  //--------------------------------------------------------------------
  // This function returns V0's impact parameter
  //--------------------------------------------------------------------
  Double_t x=fPos[0],y=fPos[1],z=fPos[2];
  Double_t px=fNmom[0]+fPmom[0];
  Double_t py=fNmom[1]+fPmom[1];
  Double_t pz=fNmom[2]+fPmom[2];

  Double_t dx=(y0-y)*pz - (z0-z)*py; 
  Double_t dy=(x0-x)*pz - (z0-z)*px;
  Double_t dz=(x0-x)*py - (y0-y)*px;
  Double_t d=TMath::Sqrt((dx*dx+dy*dy+dz*dz)/(px*px+py*py+pz*pz));
  return d;
}


Double_t AliESDv0::GetV0CosineOfPointingAngle(Double_t& refPointX, Double_t& refPointY, Double_t& refPointZ) const {
  // calculates the pointing angle of the V0 wrt a reference point

  Double_t momV0[3]; //momentum of the V0
  GetPxPyPz(momV0[0],momV0[1],momV0[2]);

  Double_t deltaPos[3]; //vector between the reference point and the V0 vertex
  deltaPos[0] = fPos[0] - refPointX;
  deltaPos[1] = fPos[1] - refPointY;
  deltaPos[2] = fPos[2] - refPointZ;

  Double_t momV02    = momV0[0]*momV0[0] + momV0[1]*momV0[1] + momV0[2]*momV0[2];
  Double_t deltaPos2 = deltaPos[0]*deltaPos[0] + deltaPos[1]*deltaPos[1] + deltaPos[2]*deltaPos[2];

  Double_t cosinePointingAngle = (deltaPos[0]*momV0[0] +
				  deltaPos[1]*momV0[1] +
				  deltaPos[2]*momV0[2] ) /
    TMath::Sqrt(momV02 * deltaPos2);
  
  return cosinePointingAngle;
}
