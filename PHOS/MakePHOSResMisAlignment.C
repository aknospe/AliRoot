void MakePHOSResMisAlignment(){
  // Create TClonesArray of residual misalignment objects for PHOS
  //
  const AliPHOSGeometry *phosGeom = AliPHOSGeometry::GetInstance("IHEP", "IHEP");
  if (!phosGeom) {
    Error("MakePHOSFullMisAlignment", "Cannot obtain AliPHOSGeometry singleton\n");
    return;
  }

  AliPHOSEMCAGeometry *emca = phosGeom->GetEMCAGeometry();
  TClonesArray *array = new TClonesArray("AliAlignObjParams", 16 + phosGeom->GetNModules() * 
                                         emca->GetNStripX() * emca->GetNStripZ());
  TClonesArray &alobj = *array;
   
  Double_t dpsi=0., dtheta=0., dphi=0.;
  Double_t displacement = 0.2;
  Int_t iIndex=0; // let all modules have index=0 in a layer with no LUT
  AliGeomManager::ELayerID iLayer = AliGeomManager::kInvalidLayer;
  UShort_t volid = AliGeomManager::LayerToVolUID(iLayer,iIndex);
 
  Int_t i=0 ;
  // Alignment for 5 PHOS modules
  new(alobj[i++]) AliAlignObjParams("PHOS/Module1",
        volid, -0.20, -0.1, +0.0, dpsi, dtheta, 0.2, kTRUE);
  new(alobj[i++]) AliAlignObjParams("PHOS/Module2",
	volid, -0.10, +0.0, -0.2, dpsi, dtheta, 0.2, kTRUE);
  new(alobj[i++]) AliAlignObjParams("PHOS/Module3",
	volid,  0.05, -0.1,  0.2, dpsi, dtheta, 0.0, kTRUE);
  new(alobj[i++]) AliAlignObjParams("PHOS/Module4",
	volid, +0.10, -0.0, -0.1, dpsi, dtheta, 0.1, kTRUE);
  new(alobj[i++]) AliAlignObjParams("PHOS/Module5",
	volid, +0.20, -0.1,  0.1, dpsi, dtheta, 0.2, kTRUE);
 
  Double_t dx=0., dy=0., dz=0. ;
  // Alignment of CPV modules
  new(alobj[i++]) AliAlignObjParams("PHOS/Module1/CPV",
        volid, dx, dy, dz, dpsi, dtheta, dphi, kTRUE);
  new(alobj[i++]) AliAlignObjParams("PHOS/Module2/CPV",
        volid, dx, dy, dz, dpsi, dtheta, dphi, kTRUE);
  new(alobj[i++]) AliAlignObjParams("PHOS/Module3/CPV",
        volid, dx, dy, dz, dpsi, dtheta, dphi, kTRUE);
  new(alobj[i++]) AliAlignObjParams("PHOS/Module4/CPV",
        volid, dx, dy, dz, dpsi, dtheta, dphi, kTRUE);
  new(alobj[i++]) AliAlignObjParams("PHOS/Module5/CPV",
        volid, dx, dy, dz, dpsi, dtheta, dphi, kTRUE);
 

  // Alignment for PHOS cradle
  new(alobj[i++]) AliAlignObjParams("PHOS/Cradle0",
	volid, 0., 0., -displacement, dpsi, dtheta, dphi, kTRUE);
  new(alobj[i++]) AliAlignObjParams("PHOS/Cradle1",
	volid, 0., 0., +displacement, dpsi, dtheta, dphi, kTRUE);

  // Alignment for cradle wheels
  new(alobj[i++])  AliAlignObjParams("PHOS/Wheel0",
	volid, 0., 0., -displacement, dpsi, dtheta, dphi, kTRUE);
  new(alobj[i++])  AliAlignObjParams("PHOS/Wheel1",
	volid, 0., 0., -displacement, dpsi, dtheta, dphi, kTRUE);
  new(alobj[i++])  AliAlignObjParams("PHOS/Wheel2",
	volid, 0., 0., +displacement, dpsi, dtheta, dphi, kTRUE);
  new(alobj[i++]) AliAlignObjParams("PHOS/Wheel3",
	volid, 0., 0., +displacement, dpsi, dtheta, dphi, kTRUE);

  AliPHOSSurvey geodesicData("phos_mod3_survey.txt");
  geodesicData.CreateAliAlignObjParams(alobj);

  const char* macroname = "MakePHOSResMisAlignment.C";
  if( TString(gSystem->Getenv("TOCDB")) != TString("kTRUE") ){
    // save on file
    const char* filename = "PHOSresidualMisalignment.root";
    TFile f(filename,"RECREATE");
    if(!f){
      Error(macroname,"cannot open file for output\n");
      return;
    }
    Info(macroname,"Saving alignment objects to the file %s", filename);
    f.cd();
    f.WriteObject(array,"PHOSAlignObjs","kSingleKey");
    f.Close();
  }else{
    // save in CDB storage
    TString Storage = gSystem->Getenv("STORAGE");
    if(!Storage.BeginsWith("local://") && !Storage.BeginsWith("alien://")) {
      Error(macroname,"STORAGE variable set to %s is not valid. Exiting\n",Storage.Data());
      return;
    }
    Info(macroname,"Saving alignment objects in CDB storage %s",
	 Storage.Data());
    AliCDBManager* cdb = AliCDBManager::Instance();
    AliCDBStorage* storage = cdb->GetStorage(Storage.Data());
    if(!storage){
      Error(macroname,"Unable to open storage %s\n",Storage.Data());
      return;
    }
    AliCDBMetaData *md= new AliCDBMetaData();
    md->SetResponsible("Yuri Kharlov");
    md->SetComment("Alignment objects for slightly misaligned geometry (residual misalignment");
    md->SetAliRootVersion(gSystem->Getenv("ARVERSION"));
    AliCDBId id("PHOS/Align/Data",0,AliCDBRunRange::Infinity());
    storage->Put(array,id, md);
  }

  array->Delete();

}
