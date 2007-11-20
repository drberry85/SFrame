// $Id: SCycleBaseConfig.cxx,v 1.1.1.1 2007-11-13 12:42:21 krasznaa Exp $
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

// ROOT include(s):
#include <TString.h>
#include <TXMLNode.h>
#include <TList.h>
#include <TXMLAttr.h>

// Local include(s):
#include "../include/SCycleBaseConfig.h"
#include "../include/SGeneratorCut.h"

ClassImp( SCycleBaseConfig );

using namespace std;

/**
 * Default constructor.
 */
SCycleBaseConfig::SCycleBaseConfig()
   : SCycleBaseBase() {

}

/**
 * Default destructor.
 */
SCycleBaseConfig::~SCycleBaseConfig() {

}

/**
 * Function reading the part of the configuration XML that describes this
 * cycle.
 */
void SCycleBaseConfig::Initialize( TXMLNode* node ) throw( SError ) {

   m_logger << INFO << "Initializing..." << SLogger::endmsg;

   // ------------- parse xml element -------------------------------------------------------

   while( node != 0 ) {
      if( ! node->HasChildren() ) {
         node = node->GetNextNode();
         continue;
      }

      if( node->GetNodeName() == TString( "InputData" ) ) {

         this->InitializeInputData( node );
   
      } else if( node->GetNodeName() == TString( "UserConfig" ) ) {

         this->InitializeUserConfig( node );

      }
      node = node->GetNextNode();
   }

   // now check if input data type appears multiple times
   this->CheckForMultipleInputData();

   // print input data classes
   for( vector< SInputData >::const_iterator iD = m_inputData.begin(); iD != m_inputData.end();
        ++iD ) {
      iD->print();
      m_logger << INFO << SLogger::endmsg;
   }

   // ------------- xml parsing terminated --------------------------------------------------
   return;

}

///////////////////////////////////////////////////////////////////////////
//                                                                       //
// Implementation of the functions declaring the properties of the       //
// derived class                                                         //
//                                                                       //
///////////////////////////////////////////////////////////////////////////

void SCycleBaseConfig::DeclareProperty( const std::string& name, std::string& value ) {

   m_stringPrefs[ name ] = &value;
   return;

}

void SCycleBaseConfig::DeclareProperty( const std::string& name, int& value ) {

   m_intPrefs[ name ] = &value;
   return;

}

void SCycleBaseConfig::DeclareProperty( const std::string& name, double& value ) {

   m_doublePrefs[ name ] = &value;
   return;

}

void SCycleBaseConfig::DeclareProperty( const std::string& name,
                                        std::vector< std::string >& value ) {

   m_stringListPrefs[ name ] = &value;
   return;

}

void SCycleBaseConfig::DeclareProperty( const std::string& name,
                                        std::vector< int >& value ) {

   m_intListPrefs[ name ] = &value;
   return;

}

void SCycleBaseConfig::DeclareProperty( const std::string& name,
                                        std::vector< double >& value ) {

   m_doubleListPrefs[ name ] = &value;
   return;

}

///////////////////////////////////////////////////////////////////////////
//                                                                       //
// Implementation of the functions called by the public Initialize(...)  //
// that handle the different aspects of the initialisation.              //
//                                                                       //
///////////////////////////////////////////////////////////////////////////

