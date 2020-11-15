package bgu.spl.net.impl.stomp;

import bgu.spl.net.api.MessageEncoderDecoderImpl;
import bgu.spl.net.api.StompMessagingProtocolImpl;
import bgu.spl.net.srv.Server;

public class StompServer {

    public static void main(String[] args) {
        if (args[1].equals("tpc")) {
            //you can use any server...
            Server.threadPerClient(
                    Integer.valueOf(args[0]), //port
                    StompMessagingProtocolImpl::new, //protocol factory
                    MessageEncoderDecoderImpl::new //message encoder decoder factory
            ).serve();
        } else if (args[1].equals("reactor")) {
            Server.reactor(
                    Runtime.getRuntime().availableProcessors(),
                    Integer.valueOf(args[0]), //port
                    StompMessagingProtocolImpl::new, //protocol factory
                    MessageEncoderDecoderImpl::new //message encoder decoder factory
            ).serve();

        }
    }
}
