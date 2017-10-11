void TMVAExample( )
{

  TFile* outputFile = TFile::Open( "TMVA.root", "RECREATE" );
  
  TMVA::Factory *factory = new TMVA::Factory( "MVAnalysis", outputFile,"!V:Transformations=I;N;D");

  TFile *input = TFile::Open("inputfilename.root");
  
  factory->AddVariable("var0", 'F');
  factory->AddVariable("var1+var2, or any other combination", 'F');   
  
  factory->AddSignalTree ( (TTree*)input->Get("TreeS") );
  factory->AddBackgroundTree ( (TTree*)input->Get("TreeB") );

  factory->PrepareTrainingAndTestTree( "", "",
				       "nTrain_Signal=200:nTrain_Background=200:nTest_Signal=200:nTest_Background=200:!V" );

  factory->BookMethod( TMVA::Types::kLikelihood, "Likelihood",
  "!V:NAvEvtPerBin=50" );

  factory->BookMethod( TMVA::Types::kMLP, "MLP", 
  "!V:NCycles=50:HiddenLayers=10,10:TestRate=5" );

  factory->BookMethod( TMVA::Types::kBDT, "BDT", 
  "!V:BoostType=Grad:nCuts=20:NNodesMax=5" );
  
  factory->TrainAllMethods();  
  factory->TestAllMethods();
  factory->EvaluateAllMethods();
  
  outputFile->Close();
  std::cout << "Caution, this was example code, please edit for real use!"<<std::endl;
  delete factory;
}
