#ifndef UKN_CG_HELPER_H_
#define UKN_CG_HELPER_H_


#include "UKN/PreDeclare.h"

namespace ukn {

    /* creating effects some simple cg shaders shared by different renderers */
    /* Shader Provider must be Cg */

    UKN_API EffectPtr GetCgEffet2D();    
    UKN_API EffectPtr GetCgEffetPass();   


} // namespace ukn


#endif