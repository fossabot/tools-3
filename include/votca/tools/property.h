/*
 * Copyright 2009-2020 The VOTCA Development Team (http://www.votca.org)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#ifndef VOTCA_TOOLS_PROPERTY_H
#define VOTCA_TOOLS_PROPERTY_H

// Standard includes
#include <cstdlib>
#include <iostream>
#include <map>
#include <stdexcept>
#include <string>

// Third party includes
#include <boost/algorithm/string/trim.hpp>
#include <boost/format.hpp>

// Local VOTCA includes
#include "eigen.h"
#include "lexical_cast.h"
#include "tokenizer.h"
#include "types.h"

namespace votca {
namespace tools {

/**
 * \brief class to manage program options with xml serialization functionality
 *
 * This class stores tags and content in a hierarchical (tree) structure similar
 * to the one used in the XML format. The structure can be either filled
 * manually or read in from an XML file using load_property_from_xml. The
 * supported XML constructs are TAGS, ATTRIBUTES, and CONTENT: <tag
 * attribute_name="attribute_value"> content
 * </tag>
 * The property object can be output to an ostream using format modifiers:
 * cout << XML << property;
 * Supported formats are XML, TXT, TEX, HLP
 */
class Property {

  /// \brief outputs the property to the ostream
  friend std::ostream &operator<<(std::ostream &out, const Property &p);

 public:
  Property() = default;
  Property(const std::string &name, const std::string &value,
           const std::string &path)
      : _name(name), _value(value), _path(path) {}

  /**
   * \brief add a new property to structure
   * @param key identifier
   * @param value value
   * @return reference to the created Property object
   */
  Property &add(const std::string &key, const std::string &value);
  /**
   * \brief set value of existing property
   * @param key identifier
   * @param value value
   * @return reference to the created Property object
   */
  Property &set(const std::string &key, const std::string &value);

  /**
   * \brief get existing property
   * @param key identifier
   * @return Reference to property object
   *
   * This function tries to find a property specified by key separated
   * by "." to step down hierarchy. If the property is not
   * found a runtime_exception is thrown.
   */
  Property &get(const std::string &key);
  const Property &get(const std::string &key) const;

  /**
   * \brief adds new or gets existing property
   * @param key identifier
   * @return Reference to property object
   *
   * This function tries to find a property specified by key separated
   * by "." to step down hierarchy. If the property is not
   * found a property with that name is added and returned.
   */
  Property &getOradd(const std::string &key);

  /**
   * \brief check whether property exists
   * @param key identifier
   * @return true or false
   */
  bool exists(const std::string &key) const;

  /**
   * \brief select property based on a filter
   * @param filter
   * @return list of pointers to property objects
   *
   * returns a list of properties that match the key criteria including
   * wildcard "*". Example: "base.item*.value"
   */
  std::vector<Property *> Select(const std::string &filter);
  std::vector<const Property *> Select(const std::string &filter) const;

  /**
   * \brief reference to value of property
   * @return std::string content
   */
  std::string &value() { return _value; }
  const std::string &value() const { return _value; }
  /**
   * \brief name of property
   * @return name
   */
  std::string &name() { return _name; }
  const std::string &name() const { return _name; }
  /**
   * \brief full path of property (including parents)
   * @return path
   *
   * e.g. cg.inverse.value
   */
  std::string &path() { return _path; }
  const std::string &path() const { return _path; }
  /**
   * \brief return value as type
   *
   * returns the value after type conversion, e.g.
   * p.as<Index>() returns an integer
   */
  template <typename T>
  T as() const;

  /**
   * \brief does the property have children?
   * \return true or false
   */
  bool HasChildren() const { return !_map.empty(); }

  /// iterator to iterate over properties
  using iterator = std::vector<Property>::iterator;
  using const_iterator = std::vector<Property>::const_iterator;
  /// \brief iterator to first child property
  iterator begin() { return _properties.begin(); }
  const_iterator begin() const { return _properties.begin(); }
  /// \brief end iterator for child properties
  iterator end() { return _properties.end(); }
  const_iterator end() const { return _properties.end(); }
  /// \brief number of child properties
  Index size() const { return Index(_properties.size()); }

