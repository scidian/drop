//
//      Created by Stephens Nunnally on 12/8/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "library/dr_debug.h"
#include "library/types/dr_variant.h"


//####################################################################################
//##    Overload Operators
//####################################################################################
DrVariant& DrVariant::operator=(const DrVariant &other) {
    m_value = other.value();
    return *this;
}


//####################################################################################
//##    Primitive Data Type Returns
//####################################################################################
bool DrVariant::toBool() {
    try { return boost::any_cast<bool>(m_value); }
    catch (const boost::bad_any_cast &) {
        try { return static_cast<bool>(boost::any_cast<int>(m_value)); }
        catch (const boost::bad_any_cast &) {
            try { return static_cast<bool>(boost::any_cast<float>(m_value)); }
            catch (const boost::bad_any_cast &) {
                try { return static_cast<bool>(boost::any_cast<double>(m_value)); }
                catch (const boost::bad_any_cast &) {
                    try { return static_cast<bool>(boost::any_cast<float>(m_value)); }
                    catch (const boost::bad_any_cast &) {
                        return false; }
                }
            }
        }
    }
}

double DrVariant::toDouble() {
    try { return boost::any_cast<double>(m_value); }
    catch (const boost::bad_any_cast &) {
        try { return static_cast<double>(boost::any_cast<float>(m_value)); }
        catch (const boost::bad_any_cast &) {
            try { return static_cast<double>(boost::any_cast<int>(m_value)); }
            catch (const boost::bad_any_cast &) {
                try { return static_cast<double>(boost::any_cast<long>(m_value)); }
                catch (const boost::bad_any_cast &) {
                    try { return static_cast<double>(boost::any_cast<unsigned int>(m_value)); }
                    catch (const boost::bad_any_cast &) {
                        try { return static_cast<double>(boost::any_cast<unsigned long>(m_value)); }
                        catch (const boost::bad_any_cast &) {
                            try { return static_cast<double>(boost::any_cast<size_t>(m_value)); }
                            catch (const boost::bad_any_cast &) {
                                return 0.0;
                            }
                        }
                    }
                }
            }
        }
    }
}

float DrVariant::toFloat() {
    try { return boost::any_cast<float>(m_value); }
    catch (const boost::bad_any_cast &) {
        try { return static_cast<float>(boost::any_cast<double>(m_value)); }
        catch (const boost::bad_any_cast &) {
            try { return static_cast<float>(boost::any_cast<int>(m_value)); }
            catch (const boost::bad_any_cast &) {
                try { return static_cast<float>(boost::any_cast<long>(m_value)); }
                catch (const boost::bad_any_cast &) {
                    try { return static_cast<float>(boost::any_cast<unsigned int>(m_value)); }
                    catch (const boost::bad_any_cast &) {
                        try { return static_cast<float>(boost::any_cast<unsigned long>(m_value)); }
                        catch (const boost::bad_any_cast &) {
                            try { return static_cast<float>(boost::any_cast<size_t>(m_value)); }
                            catch (const boost::bad_any_cast &) {
                                return 0.0f;
                            }
                        }
                    }
                }
            }
        }
    }
}

int DrVariant::toInt() {
    try { return boost::any_cast<int>(m_value); }
    catch (const boost::bad_any_cast &) {
        try { return static_cast<int>(boost::any_cast<long>(m_value)); }
        catch (const boost::bad_any_cast &) {
            try { return static_cast<int>(boost::any_cast<unsigned int>(m_value)); }
            catch (const boost::bad_any_cast &) {
                try { return static_cast<int>(boost::any_cast<unsigned long>(m_value)); }
                catch (const boost::bad_any_cast &) {
                    try { return static_cast<int>(boost::any_cast<double>(m_value)); }
                    catch (const boost::bad_any_cast &) {
                        try { return static_cast<int>(boost::any_cast<float>(m_value)); }
                        catch (const boost::bad_any_cast &) {
                            try { return static_cast<int>(boost::any_cast<size_t>(m_value)); }
                            catch (const boost::bad_any_cast &) {
                                return 0;
                            }
                        }
                    }
                }
            }
        }
    }
}