void SCycleBaseConfig::InitializeInputData( TXMLNode* node ) throw( SError ) {

   // create SInputData object
   SInputData inputData;

   TListIter attribIt( node->GetAttributes() );
   TXMLAttr* curAttr( 0 );
   while( ( curAttr = dynamic_cast< TXMLAttr* >( attribIt() ) ) != 0 ) {

      if( curAttr->GetName() == TString( "Type" ) ) {
         inputData.SetType( curAttr->GetValue() );
         m_logger << INFO << "Reading SInputData: " << inputData.GetType()
                  << SLogger::endmsg;
      }
      if( curAttr->GetName() == TString( "Version" ) )
         inputData.SetVersion( curAttr->GetValue() );
      if( curAttr->GetName() == TString( "Lumi" ) )
         inputData.SetTotalLumi( atof( curAttr->GetValue() ) );
      if( curAttr->GetName() == TString( "NEventsMax" ) ) 
         inputData.SetNEventsMax( atoi( curAttr->GetValue() ) );
   }

   TXMLNode* child = node->GetChildren();
   while( child != 0 ) {
      if( ! child->HasAttributes() ) {
         child = child->GetNextNode();
         continue;
      }

      TListIter attributes( child->GetAttributes() );
      TXMLAttr* attribute = 0;

      // get the generator cuts
      if( child->GetNodeName() == TString( "GeneratorCut" ) ) {
         TString treeName = "";
         TString formula = "";
         attribute = 0;

         while( ( attribute = dynamic_cast< TXMLAttr* >( attributes() ) ) != 0) {
            if( attribute->GetName() == TString( "Tree" ) ) treeName = attribute->GetValue();
            if( attribute->GetName() == TString( "Formula" ) ) formula = attribute->GetValue();
         }
         inputData.AddGenCut( SGeneratorCut( treeName, formula ) );

      }
      // get the input files
      else if( child->GetNodeName() == TString( "In" ) ) {
         std::string fileName = "";
         std::string treeName = "";
         Double_t lumi = 0.;

         attribute = 0;
         while( ( attribute = dynamic_cast< TXMLAttr* >( attributes() ) ) != 0) {
            if( attribute->GetName() == TString( "FileName" ) )
               fileName = attribute->GetValue();
            if( attribute->GetName() == TString( "Lumi" ) )
               lumi = atof( attribute->GetValue() );
         }
         inputData.AddSFileIn( SFile( fileName, lumi ) );

      }

      // get a "regular" input tree
      else if( child->GetNodeName() == TString( "InputTree" ) ) {
         std::string treeName = "";

         attribute = 0;
         while( ( attribute = dynamic_cast< TXMLAttr* >( attributes() ) ) != 0 ) {
            if( attribute->GetName() == TString( "Name" ) )
               treeName = attribute->GetValue();
         }
         inputData.AddInputSTree( STree( treeName ) );

      }
      // get an EventView input tree
      else if( child->GetNodeName() == TString( "EVInputTree" ) ) {
         std::string baseName = "";
         Int_t number = 0;
         std::string collTreeName = "";

         attribute = 0;
         while( ( attribute = dynamic_cast< TXMLAttr* >( attributes() ) ) != 0 ) {
            if( attribute->GetName() == TString( "BaseName" ) )
               baseName = attribute->GetValue();
            if( attribute->GetName() == TString( "Number" ) )
               number = atoi( attribute->GetValue() );
            if( attribute->GetName() == TString( "CollTreeName" ) )
               collTreeName = attribute->GetValue();
         }
         for( int i = 0; i < number; ++i ) {
            std::stringstream trName;
            trName << baseName << i;
            inputData.AddEVInputSTree( SEVTree( trName.str(), baseName,
                                                i, collTreeName ) );
         }

      }
      // get an output tree
      else if( child->GetNodeName() == TString( "OutputTree" ) ) {
         std::string treeName = "";

         attribute = 0;
         while( ( attribute = dynamic_cast< TXMLAttr* >( attributes() ) ) != 0 ) {
            if( attribute->GetName() == TString( "Name" ) )
               treeName = attribute->GetValue();
         }
         inputData.AddOutputSTree( STree( treeName ) );

      } else {
         // unknown field error
         m_logger << ERROR << " *** ERROR - unknown field " << child->GetNodeName()
                  << " ***" << SLogger::endmsg;
      }
      child = child->GetNextNode();
   }

   // Add this input data to the list:
   m_inputData.push_back( inputData );

   return;
}

