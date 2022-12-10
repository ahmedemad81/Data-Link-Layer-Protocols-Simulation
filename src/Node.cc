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

#include "Node.h"
Define_Module(Node);

void Node::initialize()
{
    // TODO - Generated method body
}

void Node::handleMessage(cMessage *msg)
{

    if (!strcmp(msg->getName(),"Coordinator0"))
    {

        string s = "C:/omnetpp-5.6.2/samples/Project_Network/src/Input/Node0.txt";
        ReadFile(s);
        sender=0;
        receiver=1;
        scheduleAt(simTime(), new cMessage(""));
    }
    else if (!strcmp(msg->getName(),"Coordinator1"))
    {
        string s = "C:/omnetpp-5.6.2/samples/Project_Network/src/Input/Node1.txt";
        ReadFile(s);
        sender=1;
        receiver=0;
        scheduleAt(simTime(), new cMessage(""));
    }
    else
    {
        MyMessage_Base *mymsg = new MyMessage_Base();
        string ByteStuffed= ByteStuffing(MessageQueue[0]);
        bitset <8> Parity=ParityCal(ByteStuffed);

        mymsg->setMType(Data);
        mymsg->setMPayload(ByteStuffed.c_str());
        mymsg->setMTrailer((Parity.to_string()).c_str());
        send(mymsg,"out");
    }

}

void Node::ReadFile(string File){

          ifstream file (File);
          string x;
          while (getline(file, x))
          {
            ErrorCode.push_back(x.substr(0, 4));
            MessageQueue.push_back(x.substr(5));
          }
          windowSize=getParentModule()->par("WS").intValue()-1;

}

string Node::ByteStuffing(string S)
{
    string newString = "";
    newString.push_back('$');
    for(int i= 0 ; i< S.size();i++)
    {
       if (S[i] == '$' || S[i] =='/')
          {
             newString.push_back('/');
             newString.push_back(S[i]);
           }
       else
        {
            newString.push_back(S[i]);
         }
    }
    newString.push_back('$');
    return newString;
}

bitset<8> Node:: ParityCal(string S)
{
    vector< bitset<8> > bitVector;
    bitset<8> parityByte(0);
    for (int i=0; i < S.size(); i++)
        {
            bitset<8> bit_msg1(S[i]);
            bitVector.push_back(bit_msg1);
        }
    for (int i=0; i < bitVector.size(); i++)
    {
        parityByte ^= bitVector[i];
    }
    return parityByte;
}

bool Node:: ErrorDetection(string S)
{
    bitset<8> Result=ParityCal(S);
    if(Result==0)
        return true;
    else
        return false;
}