long DrVariant::toLong() {
    try { return boost::any_cast<long>(m_value); }
    catch (const boost::bad_any_cast &) {
        try { return static_cast<long>(boost::any_cast<int>(m_value)); }
        catch (const boost::bad_any_cast &) {
            try { return static_cast<long>(boost::any_cast<unsigned long>(m_value)); }
            catch (const boost::bad_any_cast &) {
                try { return static_cast<long>(boost::any_cast<unsigned int>(m_value)); }
                catch (const boost::bad_any_cast &) {
                    try { return static_cast<long>(boost::any_cast<double>(m_value)); }
                    catch (const boost::bad_any_cast &) {
                        try { return static_cast<long>(boost::any_cast<float>(m_value)); }
                        catch (const boost::bad_any_cast &) {
                            try { return static_cast<long>(boost::any_cast<size_t>(m_value)); }
                            catch (const boost::bad_any_cast &) {
                                return 0;
                            }
                        }
                    }
                }
            }
        }
    }
}

unsigned int DrVariant::toUInt() {
    try { return boost::any_cast<unsigned int>(m_value); }
    catch (const boost::bad_any_cast &) {
        try { return static_cast<unsigned int>(boost::any_cast<unsigned long>(m_value)); }
        catch (const boost::bad_any_cast &) {
            try { return static_cast<unsigned int>(boost::any_cast<int>(m_value)); }
            catch (const boost::bad_any_cast &) {
                try { return static_cast<unsigned int>(boost::any_cast<long>(m_value)); }
                catch (const boost::bad_any_cast &) {
                    try { return static_cast<unsigned int>(boost::any_cast<size_t>(m_value)); }
                    catch (const boost::bad_any_cast &) {
                        return 0;
                    }
                }
            }
        }
    }
}

//####################################################################################
//##    Std Data Type Returns
//####################################################################################
std::string DrVariant::toString() {

    // #NOTE: Alternate type casting check, however, wary of consistent type names across platforms
    ///if (m_value.type() == typeid(std::string)) {

    try { return boost::any_cast<std::string>(m_value); }
    catch (const boost::bad_any_cast &) {
        try { return std::string(boost::any_cast<const char*>(m_value)); }
        catch (const boost::bad_any_cast &) {
            return std::string("");
        }
    }
}

//####################################################################################
//##    Container Data Type Returns
//####################################################################################
// Try to return from Deque, or from Vector, otherwise return empty Vector
std::deque<DrVariant> DrVariant::toDeque() {
    try { return boost::any_cast<std::deque<DrVariant>>(m_value); }
    catch (const boost::bad_any_cast &) {
        try { std::vector<DrVariant> as_vector = boost::any_cast<std::vector<DrVariant>>(m_value);
              return { as_vector.begin(), as_vector.end() }; }
        catch (const boost::bad_any_cast &) { return std::deque<DrVariant>(); } }
}
std::list<DrVariant> DrVariant::toList() {
    try { return boost::any_cast<std::list<DrVariant>>(m_value); }
    catch (const boost::bad_any_cast &) {   return std::list<DrVariant>(); }
}
// Try to return from Vector, or from Deque, otherwise return empty Vector
std::vector<DrVariant> DrVariant::toVector() {
    try { return boost::any_cast<std::vector<DrVariant>>(m_value); }
    catch (const boost::bad_any_cast &) {
        try { std::deque<DrVariant> as_deque = boost::any_cast<std::deque<DrVariant>>(m_value);
              return { as_deque.begin(), as_deque.end() }; }
        catch (const boost::bad_any_cast &) { return std::vector<DrVariant>(); }
    }
}

//####################################################################################
//##    Custom Data Type Returns
//####################################################################################
// Try to return from DrPoint, otherwise DrPointF, otherwise return Point(0,0)
DrPoint DrVariant::toPoint() {
    try { return boost::any_cast<DrPoint>(m_value); }
    catch (const boost::bad_any_cast &) {
        try { DrPointF pointf = boost::any_cast<DrPointF>(m_value);
              return DrPoint(pointf.x, pointf.y); }
        catch (const boost::bad_any_cast &) {
            return DrPoint();
        }
    }
}
// Try to return from DrPointF, otherwise DrPoint, otherwise return Point(0,0)
DrPointF DrVariant::toPointF() {
    try { return boost::any_cast<DrPointF>(m_value); }
    catch (const boost::bad_any_cast &) {
        try { DrPoint point = boost::any_cast<DrPoint>(m_value);
              return DrPointF(point.x, point.y); }
        catch (const boost::bad_any_cast &) {
            return DrPointF();
        }
    }
}




















