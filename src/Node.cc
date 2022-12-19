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
ofstream MyFile("out.txt");

void Node::initialize()
{
    // TODO - Generated method body
}

void Node::handleMessage(cMessage *msg)
{
    MyMessage_Base* recMsg= (MyMessage_Base *) msg;

    if(recMsg->getMType()==coordinator)
    {
        if (!strcmp(recMsg->getMPayload(),"Coordinator0"))
        {

            string s = "C:/omnetpp-5.6.2/samples/Project_Network/src/Input/Node0.txt";
            ReadFile(s);
            //scheduleAt(simTime(), new cMessage(""));
        }
        else if (!strcmp(recMsg->getMPayload(),"Coordinator1"))
        {
            string s = "C:/omnetpp-5.6.2/samples/Project_Network/src/Input/Node1.txt";
            ReadFile(s);
            //scheduleAt(simTime(), new cMessage(""));
        }
    }

    else if (recMsg->getMType()==Data)
    {

        // Receiver Code
        if (recMsg->getMHeader()==currentSeqNum)
        {
            ReceiveData(recMsg);
            currentSeqNum++;
        }
        else
        {
            MyMessage_Base* ackMsg = new MyMessage_Base();  // (sending same ack (seq -1 ) to sender) ???
            ackMsg->setMHeader(currentSeqNum);
            ackMsg->setMType(ACK);
            if(randLP>(getParentModule()->par("LP").doubleValue()/10))
               {
                //Adding Process time as a self message
                scheduleAt(simTime() + getParentModule()->par("PT").doubleValue(), (cMessage*)ackMsg);
               }
            cancelAndDelete(ackMsg);
        }

    }
    else if (recMsg->getMType()==ACK)
    {
        //sender code
    }
    else if (msg->isSelfMessage())
    {
        //Processing time then send
        sendDelayed((cMessage *)recMsg, getParentModule()->par("TD").doubleValue(), "out");
    }
    else if(recMsg->getMType()==TimeOut)
    {
        if (recMsg->getMHeader()==currentMsg)
        {
            //send again
        }
        cancelAndDelete(msg);
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

string Node::RemoveByteStuffing(string S)
{
    string newString = S.substr(1,S.size()-2);
    for(int i= 0 ; i< S.size();i++)
    {
       if (S[i] =='/')
          {
             continue;
          }
        newString.push_back(S[i]);

    }
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

void Node:: ReceiveData(MyMessage_Base* recMsg)
{
    MyMessage_Base* sendMsg=new MyMessage_Base();

    string msgPayLoad=recMsg->getMPayload();
    bitset<8> msgParity=recMsg->getMTrailer();
    bool header=ErrorDetection(msgParity.to_string());

    if(header==0)
        sendMsg->setMHeader(NACK);
    else
        sendMsg->setMHeader(ACK);

    if(randLP>(getParentModule()->par("LP").doubleValue()/10))
    {
        //Adding Process time as a self message
        scheduleAt(simTime() + getParentModule()->par("PT").doubleValue(), (cMessage*)recMsg);
    }

    cancelAndDelete(sendMsg);


}

void Node::Timer(){
    MyMessage_Base *myMsg = new MyMessage_Base();
    myMsg->setMType(TimeOut);
    myMsg->setMHeader(currentMsg);
    //Timer gets expired after TO
    scheduleAt(simTime() + getParentModule()->par("TO").doubleValue(), (cMessage*)myMsg);

    cancelAndDelete(myMsg);
}


