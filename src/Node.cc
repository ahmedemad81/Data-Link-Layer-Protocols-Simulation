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

//this is forbidden figure out smth else?????
MyMessage_Base *myTimer = new MyMessage_Base();

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
            string s = "D:/UNI/CCE sem 5/Networks project/Data-Link-Layer-Protocols-Simulation/src/Input/Node0.txt";
            ReadFile(s);
            for(int i = left; i< right + 1; i++)
            {
                if(i>=MessageQueue.size())
                    break;
                SendData(i);
            }
            Timer();
            //scheduleAt(simTime(), new cMessage(""));
        }
        else if (!strcmp(recMsg->getMPayload(),"Coordinator1"))
        {
            string s = "D:/UNI/CCE sem 5/Networks project/Data-Link-Layer-Protocols-Simulation/src/Input/Node1.txt";
            ReadFile(s);
            for(int i = left; i < right + 1; i++)
              {
                if(i>=MessageQueue.size())
                    break;
                SendData(i);
              }
            Timer();
            //scheduleAt(simTime(), new cMessage(""));
        }
    }

    else if (recMsg->getMType()==Data)
    {
        EV<<"receiver received data"<<std::endl;
        // Receiver Code
        if (recMsg->getMHeader()==currentSeqNum)
        {
            EV << "this is the current seqNum at receiver if its correct: "<<currentSeqNum << std::endl;
            ReceiveData(recMsg);
        }
        else
        {
            EV << "It is not correct data" << std::endl;
            EV << "this is the wrong receieved seq number: "<<recMsg->getMHeader()<<std::endl;
            MyMessage_Base* ackMsg = new MyMessage_Base();  // (sending same ack (seq -1 ) to sender
            ackMsg->setMHeader(currentSeqNum);
            ackMsg->setMType(NACK);
            string Loss="Yes";
            if(randLP>(par("LP").doubleValue()))
               {
                  //Adding Process time as a self message
                  sendDelayed((cMessage *)ackMsg, par("PT").doubleValue() + par("TD").doubleValue(), "out");
                  Loss="No";
               }
            MyFile << "At time "<< simTime()+par("PT").doubleValue() <<", "<<getName()<<" sending NACK" << " with number["<<recMsg->getMHeader() % par("WS").intValue()<<"], loss ["<< Loss <<" ]"<<endl<<endl;


            //cancelAndDelete(ackMsg);
        }

    }
    else if (recMsg->getMType()==ACK)
    {
        EV<<"Ack received"<< std::endl;
        EV << recMsg->getMHeader() << std::endl;
        //sender code
        if(recMsg->getMHeader() == left )
        {
            EV<<"correct Ack received"<< std::endl;
            windowCounter++;
            if(recMsg->getMHeader() < MessageQueue.size())
            {
                left++;
                right++;
                currentMsg++;
            }
            EV << "window counter: "<< windowCounter << std::endl;

            //time out for when the window is sent and all acks are received correctly
            if(windowCounter == par("WS").intValue())
            {
                // here we need to cancel the timeout that was called at the beginning of the cycle somehow?
                windowCounter = 0;
                MyMessage_Base *myMsg = new MyMessage_Base();
                myMsg->setMType(TimeOut);
                myMsg->setMHeader(currentMsg);
                if(left>=MessageQueue.size())
                   return;
                scheduleAt(simTime(), (cMessage*)myMsg);
                cancelAndDelete(myTimer);
            }
        }
        else
        {
            EV << "received Ack wrong: " << recMsg->getMHeader() << std::endl;
        }
        EV << "new left:" << left << std::endl;
        EV << "new right:" << right << std::endl;
    }
    else if(recMsg->getMType()==TimeOut)
    {
        EV<<"WE GOT A TIMEOUT"<< std::endl;
        //if (recMsg->getMHeader()==currentMsg) //revert comment
        //{
           for(int i = left; i< right + 1; i++)
           {
               SendData(i);
           }
            Timer();
        //}
        //cancelAndDelete(msg);
    }
    else if(recMsg->getMType() == DelayedMessage)
    {
        recMsg->setMType(Data);
        sendDelayed((cMessage *)recMsg, par("TD").doubleValue(), "out");
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
          windowSize=par("WS").intValue()-1;

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

bool Node:: ErrorDetection(string S, std::bitset<8> trailer)
{
    bitset<8> Result=ParityCal(S);
    bitset<8> parityByte = Result ^ trailer;
    //EV<<"message sent to error detection: "<<S<<std::endl;
    //EV<<"parity calc: "<<Result<<std::endl;
    if(parityByte==0)
        return true;
    else
        return false;
}

void Node:: ReceiveData(MyMessage_Base* recMsg)
{
    EV << "Message received:" << recMsg->getMPayload() << std::endl;
    EV << "this is the received sequence number: " << recMsg->getMHeader() << std::endl;

    MyMessage_Base* sendMsg=new MyMessage_Base();

    string msgPayLoad=recMsg->getMPayload();
    bitset<8> msgParity=recMsg->getMTrailer();
    bool header=ErrorDetection(msgPayLoad,msgParity);
    string type="";
    string Loss="Yes";
    if(header==0)
    {
        EV<<"setting as NACK" << std::endl;
        //EV<<"Read message payload:"<<recMsg->getMPayload()<< std::endl;
        //EV<<"trailer: "<<recMsg->getMTrailer().to_string()<<std::endl;
        sendMsg->setMHeader(currentSeqNum);
        sendMsg->setMType(NACK);
        type="NACK";
    }
    else
    {
        EV<<"setting as ACK" << std::endl;
        sendMsg->setMHeader(currentSeqNum);
        sendMsg->setMType(ACK);
        type="ACK";
        currentSeqNum += 1;
    }

   if(randLP>(par("LP").doubleValue()))
    {
        //Adding Process time as a self message
        sendDelayed((cMessage *)sendMsg, par("PT").doubleValue() + par("TD").doubleValue(), "out");
        Loss="No";
    }

    MyFile << "At time "<< simTime()+par("PT").doubleValue() <<", "<<getName()<<" sending "<< type << " with number["<<recMsg->getMHeader() % par("WS").intValue()<<"], loss ["<< Loss <<" ]"<< endl << endl;


    //cancelAndDelete(sendMsg); //revert comment
}

void Node::SendData(int i){
    if(i >= MessageQueue.size())
        return;
    MyFile <<"At ["<<simTime()<<"] , "<<getName()<<" , Introducing channel error with code =["<<ErrorCode[i]<<"]"<<endl <<endl;
    int errorflag=-1;
    string lostflag="No";
    int dupflag=0;
    int errorDelay=0;

    std::bitset<8> trailer;
    EV <<"This is the lower frame side:" << left <<std::endl;
    EV << "This is the upper frame:" << right << std::endl;
    EV << "---------------------------------" << std::endl;
    string stringMessage = MessageQueue[i];
    EV<<"this is the message from file:"<<stringMessage << std::endl;
    string errorCodes = ErrorCode[i];
    EV<<"Error code:" << errorCodes << std::endl;

    //check if this packet should be lost
    if(errorCodes[1] == '1')
    {
        //ErrorCode[i][1] = '0'; //to avoid continuously losing the same packet over and over (reset error)
        EV << "packet_loss"<< std::endl;
        lostflag="Yes";
    }
    else{
        MyMessage_Base* sendMsg=new MyMessage_Base();

        //type of message
        sendMsg->setMType(Data);

        //sequence number
        sendMsg->setMHeader(i);
        EV << "this is the sent sequence number: "<< i << std::endl;

        //Add framing
        string framedMessage = ByteStuffing(stringMessage);
        //EV<<"this is the framed message: " <<framedMessage<<std::endl;

        //calculate parity
        trailer = ParityCal(framedMessage);
        //EV<<"this is the trialer: " <<trailer<<std::endl;
        sendMsg->setMTrailer(trailer);

        //check modification
        if(errorCodes[0] == '1'){
           framedMessage = ModifyMessage(framedMessage);
           errorflag=1;
        }

        //set pay-load
        sendMsg->setMPayload(framedMessage.c_str());
        //EV<<"this is the framed message to c_str: "<<framedMessage.c_str()<<std::endl;

        //send message
        //original delay
        simtime_t sendDelay = par("PT").doubleValue()+par("TD").doubleValue();
        EV << "PT: "<<par("PT").doubleValue() << std::endl;
        EV << "TD: "<<par("TD").doubleValue() << std::endl;
        EV << "send delay: " << sendDelay <<std::endl;

        //if there is errorDelay
        if(errorCodes[3] == '1')
        {
            EV<<"there is a delay" << std::endl;
            sendDelay += par("ED").doubleValue();
            errorDelay= par("ED").doubleValue();
        }
        int acummilativeParam = i;
        if(i >= par("WS").intValue())
        {
            acummilativeParam = i - par("WS").intValue();
        }
        //scheduleAt(simTime() + i*sendDelay+par("TD").doubleValue(), (cMessage*)sendMsg);
        sendDelayed((cMessage *)sendMsg, acummilativeParam* par("PT").doubleValue()+sendDelay, "out");

        //duplication
        if(errorCodes[2] == '1')
        {
            dupflag=1;
            MyMessage_Base* duplicateMsg=new MyMessage_Base();
            duplicateMsg->setMHeader(sendMsg->getMHeader());
            duplicateMsg->setMPayload(sendMsg->getMPayload());
            duplicateMsg->setMTrailer(sendMsg->getMTrailer());
            duplicateMsg->setMType(Data);
            EV << "There is duplication" << std::endl;
            sendDelayed((cMessage *)duplicateMsg, acummilativeParam* par("PT").doubleValue()+sendDelay + par("DD").doubleValue(), "out");

        }
        int seqNum = (sendMsg->getMHeader()) % par("WS").intValue();
        MyFile<<"At time ["<<simTime()+i* par("PT").doubleValue() + par("PT").doubleValue()<<"], "<<getName()<< " [sent] frame with seq_num=["<<seqNum<<"] and payload=[ "<<sendMsg->getMPayload() <<"] and trailer=[ "<<sendMsg->getMTrailer().to_string()<< "] , Modified ["<<errorflag<<" ] , Lost ["<<lostflag<<"], Duplicate ["<<dupflag<<"], Delay["<<errorDelay<<"]."<<endl<<endl;
        if(dupflag==1)
            MyFile<<"At time ["<<simTime()+i* par("PT").doubleValue()+ par("PT").doubleValue()<<"], "<<getName()<< " [sent] frame with seq_num=["<<seqNum<<"] and payload=[ "<<sendMsg->getMPayload() <<"] and trailer=[ "<<sendMsg->getMTrailer().to_string()<< "] , Modified ["<<errorflag<<" ] , Lost ["<<lostflag<<"], Duplicate ["<<dupflag+1<<"], Delay["<<errorDelay+par("DD").doubleValue()<<"]."<<endl<<endl;
    }

    ErrorCode[i] = "0000";
}

string Node::ModifyMessage(string message){
   vector< bitset<8> > bitVector;
   string modifiedMessage ="";
   for (int i=0; i < message.size(); i++)
       {
           bitset<8> bit_msg1(message[i]);
           bitVector.push_back(bit_msg1);
       }
   bitVector[1][2] = !bitVector[1][2];

   for (int i = 0; i< bitVector.size(); i++){
       char character = (char)bitVector[i].to_ulong();
       modifiedMessage += character;
   }
   EV << "Modified message: " << modifiedMessage << std::endl;
   return modifiedMessage;
}


void Node::Timer(){

    myTimer->setMType(TimeOut);
    myTimer->setMHeader(currentMsg);
    if(left>=MessageQueue.size())
        return;
    //Timer gets expired after TO
    scheduleAt(simTime() + par("TO").intValue(), (cMessage*)myTimer);

    //cancelAndDelete(myMsg); //revert comment
}


