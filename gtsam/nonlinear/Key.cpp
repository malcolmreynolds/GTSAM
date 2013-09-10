/* ----------------------------------------------------------------------------

 * GTSAM Copyright 2010, Georgia Tech Research Corporation,
 * Atlanta, Georgia 30332-0415
 * All Rights Reserved
 * Authors: Frank Dellaert, et al. (see THANKS for the full author list)

 * See LICENSE for the license information

 * -------------------------------------------------------------------------- */

/**
 * @file Key.h
 * @brief
 * @author Richard Roberts
 * @author Alex Cunningham
 * @date Feb 20, 2012
 */

#include <iostream>

#include <boost/lexical_cast.hpp>
#include <boost/foreach.hpp>

#include <gtsam/nonlinear/Key.h>
#include <gtsam/nonlinear/LabeledSymbol.h>

namespace gtsam {

/* ************************************************************************* */
std::string _defaultKeyFormatter(Key key) {
    const Symbol asSymbol(key);
    if(asSymbol.chr() > 0)
      return (std::string)asSymbol;
    else
      return boost::lexical_cast<std::string>(key);
  }

/* ************************************************************************* */
std::string _multirobotKeyFormatter(gtsam::Key key) {
  const LabeledSymbol asLabeledSymbol(key);
  if(asLabeledSymbol.chr() > 0 && asLabeledSymbol.label() > 0)
    return (std::string)asLabeledSymbol;

  const gtsam::Symbol asSymbol(key);
  if (asLabeledSymbol.chr() > 0)
    return (std::string)asSymbol;
  else
    return boost::lexical_cast<std::string>(key);
}

/* ************************************************************************* */
void printKeySet(const gtsam::KeySet& keys, const std::string& s, const KeyFormatter& keyFormatter) {
  std::cout << s << " ";
  if (keys.empty())
    std::cout << "(none)" << std::endl;
  else {
    BOOST_FOREACH(const gtsam::Key& key, keys)
      std::cout << keyFormatter(key) << " ";
    std::cout << std::endl;
  }
}
/* ************************************************************************* */

} // \namespace gtsam