void SCycleBaseConfig::InitializeUserConfig( TXMLNode* node ) throw( SError ) {

   TXMLNode* userNode = node->GetChildren();
   while( userNode != 0 ) {

      if( ! userNode->HasAttributes() || ( userNode->GetNodeName() != TString( "Item" ) ) ) {
         userNode = userNode->GetNextNode();
         continue;
      }

      std::string name = "", stringValue = "";
      TListIter userAttributes( userNode->GetAttributes() );
      TXMLAttr* attribute = 0;
      while( ( attribute = dynamic_cast< TXMLAttr* >( userAttributes() ) ) != 0 ) {
         if( attribute->GetName() == TString( "Name" ) ) name = attribute->GetValue();
         if( attribute->GetName() == TString( "Value" ) ) stringValue = attribute->GetValue();
      }
      m_logger << DEBUG << "Found user property with name \"" << name << "\" and value \""
               << stringValue << "\"" << SLogger::endmsg;

      // If it's a string property:
      if( m_stringPrefs.find( name ) != m_stringPrefs.end() ) {
         ( *m_stringPrefs[ name ] ) = stringValue;
      }
      // If it's an integer property:
      else if( m_intPrefs.find( name ) != m_intPrefs.end() ) {
         int value = atoi( stringValue.c_str() );
         ( *m_intPrefs[ name ] ) = value;
      }
      // If it's a double property:
      else if( m_doublePrefs.find( name ) != m_doublePrefs.end() ) {
         double value = atof( stringValue.c_str() );
         ( *m_doublePrefs[ name ] ) = value;
      }
      // If it's a string list property:
      else if( m_stringListPrefs.find( name ) != m_stringListPrefs.end() ) {
         m_stringListPrefs[ name ]->clear();
         istringstream stream( stringValue );
         while( ! stream.eof() ) {
            std::string value;
            stream >> value;
            m_stringListPrefs[ name ]->push_back( value );
         }
      }
      // If it's an integer list property:
      else if( m_intListPrefs.find( name ) != m_intListPrefs.end() ) {
         m_intListPrefs[ name ]->clear();
         istringstream stream( stringValue );
         while( ! stream.eof() ) {
            int value;
            stream >> value;
            m_intListPrefs[ name ]->push_back( value );
         }
      }
      // If it's a double list property:
      else if( m_doubleListPrefs.find( name ) != m_doubleListPrefs.end() ) {
         m_doubleListPrefs[ name ]->clear();
         istringstream stream( stringValue );
         while( ! stream.eof() ) {
            double value;
            stream >> value;
            m_doubleListPrefs[ name ]->push_back( value );
         }
      }
      // If it hasn't been requested by the analysis cycle, issue a warning.
      // It might mean a typo somewhere...
      else {
         m_logger << WARNING << "User property not found: " << name << endl
                  << "Value not set!" << SLogger::endmsg;
      }

      userNode = userNode->GetNextNode();
   }

   return;
}

/**
 * Function called at the beginning of the data processing of the cycle.
 * It loops through each input file, check the event numbers in them
 * (and that they can be read) so that the event weight calculation
 * would give proper weights in case of only running over a subset
 * of the events.
 */
void SCycleBaseConfig::CheckForMultipleInputData() throw ( SError ) {

   // multimap to hold all type strings of InputData objects; will be
   // used to search InputData objects with the same name, to make
   // sure these will be processed directly consecutively so that they
   // end up in the same output file
   multimap< string, int > inputDataHelper;
   vector< SInputData >::iterator be = m_inputData.begin();
   vector< SInputData >::iterator en = m_inputData.end();
   int index = 0;

   // loop over InputData vector and copy the type names into the
   // multimap, remembering the position in the vector
   for( ; be != en; ++be,++index ) {
      inputDataHelper.insert( make_pair( be->GetType(), index ) );
   }

   multimap< string, int >::iterator help1 = inputDataHelper.begin();
   multimap< string, int >::iterator help2 = inputDataHelper.end();
   index = 0;
   std::vector< SInputData > tmpInput;

   // Now copy the InputData objects to a temporary vector in the
   // order we want them to be processed
   for( ; help1 != help2; ++help1,++index ) {
      if( help1->second != index ) {
         m_logger << WARNING << "InputData of type \"" << help1->first
                  << "\" was given as input number " << ( help1->second + 1 )
                  << " but will be repositioned and instead processed as number "
                  << ( index + 1 ) << SLogger::endmsg;         
      }
      tmpInput.push_back( m_inputData[ help1->second ] );
   }

   // Sanity check
   if( m_inputData.size() != tmpInput.size() ) {
      SError error( SError::StopExecution );
      error << "Inconsistent InputData vectors: size " << m_inputData.size()
            << " and " << tmpInput.size();
      throw error;
   }

   // Now copy the objects back into the vector we use for processing
   for( size_t i = 0; i < tmpInput.size(); ++i ) {
      m_inputData[ i ] = tmpInput[ i ];
   }

   return;
}