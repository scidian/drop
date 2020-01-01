//
//      Created by Stephens Nunnally on 12/6/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#ifndef DR_CONTAINERS_H
#define DR_CONTAINERS_H

#include <deque>
#include <list>
#include <map>
#include <set>
#include <unordered_map>
#include <vector>

namespace Dr {


//####################################################################################
//##    Deque Functions
//############################
template<class T> bool DequeContains(const std::deque<T> &my_deque, T variable_to_check) {
    bool found = (std::find(my_deque.begin(), my_deque.end(), variable_to_check) != my_deque.end());
    return found;
}


//####################################################################################
//##    List Functions
//############################
template<class T> bool ListContains(const std::list<T> &my_list, T variable_to_check) {
    bool found = (std::find(my_list.begin(), my_list.end(), variable_to_check) != my_list.end());
    return found;
}


//####################################################################################
//##    Set Functions
//############################
template<class T> bool SetContains(const std::set<T> &my_set, T variable_to_check) {
    bool found = (std::find(my_set.begin(), my_set.end(), variable_to_check) != my_set.end());
    return found;
}


//####################################################################################
//##    Vector Functions
//############################
template<class T> bool VectorContains(const std::vector<T> &my_vector, T variable_to_check) {
    bool found = (std::find(my_vector.begin(), my_vector.end(), variable_to_check) != my_vector.end());
    return found;
}



}   // End namespace Dr

#endif // DR_CONTAINERS_H






