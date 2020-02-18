//
//      Created by Stephens Nunnally on 3/17/2020, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      Progress Bar Interface
//
//
#ifndef DR_PROGRESS_H
#define DR_PROGRESS_H

#include <string>


//####################################################################################
//##    InterfaceProgressBar
//##        Used to interface slow functions to progress bars
//############################
class IProgressBar
{
private:
    int             m_item_count            { 1 };                      // Number of items to be processed during this progress bar
    int             m_current_item          { 0 };                      // Item currently being processed

    double          m_show_if_longer_than   { 1.25 };                   // Don't show if calculated time is less than this many seconds

public:
    // Constructor / Destructor
    IProgressBar(int item_count);
    virtual ~IProgressBar();


    // Virtual Progress Bar Features
    virtual void    setDisplayText(std::string text) = 0;               // Updates text on progress bar
    virtual void    stopProgress() = 0;                                 // Forces progress bar closed
    virtual bool    updateValue(int i) = 0;                             // Sends updated value to progress bar


    // Getters / Setters
    int             getCurrentItem()    { return m_current_item; }
    int             getItemCount()      { return m_item_count; }
    void            moveToNextItem()    { m_current_item++; }

    double          getShowIfWaitIsLongetThan() { return m_show_if_longer_than; }
    void            setShowIfWaitIsLongerThan(double seconds) { m_show_if_longer_than = seconds; }

};


#endif // DR_PROGRESS_H












