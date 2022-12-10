package projectnetwork;

@license(LGPL);
//
// TODO documentation
//
network Main
{
    parameters:
    int NodeID=default(0);
    int WS=default(5);
	int TO=default(10);
	double PT=default(0.5);
	double TD=default(1);
	double ED=default(4);
	double DD=default(0.1);
	double LP=default(0.1);
	
    @display("bgb=396,222");
    submodules:
        coordinator: Coordinator {
            @display("p=193,124;i=abstract/penguin");
        }
        node0: Node {
            @display("p=93,48;i=block/broadcast");
        }
        node1: Node {
            @display("p=299,48;i=block/network2");
        }
    connections:
        coordinator.outNode1 --> node1.inCoordinator;
        coordinator.outNode0 --> node0.inCoordinator;
        node0.out --> node1.inNode;
        node1.out --> node0.inNode;
}

