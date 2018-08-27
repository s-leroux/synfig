/* === S Y N F I G ========================================================= */
/*!	\file range.h
**	\brief Various discreet type definitions
**
**	$Id$
**
**	\legal
**	Copyright (c) 2017 Sylvain Leroux
**
**	This package is free software; you can redistribute it and/or
**	modify it under the terms of the GNU General Public License as
**	published by the Free Software Foundation; either version 2 of
**	the License, or (at your option) any later version.
**
**	This package is distributed in the hope that it will be useful,
**	but WITHOUT ANY WARRANTY; without even the implied warranty of
**	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
**	General Public License for more details.
**	\endlegal
*/
/* ========================================================================= */

/* === S T A R T =========================================================== */

#ifndef __SYNFIG_RANGE_H
#define __SYNFIG_RANGE_H

/* === H E A D E R S ======================================================= */

#include "real.h"
#include <cmath>

/* === M A C R O S ========================================================= */

// For some reason isnan() isn't working on macosx any more.
// This is a quick fix.
#if defined(__APPLE__) && !defined(SYNFIG_ISNAN_FIX)
#ifdef isnan
#undef isnan
#endif
inline bool isnan(double x) { return x != x; }
inline bool isnan(float x) { return x != x; }
#define SYNFIG_ISNAN_FIX 1
#ifdef isinf
#undef isinf
#endif
inline bool isinf(double x) { return !isnan(x) && isnan(x - x); }
inline bool isinf(float x) { return !isnan(x) && isnan(x - x); }
#define SYNFIG_ISINF_FIX 1
#else
#ifndef isnan
#define isnan(x) (std::isnan)(x)
#endif
#ifndef isinf
#define isinf(x) (std::isinf)(x)
#endif
#endif


/* === T Y P E D E F S ===================================================== */

/* === C L A S S E S & S T R U C T S ======================================= */

namespace synfig {

/*!	\class Range
**	
** A semi-open range of values
*/
class Range
{
public:
	typedef Real value_type;

private:
	value_type _from, _to;

public:
	Range(): _from(0.0), _to(0.0) { };
	Range(const value_type &from, const value_type &to):_from(from),_to(to) { };

	bool is_valid()const { return !(isnan(_from) || isnan(_to)); }
	bool is_nan_or_inf()const { return isnan(_from) || isnan(_to) || isinf(_from) || isinf(_to); }


  value_type  from() const { return _from; }
  value_type  to() const { return _to; }
  value_type  width() const { return _to-_from; }
  bool        empty() const { return _from >= _to; }
  operator bool() const { return !empty(); }
  bool        contains(const value_type& value) const { return (value >= _from) && (value < _to); }

  static value_type map(const value_type& value,
                        const value_type& xmin, const value_type& xmax,
                        const value_type& ymin, const value_type& ymax)
  {
    return ymin+(value-xmin)*(ymax-ymin)/(xmax-xmin);
  }

  value_type map(const value_type& value, const value_type& f2, const value_type& t2) const
  {
    return Range::map(value, _from, _to, f2, t2);
  }

  Range map(const Range& value, const value_type& f2, const value_type& t2) const
  {
    return Range(map(value._from, f2, t2), map(value._to, f2, t2));
  }
  
  value_type map(const value_type& value, const Range& dest) const
  {
      return map(value, dest._from, dest._to);
  }
  
  Range map(const Range& value, const Range& dest) const
  {
      return Range(map(value._from, dest), map(value._to, dest));
  }

  value_type unmap(const value_type& value, const value_type& f2, const value_type& t2) const
  {
    return Range::map(value, f2, t2, _from, _to);
  }

  Range unmap(const Range& value, const value_type& f2, const value_type& t2) const
  {
      return Range(unmap(value._from, f2, t2), unmap(value._to, f2, t2));
  }
  
  value_type unmap(const value_type& value, const Range& dest) const
  {
      return unmap(value, dest._from, dest._to);
  }
  
  Range unmap(const Range& value, const Range& dest) const
  {
      return Range(unmap(value._from, dest), unmap(value._to, dest));
  }


  static value_type clamp(const value_type& value,
                          const value_type& a, const value_type& b)
  {
      return (value < a) ? a :
             (value > b) ? b :
             value;
  }
  
  value_type clamp(const value_type& value) const
  {
    return Range::clamp(value, _from, _to);
  }
  
  Range clamp(const Range& value) const
  {
    return Range(clamp(value._from), clamp(value._to));
  }
};


}; // END of namespace synfig


/* === E N D =============================================================== */

#endif