  /**
   * \brief return attribute as type
   *
   * returns an attribute after type conversion, e.g.
   * p.getAttribute<Index>() returns an integer
   */
  template <typename T>
  T getAttribute(const std::string &attribute) const;
  /**
   * \brief set an attribute
   */
  template <typename T>
  void setAttribute(const std::string &attribute, const T &value);
  /**
   * \brief return true if a node has attributes
   */
  bool hasAttributes() const { return _attributes.size() > 0; }
  /**
   * \brief return true if an attribute exists
   */
  bool hasAttribute(const std::string &attribute) const;
  /** for iterator-based access of Attributes */
  typedef std::map<std::string, std::string>::iterator AttributeIterator;
  typedef std::map<std::string, std::string>::const_iterator
      const_AttributeIterator;
  /**
   * \brief returns an iterator to an attribute
   */
  AttributeIterator findAttribute(const std::string &attribute) {
    return _attributes.find(attribute);
  }
  const_AttributeIterator findAttribute(const std::string &attribute) const {
    return _attributes.find(attribute);
  }
  /**
   * \brief returns an iterator to the first attribute
   */
  AttributeIterator firstAttribute() { return _attributes.begin(); }
  const_AttributeIterator firstAttribute() const { return _attributes.begin(); }
  /**
   * \brief returns an iterator to the last attribute
   */
  AttributeIterator lastAttribute() { return _attributes.end(); }
  const_AttributeIterator lastAttribute() const { return _attributes.end(); }
  /**
   * \brief return attribute as type
   *
   * returns an attribute after type conversion, e.g.
   * p.getAttribute<Index>() returns an integer
   */
  template <typename T>
  T getAttribute(AttributeIterator it);

  template <typename T>
  T getAttribute(const_AttributeIterator it) const;

  void LoadFromXML(const std::string &filename);

  static Index getIOindex() { return IOindex; };

 private:
  std::map<std::string, Index> _map;
  std::map<std::string, std::string> _attributes;
  std::vector<Property> _properties;

  std::string _name = "";
  std::string _value = "";
  std::string _path = "";

  static const Index IOindex;
};

// TO DO: write a better function for this!!!!
template <>
inline bool Property::as<bool>() const {
  if (_value == "true" || _value == "TRUE" || _value == "1") {
    return true;
  } else {
    return false;
  }
}

template <typename T>
inline T Property::as() const {
  return lexical_cast<T>(_value, "wrong type in " + _path + "." + _name + "\n");
}

template <>
inline std::string Property::as<std::string>() const {
  std::string tmp(_value);
  boost::trim(tmp);
  return tmp;
}

template <>
inline Eigen::VectorXd Property::as<Eigen::VectorXd>() const {
  std::vector<double> tmp;
  Tokenizer tok(as<std::string>(), " ,");
  tok.ConvertToVector<double>(tmp);
  return Eigen::Map<Eigen::VectorXd>(tmp.data(), tmp.size());
}

template <>
inline Eigen::Vector3d Property::as<Eigen::Vector3d>() const {
  std::vector<double> tmp;
  Tokenizer tok(as<std::string>(), " ,");
  tok.ConvertToVector<double>(tmp);
  Eigen::Vector3d result;
  if (Index(tmp.size()) != result.size()) {
    throw std::runtime_error("Vector has " +
                             boost::lexical_cast<std::string>(tmp.size()) +
                             " instead of three entries");
  }
  result << tmp[0], tmp[1], tmp[2];
  return result;
}

template <>
inline std::vector<Index> Property::as<std::vector<Index> >() const {
  std::vector<Index> tmp;
  Tokenizer tok(as<std::string>(), " ,\n\t");
  tok.ConvertToVector<Index>(tmp);
  return tmp;
}

template <>
inline std::vector<double> Property::as<std::vector<double> >() const {
  std::vector<double> tmp;
  Tokenizer tok(as<std::string>(), " ,\n\t");
  tok.ConvertToVector<double>(tmp);
  return tmp;
}

inline bool Property::hasAttribute(const std::string &attribute) const {
  std::map<std::string, std::string>::const_iterator it =
      _attributes.find(attribute);
  if (it == _attributes.end()) {
    return false;
  }
  return true;
}

template <typename T>
inline T Property::getAttribute(
    std::map<std::string, std::string>::const_iterator it) const {
  if (it != _attributes.end()) {
    return lexical_cast<T>((*it).second);
  } else {
    std::stringstream s;
    s << *this << std::endl;
    throw std::runtime_error(s.str() + "attribute " + (*it).first +
                             " not found\n");
  }
}

template <typename T>
inline T Property::getAttribute(const std::string &attribute) const {
  std::map<std::string, std::string>::const_iterator it =
      _attributes.find(attribute);

  if (it != _attributes.end()) {
    return lexical_cast<T>(_attributes.at(attribute),
                           "wrong type in attribute " + attribute +
                               " of element " + _path + "." + _name + "\n");
  } else {
    std::stringstream s;
    s << *this << std::endl;
    throw std::runtime_error(s.str() + "attribute " + attribute +
                             " not found\n");
  }
}
template <typename T>
inline void Property::setAttribute(const std::string &attribute,
                                   const T &value) {
  _attributes[attribute] =
      lexical_cast<std::string>(value, "wrong type to set attribute");
}

}  // namespace tools
}  // namespace votca

#endif  // VOTCA_TOOLS_PROPERTY_H
