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

#ifndef __PROJECT_NETWORK_NODE_H_
#define __PROJECT_NETWORK_NODE_H_

#include <omnetpp.h>
#include "headers.h"
using namespace omnetpp;

/**
 * TODO - Generated class
 */
class Node : public cSimpleModule
{

    vector<string> ErrorCode;
    vector<string> MessageQueue;

    volatile int randLP=int(uniform(0,100));
    //Window variables
    int windowSize=0;
    int left=0;
    int right=0;

    //Go Back N
    int currentSeqNum=0;
    int currentMsg=0;

  protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
    void ReadFile(string File);
    string ByteStuffing(string S);
    string RemoveByteStuffing(string S);
    bitset<8> ParityCal(string S);
    bool ErrorDetection(string S);
    void ReceiveData(MyMessage_Base* recMsg);
    void Timer();
};

#endif
