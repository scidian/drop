//
//      Created by Stephens Nunnally on 3/17/2020, (c) 2019 Scidian Studios, All Rights Reserved
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
    // Progress Bar Info
    int             m_item_count            { 1 };                  // Number of items to be processed during this progress bar
    int             m_current_item          { 0 };                  // Item currently being processed
    bool            m_canceled              { false };              // Becomes true if user cancels event

    // Sub Function Variables
    double          m_show_if_longer_than   { 1.50 };               // Don't show if calculated time is less than this many seconds
    bool            m_progress_locked       { false };
    double          m_add_in                { 0.0 };                // Used in sub functions as value add in
    double          m_multiplier            { 1.0 };                // Used in sub functions as value multiplier

    // Display Variables
    std::string     m_prefix                { "" };
    std::string     m_suffix                { "" };

public:
    // Constructor / Destructor
    IProgressBar(int item_count);
    virtual ~IProgressBar();


    // Virtual Progress Bar Features
    virtual void    setDisplayText(std::string text) = 0;                   // Updates text on progress bar
    virtual void    stopProgress() = 0;                                     // Forces progress bar closed
    virtual void    updateValue(double d) = 0;                              // Sends updated value to progress bar (pass in 0 to 100)


    // Progress Locking
    bool            isProgressLocked()  { return m_progress_locked; }       // These functions are to be used before passing to a sub function, keeps animation
    void            lockProgress()      { m_progress_locked = true; }       //      going while sub function is running, but not increase progress bar since the
    void            unlockProgress()    { m_progress_locked = false; }      //      sub routine was not included in original item count


    // Sub Function Values
    void            setAddIn(double add)        { m_add_in = add; }
    double          getAddIn()                  { return m_add_in; }
    void            setMultiplier(double mul)   { m_multiplier = mul; }
    double          getMultiplier()             { return m_multiplier; }


    // Display Text Prefix / Suffix
    std::string     getPrefix() { return m_prefix; }
    std::string     getSuffix() { return m_suffix; }
    void            setPrefix(std::string prefix) { m_prefix = prefix; }
    void            setSuffix(std::string suffix) { m_suffix = suffix; }


    // Getters / Setters
    bool            isCanceled()        { return m_canceled; }
    void            setCanceled()       { m_canceled = true; }

    int             getCurrentItem()    { return m_current_item; }
    int             getItemCount()      { return m_item_count; }
    void            moveToNextItem()    { if (m_progress_locked == false) m_current_item++; }

    double          getShowIfWaitIsLongetThan() { return m_show_if_longer_than; }
    void            setShowIfWaitIsLongerThan(double seconds) { m_show_if_longer_than = seconds; }

};


#endif // DR_PROGRESS_H












