//--------------------------------------------------------
// file : InitTypeProperty.cpp
//
// project : GalilV2
// sorce code for InitTypeParser class
//--------------------------------------------------------

#include <yat4tango/CommonHeader.h>
#include <yat4tango/ExceptionHelper.h>
#include <yat/utils/XString.h>
#include "InitTypeParser.h"

using namespace std;

namespace GalilAxis_ns
{


// ============================================================================
//
// ============================================================================
static enum
{
	FI = 0,
	FH,
	LSFWD,
	LSBWD
} ComponentName;

// ============================================================================
// 
// ============================================================================
static const std::string component_name[] =
{
	"FI",
	"FH",
	"LSFWD",
	"LSBWD"
};

// ============================================================================
// 
// ============================================================================
#define kKEYWORD_VALUE_SEP ":"
//-----------------------------------------------------------------------------
#define kNEXT_KEYWORD_SEP  ";"
//-----------------------------------------------------------------------------
#define kMIN_LSBWD 0
#define kMAX_LSBWD 15
//-----------------------------------------------------------------------------
#define kMIN_LSFWD kMIN_LSBWD
#define kMAX_LSFWD kMAX_LSBWD
//-----------------------------------------------------------------------------
#define kMIN_FI    -7
#define kMAX_FI     7
//-----------------------------------------------------------------------------
#define kMIN_FH    kMIN_FI
#define kMAX_FH    kMAX_FI
//-----------------------------------------------------------------------------

// ============================================================================
// InitTypeParser::InitTypeParser
// ============================================================================
InitTypeParser::InitTypeParser (const std::string& _prop, 
																bool limit_switch_inverted) 
 : prop_ (_prop),
   m_ls_inverted (limit_switch_inverted)
{
	::memset(this->component_values_, 0, (kNUM_COMPONENTS * sizeof(short)));
}

// ============================================================================
// InitTypeParser::~InitTypeParser
// ============================================================================
InitTypeParser::~InitTypeParser()
{

}  

// ============================================================================
// InitTypeParser::to_init_type_value
// ============================================================================
unsigned long InitTypeParser::to_init_type_value ()
    throw (Tango::DevFailed)
{
  //- check property 
	if (prop_.size() == 0)
	{
		THROW_DEVFAILED(_CPTC("INVALID_ARGUMENT"),
                    _CPTC("invalid InitType property [check its value in the Tango database]"),
                    _CPTC("InitTypeParser::to_init_type_value"));
	}

  //- parse the property string 
	for (size_t i = 0; i < kNUM_COMPONENTS; i++)
	{
		//- we look for identifier in componement_name, store value in componement_values
		this->retreive_component_value (component_name[i], this->component_values_[i]);
	}

  //- check LSBWD value
  if (component_values_[LSBWD] < kMIN_LSBWD || component_values_[LSBWD] > kMAX_LSBWD)
  {
    yat::OSStream err_desc;
    err_desc << "InitType component <" 
             << component_name[LSBWD]
             << "> has an invalid associated value - valid range is ["
             << kMIN_LSBWD
             << ","
             << kMAX_LSBWD
             << "]"
             << "- check InitType property";
    THROW_DEVFAILED(_CPTC("INVALID_PROPERTY"),
                    _CPTC(err_desc.str().c_str()),
                    _CPTC("InitTypeParser::to_init_type_value"));
  }

  //- check LSFWD value
  if (component_values_[LSFWD] < kMIN_LSFWD || component_values_[LSFWD] > kMAX_LSFWD)
  {
    yat::OSStream err_desc;
    err_desc << "InitType component <" 
             << component_name[LSFWD]
             << "> has an invalid associated value - valid range is ["
             << kMIN_LSFWD
             << ","
             << kMAX_LSFWD
             << "]"
             << "- check InitType property";
    THROW_DEVFAILED(_CPTC("INVALID_PROPERTY"),
                    _CPTC(err_desc.str().c_str()),
                    _CPTC("InitTypeParser::to_init_type_value"));
  }

  //- check FI value
  if (component_values_[FI] < kMIN_FI || component_values_[FI] > kMAX_FI)
  {
    yat::OSStream err_desc;
    err_desc << "InitType component <" 
             << component_name[FI]
             << "> has an invalid associated value - valid range is ["
             << kMIN_FI
             << ","
             << kMAX_FI
             << "]"
             << "- check InitType property";
    THROW_DEVFAILED(_CPTC("INVALID_PROPERTY"),
                    _CPTC(err_desc.str().c_str()),
                    _CPTC("InitTypeParser::to_init_type_value"));
  }

  //- check FH value
  if (component_values_[FH] < kMIN_FH || component_values_[FH] > kMAX_FH)
  {
    yat::OSStream err_desc;
    err_desc << "InitType component <" 
             << component_name[FH]
             << "> has an invalid associated value - valid range is ["
             << kMIN_FH
             << ","
             << kMAX_FH
             << "]"
             << "- check InitType property";
    THROW_DEVFAILED(_CPTC("INVALID_PROPERTY"),
                    _CPTC(err_desc.str().c_str()),
                    _CPTC("InitTypeParser::to_init_type_value"));
  }

  //- be sure only one limit switch is used 
  if (component_values_[LSBWD] && component_values_[LSFWD])
  {
    yat::OSStream err_desc;
    err_desc << "inconsistent InitType "
             << "- both LSBWD and LSFWD appear in the property " 
             << "- only one of them should be used";
    THROW_DEVFAILED(_CPTC("INVALID_PROPERTY"),
                    _CPTC(err_desc.str().c_str()),
                    _CPTC("InitTypeParser::to_init_type_value"));
  }

	//- invert limit switches values if necessary :
	//- if AxisPositionRatio is < 0 limit switch must be inverted
	if (m_ls_inverted)
	{
		//- invert limit switches values 
		short tmp = component_values_[LSFWD];
		component_values_[LSFWD] = component_values_[LSBWD];
		component_values_[LSBWD] = tmp;
		//- and change the signs of FI and FH
		component_values_[FH] *= static_cast<short>(-1);
		component_values_[FI] *= static_cast<short>(-1);
	}

  //- build InitType value from individual <component_values_> 
  unsigned long tmp;
  unsigned long init_type = 0;
  unsigned long neg_mask = 0x00000008;

  for (size_t j = FI; j < kNUM_COMPONENTS; j++)
	{
    tmp = ::abs(this->component_values_[j]);

    if (this->component_values_[j] < 0)
      tmp |= neg_mask;
    
		init_type |= tmp << (j * 4);
	}

  return init_type;
}
                           
// ============================================================================
// InitTypeParser::retreive_component_value
// ============================================================================
void InitTypeParser::retreive_component_value (const std::string to_find, short &val)
    throw (Tango::DevFailed)
{
	std::string::size_type ind = prop_.find(to_find);

  //- component not found in property - just set its associated value to 0
	if (ind == std::string::npos)
  {
    val = 0;
    return;
  }

  YAT_LOG("found " << to_find << " in " << prop_ );

  //- find KEYWORD/VALUE separator
  std::string::size_type kw_val_sep_pos = prop_.find_first_of(kKEYWORD_VALUE_SEP, ind);
  if (kw_val_sep_pos == std::string::npos)
  {
    yat::OSStream err_desc;
    err_desc << "syntax error in InitType property near <" 
             << to_find
             << "> component [syntax example: LSBWD:1;FI:2;FH:-3]";
    THROW_DEVFAILED(_CPTC("INVALID_PROP�RTY"),
                    _CPTC(err_desc.str().c_str()),
                    _CPTC("InitTypeParser::retreive_component_value"));
  }

  //- find next KEYWORD separator (may be end of string)
  std::string::size_type next_kw_sep_pos = prop_.find_first_of(kNEXT_KEYWORD_SEP, ind);
  if (next_kw_sep_pos == std::string::npos)
  {
    next_kw_sep_pos = prop_.size(); 
  }

  try
  {
    std::string::size_type sub_str_begin = kw_val_sep_pos + 1;
    std::string::size_type sub_str_len = next_kw_sep_pos - kw_val_sep_pos - 1;

    YAT_LOG(prop_.substr(sub_str_begin, sub_str_len));

    val = yat::XString<short>::to_num(prop_.substr(sub_str_begin, sub_str_len));
  }
  catch (Tango::DevFailed&)
  {
    yat::OSStream err_desc;
    err_desc << "InitType component <" 
             << to_find
             << "> has no or invalid associated value [check InitType property]";
    THROW_DEVFAILED(_CPTC("INVALID_PROPERTY"),
                    _CPTC(err_desc.str().c_str()),
                    _CPTC("InitTypeParser::retreive_component_value"));
  }
}

} //- namespace GalilAxis_ns




