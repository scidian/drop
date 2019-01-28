//
//      Created by Stephens Nunnally on 1/21/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      Helpful misc functions
//
//

#include <cmath>

#include <QtWidgets>
//#include <QWidget>

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
//##        Trimms double to max_decimal_places, and then removes any trailing zeros
//####################################################################################
QString RemoveTrailingDecimals(double value, int max_decimal_places)
{
    QWidget widget;
    double int_part, decimal_part = 0;

    // Remove any extra starting decimal places
    value = widget.locale().toString(value, QLatin1Char('f').unicode(), max_decimal_places).toDouble();

    int count = 0;
    while (count < max_decimal_places) {
        decimal_part = std::modf(value * pow(10, count), &int_part);
        if (decimal_part == 0.0) break;
        ++count;
    }

    return widget.locale().toString(value, QLatin1Char('f').unicode(), count);
}


//####################################################################################
//##        Used to show a modal messagebox
//####################################################################################
void ShowMessageBox(QString new_message)
{
    QMessageBox *msg_box = new QMessageBox(nullptr);
    msg_box->setText(new_message);
    msg_box->exec();

    delete msg_box;
    ///QMessageBox::about(nullptr, "Sample Box", "Shows a message box");
}


}








