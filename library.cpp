//
//      Created by Stephens Nunnally on 1/21/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      Helpful misc functions
//
//

#include <QGraphicsDropShadowEffect>

#include "colors.h"
#include "library.h"

namespace Dr {


//####################################################################################
//##        Returns true if 'number_desired' is within +-'tolerance' of 'number_to_check'
//####################################################################################
//
bool IsCloseTo(double number_desired, double number_to_check, double tolerance)
{
    return ( (number_to_check <= (number_desired + tolerance)) && (number_to_check >= (number_desired - tolerance)) );
}


//####################################################################################
//##        Used to show a modal messagebox
//####################################################################################
void ShowMessageBox(QString new_message)
{
    QMessageBox *msgBox = new QMessageBox(nullptr);
    msgBox->setText(new_message);
    msgBox->exec();
}


}








