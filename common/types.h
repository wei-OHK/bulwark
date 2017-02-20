// Copyright (C) 2015-2016 Wei@OHK, Hiroshima University.
// This file is part of the "bulwark framework".
// For conditions of distribution and use, see copyright notice in bulwark.h

#ifndef _BUL_COMMON_TYPES_H
#define _BUL_COMMON_TYPES_H

/// Creates simple tags
#define MAKE_TAG(c0, c1, c2, c3) \
		((unsigned int)(unsigned char)(c0) | ((unsigned int)(unsigned char)(c1) << 8) | \
			((unsigned int)(unsigned char)(c2) << 16) | ((unsigned int)(unsigned char)(c3) << 24 ))

#endif /* _BUL_COMMON_TYPES_H */
