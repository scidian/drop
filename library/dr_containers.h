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
//##    List Functions
//############################
template<class T> bool ListContains(std::list<T> &my_list, T variable_to_check) {
    bool found = (std::find(my_list.begin(), my_list.end(), variable_to_check) != my_list.end());
    return found;
}




}   // end namespace Dr

#endif // DR_CONTAINERS_H






