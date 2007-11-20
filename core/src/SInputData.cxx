// $Id: SInputData.cxx,v 1.1.1.1 2007-11-13 12:42:21 krasznaa Exp $
/***************************************************************************
 * @Project: SFrame - ROOT-based analysis framework for ATLAS
 * @Package: Core
 *
 * @author Stefan Ask       <Stefan.Ask@cern.ch>           - Manchester
 * @author David Berge      <David.Berge@cern.ch>          - CERN
 * @author Johannes Haller  <Johannes.Haller@cern.ch>      - Hamburg
 * @author Andreas Hoecker  <Andreas.Hocker@cern.ch>       - CERN
 * @author A. Krasznahorkay <Attila.Krasznahorkay@cern.ch> - CERN/Debrecen
 *
 ***************************************************************************/

// Local include(s):
#include "../include/SInputData.h"

using namespace std;

SFile& SFile::operator= ( const SFile& parent ) {

   this->file = parent.file;
   this->lumi = parent.lumi;
   this->events = parent.events;

   return *this;

}

Bool_t SFile::operator== ( const SFile& rh ) const {

   if( ( this->file == rh.file ) && ( this->lumi == rh.lumi ) &&
       ( this->events == rh.events ) ) {
      return kTRUE;
   } else {
      return kFALSE;
   }

}

Bool_t SFile::operator!= ( const SFile& rh ) const {

   return ( ! ( *this == rh ) );

}

STree& STree::operator= ( const STree& parent ) {

   this->treeName = parent.treeName;

   return *this;

}

Bool_t STree::operator== ( const STree& rh ) const {

   if( this->treeName == rh.treeName ) {
      return kTRUE;
   } else {
      return kFALSE;
   }

}

Bool_t STree::operator!= ( const STree& rh ) const {

   return ( ! ( *this == rh ) );

}

SEVTree& SEVTree::operator= ( const SEVTree& parent ) {

   this->treeName = parent.treeName;
   this->treeBaseName = parent.treeBaseName;
   this->viewNumber = parent.viewNumber;
   this->collTreeName = parent.collTreeName;

   return *this;

}

Bool_t SEVTree::operator== ( const SEVTree& rh ) const {

   if( ( this->treeName == rh.treeName ) &&
       ( this->treeBaseName == rh.treeBaseName ) && ( this->viewNumber == rh.viewNumber ) &&
       ( this->collTreeName == rh.collTreeName ) ) {
      return kTRUE;
   } else {
      return kFALSE;
   }

}

Bool_t SEVTree::operator!= ( const SEVTree& rh ) const {

   return ( ! ( *this == rh ) );

}

SInputData::SInputData()
   : m_type( "unknown" ), m_version( 0 ), m_totalLumiGiven( 0 ),
     m_totalLumiSum( 0 ), m_eventsTotal(0), m_neventsmax( -1 ), m_logger( "SInputData" ) {

   m_logger << VERBOSE << "In constructor" << SLogger::endmsg;
}

SInputData::~SInputData() {

   m_logger << VERBOSE << "In destructor" << SLogger::endmsg;

}

void SInputData::AddSFileIn( const SFile& sfile ) {

   m_sfileIn.push_back( sfile );
   m_totalLumiSum += sfile.lumi;
   return;
}

Double_t SInputData::GetTotalLumi() const { 
  
   Double_t return_lumi = 0.;
   // use the given luminosity for this InputData in case it is specified
   if (m_totalLumiGiven) return_lumi = m_totalLumiGiven;
   // otherwise use the sum of all files
   else return_lumi = m_totalLumiSum;
  
   // make sure that the lumi is not zero
   if( ! return_lumi ) 
      m_logger << FATAL << "total luminosity for "<< GetType() << "is ZERO!"
               << SLogger::endmsg;

   return return_lumi;
}

Double_t SInputData::GetScaledLumi() const { 
  
   Double_t scaled_lumi = 0.;

   if( m_neventsmax > -1. ) {
      scaled_lumi = GetTotalLumi() * m_neventsmax / m_eventsTotal;
   } else
      scaled_lumi=GetTotalLumi();

   return scaled_lumi;
}

SInputData& SInputData::operator= ( const SInputData& parent ) {

   this->m_type = parent.m_type;
   this->m_version = parent.m_version;
   this->m_totalLumiGiven = parent.m_totalLumiGiven;
   this->m_gencuts = parent.m_gencuts;
   this->m_sfileIn = parent.m_sfileIn;
   this->m_inputTrees = parent.m_inputTrees;
   this->m_evInputTrees = parent.m_evInputTrees;
   this->m_outputTrees = parent.m_outputTrees;
   this->m_totalLumiSum = parent.m_totalLumiSum;
   this->m_eventsTotal = parent.m_eventsTotal;
   this->m_neventsmax = parent.m_neventsmax;

   return *this;

}

Bool_t SInputData::operator== ( const SInputData& rh ) const {

   if( ( this->m_type == rh.m_type ) && ( this->m_version == rh.m_version ) &&
       ( this->m_totalLumiGiven == rh.m_totalLumiGiven ) &&
       ( this->m_gencuts == rh.m_gencuts ) && ( this->m_sfileIn == rh.m_sfileIn ) &&
       ( this->m_inputTrees == rh.m_inputTrees ) &&
       ( this->m_evInputTrees == rh.m_evInputTrees ) &&
       ( this->m_outputTrees == rh.m_outputTrees ) &&
       ( this->m_totalLumiSum == rh.m_totalLumiSum ) &&
       ( this->m_eventsTotal == rh.m_eventsTotal ) &&
       ( this->m_neventsmax == rh.m_neventsmax ) ) {
      return kTRUE;
   } else {
      return kFALSE;
   }

}

Bool_t SInputData::operator!= ( const SInputData& rh ) const {

   return ( ! ( *this == rh ) );

}

void SInputData::print() const {

   m_logger << INFO << " ---------------------------------------------------------" << endl;
   m_logger << " Type               : " << GetType() << endl;
   m_logger << " Version            : " << GetVersion() << endl;
   m_logger << " Total luminosity   : " << GetTotalLumi() << "pb-1" << endl;
   m_logger << " NEventsMax         : " << GetNEventsMax() << endl;

   for( vector< SGeneratorCut >::const_iterator gc = m_gencuts.begin();
        gc != m_gencuts.end(); ++gc )
      m_logger << " Generator cut      : Tree:" << gc->GetTreeName() << " Formula: "
               << gc->GetFormula() << endl;

   for( vector< SFile >::const_iterator f = m_sfileIn.begin(); f != m_sfileIn.end();
        ++f )
      m_logger << " Input SFiles       : " << "'" << f->file << "' (file) | '" << f->lumi
               << "' (lumi) " << endl;
   for( std::vector< STree >::const_iterator t = m_inputTrees.begin();
        t != m_inputTrees.end(); ++t )
      m_logger << " Input tree         : " << "'" << t->treeName << "'" << endl;
   for( std::vector< SEVTree >::const_iterator t = m_evInputTrees.begin();
        t != m_evInputTrees.end(); ++t )
      m_logger << " EV Input tree      : " << "'" << t->treeName << "' (tree) | '"
               << t->treeBaseName << "' (base name) | '" << t->collTreeName
               << "' (coll. tree name)" << endl;
   for( std::vector< STree >::const_iterator t = m_outputTrees.begin();
        t != m_outputTrees.end(); ++t )
      m_logger << " Output tree        : " << "'" << t->treeName << "'" << endl;

   m_logger << " ---------------------------------------------------------" << SLogger::endmsg;

   return;

}