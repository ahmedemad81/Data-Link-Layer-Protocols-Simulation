//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#include "Coordinator.h"
Define_Module(Coordinator);

void Coordinator::initialize()
{
    scheduleAt(simTime(), new cMessage(""));
}

void Coordinator::handleMessage(cMessage *msg)
{
    // TODO - Generated method body

    string s = "C:/omnetpp-5.6.2/samples/Project_Network/src/coordinator.txt";
    ifstream file (s);
    string x,nodeSelected,delay;
    while (getline(file, x))
    {
       // Process x
       // Get node number
       nodeSelected = x[0];
       delay = x[2];
     }

    cMessage* newMsg = new cMessage();
    double d=stod(delay);
    newMsg->setName(delay.c_str());
    if(nodeSelected=="0")
    sendDelayed(newMsg,d,"outNode0");
    else if (nodeSelected=="1")
    sendDelayed(newMsg,d,"outNode1");

}
