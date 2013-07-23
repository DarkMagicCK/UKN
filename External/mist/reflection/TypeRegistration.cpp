//
//  TypeRegistration.cpp
//  Mist
//
//  Created by Ruiwei Bu on 1/27/12.
//  Copyright (c) 2012 heizi. All rights reserved.
//

#include "mist/reflection/TypeDatabase.h"

#include "mist/MathUtil.h"
#include "mist/Preprocessor.h"

namespace mist {
    
    namespace reflection {
        /*
        static void OnMathTypeRegistration(TypeDB& db) {
            {
                FieldInfo fields[] = {
                    MIST_RF_TYPE_FIELD_2(Vector2, x),
                    MIST_RF_TYPE_FIELD_2(Vector2, y),
                };
                
                db.createType<Vector2>().defFields(fields, db);
            } 
            
            {
                
                FieldInfo fields[] = {
                    MIST_RF_TYPE_FIELD_2_F_D(Vector3, x, 0, "x component of the vector"),
                    MIST_RF_TYPE_FIELD_2_F_D(Vector3, y, 0, "y component of the vector"),
                    MIST_RF_TYPE_FIELD_2_F_D(Vector3, z, 0, "z component of the vector")
                };
                
                db.createType<Vector3>().defFields(fields, db);
            }
        }
        */
        static void OnTypeDBInitialize() {
          //  TypeDB& db = TypeDB::Default();
            
        //    OnMathTypeRegistration(db);
        }
        
        MIST_STATIC_RUN_CODE(OnTypeDBInitialize());
        
        
    } // namespace mist
    
}