/**********************************************************************************
 * Project   : TMVA - a Root-integrated toolkit for multivariate data analysis    *
 * Package   : TMVA                                                               *
 * Exectuable: TMVApplicationAlternative                                          *
 *                                                                                *
 * This exectutable provides a simple example on how to use the trained MVAs      *
 * within a C++ analysis module                                                   *
 **********************************************************************************/

#include <iostream> // Stream declarations

#include "TFile.h"
#include "TH1F.h"
#include "TTree.h"
#include "TMVA/Reader.h"
#include "TMVA/IMethod.h"
#include "TMVA/MethodCuts.h"

using namespace std;

// ---------------------------------------------------------------
// choose MVA methods to be trained + tested
Bool_t Use_Cuts           = 0;
Bool_t Use_Likelihood     = 1;
Bool_t Use_LikelihoodD    = 0;
Bool_t Use_PDERS          = 0;
Bool_t Use_HMatrix        = 0;
Bool_t Use_Fisher         = 1;
Bool_t Use_MLP            = 0;
Bool_t Use_CFMlpANN       = 0;
Bool_t Use_TMlpANN        = 0;
Bool_t Use_BDT            = 0;
// ---------------------------------------------------------------

int main( void ) 
{
   cout << endl;
   cout << "==> start TMVApplicationAlternative" << endl;

   //
   // create TMVA::Reader object
   //
   TMVA::Reader *reader = new TMVA::Reader();    

   // create a set of variables and declare them to the reader
   // - the variable names must corresponds in name and type to 
   // those given in the weight file(s) that you use
   Float_t var1, var2, var3, var4;
   reader->AddVariable( "var1", &var1 );
   reader->AddVariable( "var2", &var2 );
   reader->AddVariable( "var3", &var3 );
   reader->AddVariable( "var4", &var4 );

   //
   // book TMVA methods
   //
   string dir    = "../examples/weights/";
   string prefix = "MVAnalysis";

   TMVA::IMethod* methodCut = Use_Cuts        ? reader->BookMVA( "Cuts method",         dir + prefix + "_Cuts.weights.txt"        ) : 0;
   TMVA::IMethod* methodLk  = Use_Likelihood  ? reader->BookMVA( "Likelihood method",   dir + prefix + "_Likelihood.weights.txt"  ) : 0;
   TMVA::IMethod* methodLkD = Use_LikelihoodD ? reader->BookMVA( "LikelihoodD method",  dir + prefix + "_LikelihoodD.weights.txt" ) : 0;
   TMVA::IMethod* methodPd  = Use_PDERS       ? reader->BookMVA( "PDERS method",        dir + prefix + "_PDERS.weights.txt"       ) : 0;
   TMVA::IMethod* methodHm  = Use_HMatrix     ? reader->BookMVA( "HMatrix method",      dir + prefix + "_HMatrix.weights.txt"     ) : 0;
   TMVA::IMethod* methodFi  = Use_Fisher      ? reader->BookMVA( "Fisher method",       dir + prefix + "_Fisher.weights.txt"      ) : 0;
   TMVA::IMethod* methodNn  = Use_MLP         ? reader->BookMVA( "MLP method",          dir + prefix + "_MLP.weights.txt"         ) : 0;
   TMVA::IMethod* methodNnC = Use_CFMlpANN    ? reader->BookMVA( "CFMlpANN method",     dir + prefix + "_CFMlpANN.weights.txt"    ) : 0;
   TMVA::IMethod* methodNnT = Use_TMlpANN     ? reader->BookMVA( "TMlpANN method",      dir + prefix + "_TMlpANN.weights.txt"     ) : 0;
   TMVA::IMethod* methodBdt = Use_BDT         ? reader->BookMVA( "BDT method",          dir + prefix + "_BDT.weights.txt"         ) : 0;
  
   // 
   // book output histograms
   TH1F* histLk  = Use_Likelihood  ? new TH1F( "MVA_Likelihood",  "MVA_Likelihood",  100, -1, 1 ) : 0;
   TH1F* histLkD = Use_LikelihoodD ? new TH1F( "MVA_LikelihoodD", "MVA_LikelihoodD", 100, -1, 1 ) : 0;
   TH1F* histPd  = Use_PDERS       ? new TH1F( "MVA_PDERS",       "MVA_PDERS",       100, -1, 1 ) : 0;
   TH1F* histHm  = Use_HMatrix     ? new TH1F( "MVA_HMatrix",     "MVA_HMatrix",     100, -4, 4 ) : 0;
   TH1F* histFi  = Use_Fisher      ? new TH1F( "MVA_Fisher",      "MVA_Fisher",      100, -4, 4 ) : 0;
   TH1F* histNn  = Use_MLP         ? new TH1F( "MVA_MLP",         "MVA_MLP",         100, -1, 1 ) : 0;
   TH1F* histNnC = Use_CFMlpANN    ? new TH1F( "MVA_CFMlpANN",    "MVA_CFMlpANN",    100, -1, 1 ) : 0;
   TH1F* histNnT = Use_TMlpANN     ? new TH1F( "MVA_TMlpANN",     "MVA_TMlpANN",     100, -1, 1 ) : 0;
   TH1F* histBdt = Use_BDT         ? new TH1F( "MVA_BDT",         "MVA_BDT",         100, -1, 1 ) : 0;

   //
   // Prepare input tree (this must be replaced by your data source)
   // in this example, there is a toy tree with signal and one with background events
   // we'll later on use only the "signal" events for the test in this example.
   //   
   TFile *input      = new TFile("../examples/data/toy_sigbkg.root");
   TTree *signal     = (TTree*)input->Get("TreeS");
   TTree *background = (TTree*)input->Get("TreeB");  

   //
   // prepare the tree
   // - here the variable names have to corresponds to your tree
   // - you can use the same variables as above which is slightly faster,
   //   but of course you can use different ones and copy the values inside
   //   the event loop
   // - if you use the same variables as above make sure they have the same 
   //   type as in the tree (currently only float or int)
   TTree* theTree = signal;
   theTree->SetBranchAddress( "var1", &var1 );
   theTree->SetBranchAddress( "var2", &var2 );
   theTree->SetBranchAddress( "var3", &var3 );
   theTree->SetBranchAddress( "var4", &var4 );

   // efficiency calculator for cut method
   int nSelCuts = 0;
   if (Use_Cuts) {
      // give the desired signal efficienciy
      double effS = 0.7;
      ((TMVA::MethodCuts*)methodCut)->SetTestSignalEfficiency( effS );
   }

   cout << "--- processing: " << theTree->GetEntries() << " events" << endl;
   for (Long64_t ievt=2000; ievt<theTree->GetEntries();ievt++) {

      theTree->GetEntry(ievt);
    
      if (ievt%1000 == 0) cout << "--- ... processing event: " << ievt << endl;

      // 
      // return the MVA
      // 
      if (Use_Cuts) {
         // check if event passes cuts
         Bool_t passed = methodCut->GetMvaValue();
         if (passed) nSelCuts++;
      }

      double effS   = 0.9; // give the desired signal efficiency for Cuts method

      if (Use_Likelihood ) histLk ->Fill( methodLk ->GetMvaValue() );
      if (Use_LikelihoodD) histLkD->Fill( methodLkD->GetMvaValue() );
      if (Use_PDERS      ) histPd ->Fill( methodPd ->GetMvaValue() );
      if (Use_HMatrix    ) histHm ->Fill( methodHm ->GetMvaValue() );
      if (Use_Fisher     ) histFi ->Fill( methodFi ->GetMvaValue() );
      if (Use_MLP        ) histNn ->Fill( methodNn ->GetMvaValue() );
      if (Use_CFMlpANN   ) histNnC->Fill( methodNnC->GetMvaValue() );
      if (Use_TMlpANN    ) histNnT->Fill( methodNnT->GetMvaValue() );
      if (Use_BDT        ) histBdt->Fill( methodBdt->GetMvaValue() );
        
   }
   cout << "--- end of event loop" << endl;
   if (Use_Cuts) cout << "--- efficiency for cut method: " << double(nSelCuts)/theTree->GetEntries()
                      << endl;

   //
   // write histograms
   //
   TFile *target  = new TFile( "TMVApp.root","RECREATE" );
   if (Use_Likelihood  ) histLk ->Write();
   if (Use_LikelihoodD ) histLkD->Write();
   if (Use_PDERS       ) histPd ->Write();
   if (Use_HMatrix     ) histHm ->Write();
   if (Use_Fisher      ) histFi ->Write();
   if (Use_MLP         ) histNn ->Write();
   if (Use_CFMlpANN    ) histNnC->Write();
   if (Use_TMlpANN     ) histNnT->Write();
   if (Use_BDT         ) histBdt->Write();
   target->Close();

   cout << "--- created root file: \"TMVApp.root\" containing the MVA output histograms" << endl;
  
   delete reader;
    
   cout << "==> TMVApplicationAlternative is done!" << endl << endl;
} 

