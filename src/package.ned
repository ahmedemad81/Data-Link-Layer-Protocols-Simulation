package projectnetwork;

@license(LGPL);
//
// TODO documentation
//
network Main
{
    @display("bgb=396,222");
    submodules:
        coordinator: Coordinator {
            @display("p=193,124;i=abstract/penguin");
        }
        Node0: Node {
            @display("p=93,48;i=block/broadcast");
        }
        Node1: Node {
            @display("p=299,48;i=block/network2");
        }
    connections:
        coordinator.outNode1 --> Node1.inCoordinator;
        coordinator.outNode0 --> Node0.inCoordinator;
        Node0.out --> Node1.inNode;
        Node1.out --> Node0.inNode;
}

