//--------------------------------------------------------
// file : InitTypeProperty.h
// project : GalilV2
// Class : InitTypeParser
//
// utility : this class parses the property string InitType
// and builds the InitTypeValue as requested by Galil 
// motion controller micro-code
//
// it's a human-to machine converter
// InitType string is of form : LSFWD:1;FI:-3;LSBWD:12;FH:-7
// 
// keep in mind : 
//   0 <= LSBWD | LSFWD <= 15
//  -7 <= FI    | FH    <= 7
//
// it's usualy not usefull to go to LSBWD and after to LSFWD 
// micro-code ignores it
//---------------------------------------------------------



#ifndef __init_type_property_h__
#define __init_type_property_h__

#include <iostream>
#include <sstream>
#include <string>


// chaines dans les proprietes
// du type : "LSFWD:1;FI:-3;"




// on passe a cette classe la chaine init_type dans le constructeur
// on lui demande un type de prise de ref : 
// if(InitTypeParser.ref_type(std::string(LIMIT_SWITCH_FORWARD))
// => c'est OK le type de prise de ref. est existant et valide
// sinon on choppe l'erreur => on la réécrit dans la property?

#define kNUM_COMPONENTS 4

namespace GalilAxis_ns
{

class InitTypeParser
{

public : 
	// ctor
	InitTypeParser (const std::string& prop, bool limit_switch_inverted);

	// dtor
	virtual ~InitTypeParser ();

	//- la vraie valeur codée comme le micro-code l'attend 
	unsigned long to_init_type_value ()
    throw (Tango::DevFailed); 

private : 
	//- retreives the value in the string
	void retreive_component_value (const std::string component_str, 
                                 short& component_val)
    throw (Tango::DevFailed); 

  std::string prop_;
	bool m_ls_inverted;

	short component_values_[kNUM_COMPONENTS];
};

} //- namespace GalilAxis_ns

#endif // __init_type_property_h__

