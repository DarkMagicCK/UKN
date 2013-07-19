//
//  Shader.fsh
//  UKN
//
//  Created by Robert Bu on 7/19/13.
//  Copyright (c) 2013 Robert Bu. All rights reserved.
//

varying lowp vec4 colorVarying;

void main()
{
    gl_FragColor = colorVarying;
}
