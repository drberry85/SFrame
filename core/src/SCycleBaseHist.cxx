// $Id: SCycleBaseHist.cxx,v 1.4.2.1 2008-12-01 14:52:56 krasznaa Exp $
/***************************************************************************
 * @Project: SFrame - ROOT-based analysis framework for ATLAS
 * @Package: Core
 *
 * @author Stefan Ask       <Stefan.Ask@cern.ch>           - Manchester
 * @author David Berge      <David.Berge@cern.ch>          - CERN
 * @author Johannes Haller  <Johannes.Haller@cern.ch>      - Hamburg
 * @author A. Krasznahorkay <Attila.Krasznahorkay@cern.ch> - CERN/Debrecen
 *
 ***************************************************************************/

// STL include(s):
#include <vector>
#include <string>

// ROOT include(s):
#include <TDirectory.h>
#include <TH1.h>
#include <TList.h>

// Local inlcude(s):
#include "../include/SCycleBaseHist.h"
#include "../include/SCycleOutput.h"

/*
#ifndef DOXYGEN_IGNORE
ClassImp( SCycleBaseHist );
#endif // DOXYGEN_IGNORE
*/

using namespace std;

/**
 * The constructor initialises the base class and the member variables.
 */
SCycleBaseHist::SCycleBaseHist()
   : SCycleBaseBase(), m_histoMap(), m_output( 0 ) {

   m_logger << VERBOSE << "SCycleBaseHist constructed" << SLogger::endmsg;

}

/**
 * Another one of the "I don't do anything" destructors.
 */
SCycleBaseHist::~SCycleBaseHist() {

   m_logger << VERBOSE << "SCycleBaseHist destructed" << SLogger::endmsg;

}

void SCycleBaseHist::SetHistOutput( TList* output ) {

   m_output = output;
   return;

}

TList* SCycleBaseHist::GetHistOutput() const {

   return m_output;

}

/**
 * Function for writing any kind of object inheriting from TObject into
 * the output file. It is meant to be used with objects that are
 * created once, then they don't have to be touched again. (Like
 * TGraph and friends.)
 *
 * To write out a TGraph for instance, you could write something like:
 *
 * <code>
 *   TGraph mygraph( n, x_array, y_array );<br/>
 *   mygraph.SetName( "MyGraph" );<br/>
 *   WriteObj( mygraph );
 * </code>
 *
 * @param obj       Constant reference to the object to be written out
 * @param directory Optional directory name in which to save the object
 */
void SCycleBaseHist::WriteObj( const TObject& obj,
                               const char* directory ) throw( SError ) {

   GetTempDir()->cd();

   TString path = ( directory ? directory : "" ) + TString( "/" ) +
      TString( obj.GetName() );
   SCycleOutput* out = dynamic_cast< SCycleOutput* >( m_output->FindObject( path ) );
   if( ! out ) {
      out = new SCycleOutput( obj.Clone(), path, directory );
      m_output->Add( out );
   }

   gROOT->cd(); // So that the temporary objects would be created
                // in a general memory space.

   return;
}

/**
 * This function is very similar to SCycleBaseHist::Retrieve. It looks for
 * a <strong>1-dimensional histogram</strong> in the output file with a
 * given name in a given directory and returns a pointer to it.
 *
 * The important difference wrt. SCycleBaseHist::Retrieve is that it only
 * uses the slow ROOT methods for finding the histogram once for each
 * new output file. It uses a caching mechanism for all histograms that
 * were already searched for, making the n-th search much faster than
 * that performed by SCycleBaseHist::Retrieve. It's still slower than
 * using separate pointers, but not by much.
 *
 * It should be especially useful when handling a lot of histograms.
 * Having a pointer for each of these histograms can be a pain above
 * a certain number. Instead you can book a histogram once in
 * SCycleBase::BeginInputData and then you can access it with:
 *
 * <code>
 *  In BeginInputData:
 *    Book( TH1D( "hist", "Histogram", 100, 0.0, 100.0 ) );
 *
 *  In ExecuteEvent:
 *    Hist( "hist" )->Fill( 50.0 );
 * </code>
 *
 * @param name The name of the histogram
 * @param dir  The name of the directory the histogram is in
 */
TH1* SCycleBaseHist::Hist( const char* name, const char* dir ) {

   TH1* result;

   pair< const char*, const char* > this_pair( name, dir );
   map< pair< const char*, const char* >, TH1* >::const_iterator it;
   if( ( it = m_histoMap.find( this_pair ) ) != m_histoMap.end() ) {
      result = it->second;
   } else {
      m_logger << VERBOSE << "Hist(): Using Retrieve for name \""
               << name << "\" and dir \"" << ( dir ? dir : "" ) << "\""
               << SLogger::endmsg;
      result = m_histoMap[ this_pair ] = Retrieve< TH1 >( name, dir );
   }

   return result;

}

TDirectory* SCycleBaseHist::GetTempDir() const {

   static TDirectory* tempdir = 0;

   if( ! tempdir ) {
      gROOT->cd();
      tempdir = gROOT->mkdir( "SFrameTempDir" );
      if( ! tempdir ) {
         m_logger << ERROR << "Temporary directory could not be created"
                  << SLogger::endmsg;
      }
   }

   return tempdir;

}
