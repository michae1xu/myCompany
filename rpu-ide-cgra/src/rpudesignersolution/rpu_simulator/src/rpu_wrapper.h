#ifndef __RPU_WRAPPER_H__
#define __RPU_WRAPPER_H__
#include "systemc.h"

//#define USE_DLL_MODE


#ifndef USE_DLL_MODE
#else
#include "rpu_global.h"
#endif

namespace rpu_esl_model
{
	class rpu;
//#ifndef USE_DLL_MODE
//	class  rpu_wrapper : public sc_module
//#else
//	class RPUSIMSHARED_EXPORT rpu_wrapper : public sc_module
//#endif
//    {
//    public:
//		rpu_wrapper(sc_module_name name);
//        SC_HAS_PROCESS(rpu_wrapper);
//
//        ~rpu_wrapper();
//
//    };
//
#ifndef USE_DLL_MODE
	//rpu_wrapper*  creat_rpu_chip(sc_module_name name);
#else
	RPUSIMSHARED_EXPORT int  creat_rpu_chip(sc_module_name name, string ide_xml_name,string mem_xml_name);
#endif

}



#endif
