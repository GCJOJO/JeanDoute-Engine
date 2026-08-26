$input a_position, a_normal, a_uv0
$output v_normal, v_uv0

/*
 * Copyright 2011-2026 Branimir Karadzic. All rights reserved.
 * License: https://github.com/bkaradzic/bgfx/blob/master/LICENSE
 */

#include "common/common.sh"

void main()
{
    gl_Position = mul(u_modelViewProj, vec4(a_position, 1.0) );
    v_normal = a_normal;
    v_uv0 = a_uv0;
}