$input v_normal, v_uv0

/*
 * Copyright 2011-2026 Branimir Karadzic. All rights reserved.
 * License: https://github.com/bkaradzic/bgfx/blob/master/LICENSE
 */

#include "common/common.sh"

void main()
{
	gl_FragColor = vec4(v_normal, 1.0);
}